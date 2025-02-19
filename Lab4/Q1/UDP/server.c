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
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1)
    {
        // Receive request
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        int option;
        char data[BUFFER_SIZE];
        sscanf(buffer, "%d %[^\n]", &option, data);

        char response[BUFFER_SIZE];

        if (option == 1)
        {
            snprintf(response, sizeof(response), "Name: John Doe, Address: 123 Main St, NY");
        }
        else if (option == 2)
        {
            snprintf(response, sizeof(response), "Dept: CSE, Semester: 5, Section: B, Courses: DSA, OS, Networks");
        }
        else if (option == 3)
        {
            snprintf(response, sizeof(response), "Subject: %s, Marks: 85", data);
        }
        else
        {
            snprintf(response, sizeof(response), "Invalid option!");
        }

        // Send response
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}
