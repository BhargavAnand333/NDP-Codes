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

// Function to sort an array in ascending order
void sort_ascending(char *str, int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (str[i] > str[j])
            {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

// Function to sort an array in descending order
void sort_descending(char *str, int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (str[i] < str[j])
            {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Creating socket
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

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0)
    {
        perror("Client connection failed");
        exit(EXIT_FAILURE);
    }

    // Receive alphanumeric string from client
    memset(buffer, 0, BUFFER_SIZE);
    read(client_fd, buffer, BUFFER_SIZE);
    printf("Received string from client: %s\n", buffer);

    // Fork to create child process
    pid_t pid = fork();

    if (pid == 0)
    { // Child Process
        char digits[BUFFER_SIZE];
        int d_len = 0;

        // Extract digits
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (buffer[i] >= '0' && buffer[i] <= '9')
            {
                digits[d_len++] = buffer[i];
            }
        }
        digits[d_len] = '\0';

        // Sort digits in ascending order
        sort_ascending(digits, d_len);

        // Prepare result string with process ID
        char result[BUFFER_SIZE];
        snprintf(result, sizeof(result), "Child (PID: %d) sorted numbers: %s", getpid(), digits);

        // Send result to client
        write(client_fd, result, strlen(result));

        exit(0); // Exit child process
    }
    else
    { // Parent Process
        char characters[BUFFER_SIZE];
        int c_len = 0;

        // Extract characters
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))
            {
                characters[c_len++] = buffer[i];
            }
        }
        characters[c_len] = '\0';

        // Sort characters in descending order
        sort_descending(characters, c_len);

        // Prepare result string with process ID
        char result[BUFFER_SIZE];
        snprintf(result, sizeof(result), "Parent (PID: %d) sorted characters: %s", getpid(), characters);

        // Send result to client
        write(client_fd, result, strlen(result));

        // Wait for child process to terminate
        wait(NULL);
    }

    // Close sockets
    close(client_fd);
    close(server_fd);
    return 0;
}
