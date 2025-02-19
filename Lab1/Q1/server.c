#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to search for an element
int search(int arr[], int n, int key)
{
    for (int i = 0; i < n; i++)
    {
        if (arr[i] == key)
        {
            return 1; // Found
        }
    }
    return 0; // Not Found
}

// Function to sort array in ascending order
void sort(int arr[], int n, int ascending)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if ((ascending && arr[j] > arr[j + 1]) || (!ascending && arr[j] < arr[j + 1]))
            {
                // Swap elements
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Function to split numbers into odd and even
void split(int arr[], int n, char *response)
{
    char odd[BUFFER_SIZE] = "Odd: ";
    char even[BUFFER_SIZE] = "Even: ";
    char temp[20];

    for (int i = 0; i < n; i++)
    {
        sprintf(temp, "%d ", arr[i]);
        if (arr[i] % 2 == 0)
        {
            strcat(even, temp);
        }
        else
        {
            strcat(odd, temp);
        }
    }

    // Concatenate results
    snprintf(response, BUFFER_SIZE, "%s\n%s", odd, even);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

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

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0)
    {
        perror("Client connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Client connected!\n");

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);

        // Check for exit condition
        if (strncmp(buffer, "exit", 4) == 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        int choice, n, search_key;
        sscanf(buffer, "%d %d", &choice, &n);
        int arr[n];

        char *token = strtok(buffer + 4, " ");
        for (int i = 0; i < n; i++)
        {
            arr[i] = atoi(token);
            token = strtok(NULL, " ");
        }

        char response[BUFFER_SIZE] = "";

        if (choice == 1)
        {                            // Search
            search_key = arr[n - 1]; // Last value is search key
            if (search(arr, n - 1, search_key))
            {
                sprintf(response, "Number %d found", search_key);
            }
            else
            {
                sprintf(response, "Number %d not found", search_key);
            }
        }
        else if (choice == 2 || choice == 3)
        { // Sort Ascending/Descending
            sort(arr, n, choice == 2);
            sprintf(response, "Sorted Array: ");
            for (int i = 0; i < n; i++)
            {
                char temp[10];
                sprintf(temp, "%d ", arr[i]);
                strcat(response, temp);
            }
        }
        else if (choice == 4)
        { // Split into odd/even
            split(arr, n, response);
        }

        send(client_fd, response, strlen(response), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
