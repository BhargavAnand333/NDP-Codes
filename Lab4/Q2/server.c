#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "database.txt"

// Function to look up IP for a given domain
char *lookup_dns(const char *domain)
{
    static char ip[BUFFER_SIZE];
    FILE *file = fopen(DB_FILE, "r");

    if (!file)
    {
        perror("Database file not found");
        return "Error: DNS Database unavailable";
    }

    char file_domain[BUFFER_SIZE], file_ip[BUFFER_SIZE];
    while (fscanf(file, "%s %s", file_domain, file_ip) != EOF)
    {
        if (strcmp(domain, file_domain) == 0)
        {
            fclose(file);
            strcpy(ip, file_ip);
            return ip;
        }
    }

    fclose(file);
    return "Domain not found";
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

    printf("DNS Server running on port %d...\n", PORT);

    while (1)
    {
        // Accept client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0)
        {
            perror("Client connection failed");
            continue;
        }

        // Read hostname from client
        memset(buffer, 0, BUFFER_SIZE);
        read(client_fd, buffer, BUFFER_SIZE);
        printf("Received request for: %s\n", buffer);

        // Lookup DNS
        char *ip_address = lookup_dns(buffer);

        // Send response
        write(client_fd, ip_address, strlen(ip_address));

        // Close client connection
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
