#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server!\n");

    while (1)
    {
        int choice, n, search_key;
        printf("\n1. Search\n2. Sort Ascending\n3. Sort Descending\n4. Split Odd/Even\n5. Exit\nEnter choice: ");
        scanf("%d", &choice);

        if (choice == 5)
        {
            send(sock, "exit", 4, 0);
            break;
        }

        printf("Enter number of elements: ");
        scanf("%d", &n);

        int arr[n];
        printf("Enter elements: ");
        for (int i = 0; i < n; i++)
        {
            scanf("%d", &arr[i]);
        }

        if (choice == 1)
        {
            printf("Enter number to search: ");
            scanf("%d", &search_key);
            arr[n++] = search_key; // Append search key
        }

        sprintf(buffer, "%d %d", choice, n);
        for (int i = 0; i < n; i++)
        {
            char temp[10];
            sprintf(temp, " %d", arr[i]);
            strcat(buffer, temp);
        }

        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);

        printf("Server Response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
