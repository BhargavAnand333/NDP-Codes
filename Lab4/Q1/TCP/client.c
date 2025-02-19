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

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    // User input
    printf("Select an option:\n1. Registration Number\n2. Name of Student\n3. Subject Code\nEnter choice: ");
    int option;
    char data[BUFFER_SIZE];

    scanf("%d", &option);
    getchar(); // Consume newline

    if (option == 3)
    {
        printf("Enter Subject Code: ");
        fgets(data, BUFFER_SIZE, stdin);
        data[strcspn(data, "\n")] = '\0';
    }
    else
    {
        strcpy(data, "N/A");
    }

    // Send request
    snprintf(buffer, sizeof(buffer), "%d %s", option, data);
    write(sockfd, buffer, strlen(buffer));

    // Receive response
    memset(buffer, 0, BUFFER_SIZE);
    read(sockfd, buffer, BUFFER_SIZE);
    printf("Response: %s\n", buffer);

    // Close socket
    close(sockfd);
    return 0;
}
