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
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Creating child process for full-duplex chat...\n");

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        // Child process - Handles sending messages
        printf("Client Child Process: PID = %d, PPID = %d\n", getpid(), getppid());

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            printf("Client: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }
    else
    {
        // Parent process - Handles receiving messages
        printf("Client Parent Process: PID = %d\n", getpid());

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            recv(sockfd, buffer, BUFFER_SIZE, 0);
            printf("Server: %s", buffer);
        }
    }

    close(sockfd);
    return 0;
}
