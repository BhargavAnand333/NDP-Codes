#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to check if a file exists
int file_exists(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

// Function to count occurrences of a string in a file
int count_occurrences(char *filename, char *search_str)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return -1;

    char buffer[BUFFER_SIZE];
    int count = 0;
    while (fscanf(file, "%s", buffer) != EOF)
    {
        if (strcmp(buffer, search_str) == 0)
        {
            count++;
        }
    }
    fclose(file);
    return count;
}

// Function to replace a string in a file
int replace_string(char *filename, char *old_str, char *new_str)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return -1;

    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
        return -1;

    char buffer[BUFFER_SIZE];
    int replaced = 0;

    while (fscanf(file, "%s", buffer) != EOF)
    {
        if (strcmp(buffer, old_str) == 0)
        {
            fprintf(temp, "%s ", new_str);
            replaced = 1;
        }
        else
        {
            fprintf(temp, "%s ", buffer);
        }
    }

    fclose(file);
    fclose(temp);

    if (!replaced)
        return 0;

    remove(filename);
    rename("temp.txt", filename);

    return 1;
}

// Function to reorder file contents by ASCII values
void reorder_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return;

    char words[100][BUFFER_SIZE];
    int count = 0;

    while (fscanf(file, "%s", words[count]) != EOF)
    {
        count++;
    }
    fclose(file);

    // Sort words based on ASCII values
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = i + 1; j < count; j++)
        {
            if (strcmp(words[i], words[j]) > 0)
            {
                char temp[BUFFER_SIZE];
                strcpy(temp, words[i]);
                strcpy(words[i], words[j]);
                strcpy(words[j], temp);
            }
        }
    }

    // Write sorted words back to the file
    file = fopen(filename, "w");
    for (int i = 0; i < count; i++)
    {
        fprintf(file, "%s ", words[i]);
    }
    fclose(file);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create TCP socket
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
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Receive filename
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);

        if (!file_exists(buffer))
        {
            send(client_fd, "File not present", strlen("File not present"), 0);
            close(client_fd);
            continue;
        }

        send(client_fd, "File found", strlen("File found"), 0);

        while (1)
        {
            // Receive option
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            int option = atoi(buffer);

            if (option == 4)
            {
                printf("Client requested exit.\n");
                break;
            }
            else if (option == 1)
            {
                // Search string
                recv(client_fd, buffer, BUFFER_SIZE, 0);
                int count = count_occurrences(buffer, buffer);
                char response[BUFFER_SIZE];
                snprintf(response, BUFFER_SIZE, count > 0 ? "String found %d times" : "String not found", count);
                send(client_fd, response, strlen(response), 0);
            }
            else if (option == 2)
            {
                // Replace string
                char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
                recv(client_fd, str1, BUFFER_SIZE, 0);
                recv(client_fd, str2, BUFFER_SIZE, 0);

                int replaced = replace_string(buffer, str1, str2);
                send(client_fd, replaced ? "String replaced" : "String not found", strlen(replaced ? "String replaced" : "String not found"), 0);
            }
            else if (option == 3)
            {
                // Reorder file
                reorder_file(buffer);
                send(client_fd, "File reordered", strlen("File reordered"), 0);
            }
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
