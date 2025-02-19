#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to sort a string
void sort_string(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (tolower(str[i]) > tolower(str[j]))
            {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

// Function to check if two strings are anagrams
int are_anagrams(char *str1, char *str2)
{
    if (strlen(str1) != strlen(str2))
        return 0;
    sort_string(str1);
    sort_string(str2);
    return (strcmp(str1, str2) == 0);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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

    // Listen for client connections
    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Log the connection
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        printf("Connected to client: %s:%d at %02d:%02d:%02d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),
               t->tm_hour, t->tm_min, t->tm_sec);

        // Receive strings from client
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE);

        // Extract two strings
        char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
        sscanf(buffer, "%s %s", str1, str2);

        // Check for anagram
        char result[BUFFER_SIZE];
        if (are_anagrams(str1, str2))
        {
            snprintf(result, BUFFER_SIZE, "The strings '%s' and '%s' are anagrams.", str1, str2);
        }
        else
        {
            snprintf(result, BUFFER_SIZE, "The strings '%s' and '%s' are NOT anagrams.", str1, str2);
        }

        // Send result back to client
        write(client_fd, result, strlen(result));

        // Close client connection
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
