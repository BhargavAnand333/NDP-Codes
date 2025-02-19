#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to check if a string is a palindrome
int is_palindrome(char *str)
{
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++)
    {
        if (str[i] != str[len - i - 1])
        {
            return 0; // Not a palindrome
        }
    }
    return 1; // Is a palindrome
}

// Function to count vowel occurrences
void count_vowels(char *str, int *vowel_count)
{
    char vowels[] = "aeiouAEIOU";
    memset(vowel_count, 0, 5 * sizeof(int));

    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (str[i] == vowels[j])
            {
                vowel_count[j % 5]++; // Normalize uppercase and lowercase vowels
                break;
            }
        }
    }
}

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

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        // Receive message from client
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        // Check for termination condition
        if (strncmp(buffer, "Halt", 4) == 0)
        {
            printf("Received 'Halt'. Terminating server.\n");
            break;
        }

        // Process received string
        int len = strlen(buffer);
        int palindrome = is_palindrome(buffer);
        int vowel_count[5] = {0};
        count_vowels(buffer, vowel_count);

        // Prepare response
        char response[BUFFER_SIZE];
        snprintf(response, BUFFER_SIZE,
                 "Palindrome: %s\nLength: %d\nVowel Counts -> a:%d e:%d i:%d o:%d u:%d",
                 palindrome ? "Yes" : "No", len, vowel_count[0], vowel_count[1], vowel_count[2], vowel_count[3], vowel_count[4]);

        // Send response back to client
        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    // Close socket
    close(sockfd);
    return 0;
}
