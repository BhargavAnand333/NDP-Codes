#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to swap two characters
void swap(char *a, char *b)
{
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Function to generate permutations recursively
void permute(char *str, int left, int right, int sockfd, struct sockaddr_in *client_addr, socklen_t addr_len)
{
    if (left == right)
    {
        sendto(sockfd, str, strlen(str), 0, (struct sockaddr *)client_addr, addr_len);
        usleep(10000); // Small delay to avoid packet loss
        return;
    }
    for (int i = left; i <= right; i++)
    {
        swap((str + left), (str + i));
        permute(str, left + 1, right, sockfd, client_addr, addr_len);
        swap((str + left), (str + i)); // Backtrack
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
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
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server is running on port %d...\n", PORT);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        printf("Received from client: %s\n", buffer);

        // If "Halt" is received, terminate the server
        if (strcmp(buffer, "Halt") == 0)
        {
            printf("Terminating server...\n");
            break;
        }

        // Calculate and send permutations
        int length = strlen(buffer);
        permute(buffer, 0, length - 1, sockfd, &client_addr, addr_len);

        // Send termination signal
        char end_msg[] = "END";
        sendto(sockfd, end_msg, strlen(end_msg), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}
