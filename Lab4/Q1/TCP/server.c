#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle client requests
void handle_request(int client_fd)
{
    char buffer[BUFFER_SIZE];

    // Read request from client
    read(client_fd, buffer, BUFFER_SIZE);

    int option;
    char data[BUFFER_SIZE];
    sscanf(buffer, "%d %[^\n]", &option, data);

    pid_t pid = fork();

    if (pid == 0)
    { // Child process
        char response[BUFFER_SIZE];

        if (option == 1)
        { // Registration Number
            snprintf(response, sizeof(response), "Child (PID: %d): Name - John Doe, Address - 123 Main St, NY", getpid());
        }
        else if (option == 2)
        { // Name of Student
            snprintf(response, sizeof(response), "Child (PID: %d): Dept - CSE, Semester - 5, Section - B, Courses: DSA, OS, Networks", getpid());
        }
        else if (option == 3)
        { // Subject Code
            snprintf(response, sizeof(response), "Child (PID: %d): Subject - %s, Marks - 85", getpid(), data);
        }
        else
        {
            snprintf(response, sizeof(response), "Invalid option!");
        }

        write(client_fd, response, strlen(response));
        exit(0);
    }
    else
    {               // Parent process
        wait(NULL); // Wait for child to finish
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

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

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("TCP Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Client connection failed");
            continue;
        }

        handle_request(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
