#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create TCP socket
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

    // Send filename
    printf("Enter filename: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
    send(sockfd, buffer, strlen(buffer), 0);

    // Receive server response
    memset(buffer, 0, BUFFER_SIZE);
    recv(sockfd, buffer, BUFFER_SIZE, 0);
    printf("Server: %s\n", buffer);

    if (strcmp(buffer, "File not present") == 0)
    {
        close(sockfd);
        return 0;
    }

    while (1)
    {
        printf("\nMenu:\n1. Search\n2. Replace\n3. Reorder\n4. Exit\nChoice: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sockfd, buffer, strlen(buffer), 0);

        if (atoi(buffer) == 4)
            break;

        // Additional inputs for options
        if (atoi(buffer) == 1)
        {
            printf("Enter string to search: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        memset(buffer, 0, BUFFER_SIZE);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
