#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected. Creating child process for full-duplex chat...\n");

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process - Handles sending messages
        printf("Server Child Process: PID = %d, PPID = %d\n", getpid(), getppid());

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Server: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(client_fd, buffer, strlen(buffer), 0);
        }
    }
    else
    {
        // Parent process - Handles receiving messages
        printf("Server Parent Process: PID = %d\n", getpid());

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            printf("Client: %s", buffer);
        }
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
