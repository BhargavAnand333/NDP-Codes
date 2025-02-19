#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);
    char buffer[BUFFER_SIZE];

    // Creating UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configuring server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // Get user input
        printf("Enter a string (or 'Halt' to stop): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

        // Send message to server
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, addr_len);

        // If "Halt" is sent, terminate the client
        if (strcmp(buffer, "Halt") == 0)
        {
            printf("Terminating client...\n");
            break;
        }

        printf("Permutations received from server:\n");

        while (1)
        {
            memset(buffer, 0, BUFFER_SIZE);
            recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);

            // If server sends "END", stop receiving
            if (strcmp(buffer, "END") == 0)
            {
                break;
            }

            printf("%s\n", buffer);
        }
    }

    close(sockfd);
    return 0;
}
