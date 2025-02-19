#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 2

int client_count = 0;

void handle_client(int client_socket, struct sockaddr_in client_addr)
{
    char buffer[BUFFER_SIZE];
    FILE *file;

    // Read client message
    memset(buffer, 0, BUFFER_SIZE);
    read(client_socket, buffer, BUFFER_SIZE);

    printf("Received: %s from %s:%d\n", buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // If too many clients, terminate session
    if (client_count > MAX_CLIENTS)
    {
        char msg[] = "terminate session";
        write(client_socket, msg, strlen(msg));
        close(client_socket);
        return;
    }

    // Open file and append message
    file = fopen("data.txt", "a");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "%s ", buffer);
    fclose(file);

    // If both clients have connected, display result
    if (client_count == MAX_CLIENTS)
    {
        file = fopen("data.txt", "r");
        if (file == NULL)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        char content[BUFFER_SIZE] = "Manipal ";
        char temp[BUFFER_SIZE];

        while (fscanf(file, "%s", temp) != EOF)
        {
            strcat(content, temp);
            strcat(content, " ");
        }
        fclose(file);

        printf("Final Output: %s\n", content);
    }

    close(client_socket);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Create server socket
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

    // Bind server socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for clients
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        client_count++;

        // Fork process to handle client
        if (fork() == 0)
        {
            close(server_fd);
            handle_client(client_fd, client_addr);
            exit(0);
        }

        close(client_fd);
        waitpid(-1, NULL, WNOHANG); // Clean up zombie processes
    }

    close(server_fd);
    return 0;
}
