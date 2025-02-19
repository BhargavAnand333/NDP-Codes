#include <stdio.h>
#include <string.h>

// Function to count the number of 1s in a binary string
int count_ones(const char *data)
{
    int count = 0;
    for (int i = 0; data[i] != '\0'; i++)
    {
        if (data[i] == '1')
        {
            count++;
        }
    }
    return count;
}

// Function to append the parity bit
void add_parity(char *data, char parity_type)
{
    int ones_count = count_ones(data);
    int len = strlen(data);

    // Adding parity bit based on type
    if (parity_type == 'O')
    { // Odd parity
        data[len] = (ones_count % 2 == 0) ? '1' : '0';
    }
    else
    { // Even parity
        data[len] = (ones_count % 2 == 0) ? '0' : '1';
    }
    data[len + 1] = '\0'; // Null terminate the string
}

// Function to check if received data is correct
int check_parity(const char *received_data, char parity_type)
{
    int ones_count = count_ones(received_data);

    // Check based on parity type
    if (parity_type == 'O')
    { // Odd parity
        return (ones_count % 2 != 0);
    }
    else
    { // Even parity
        return (ones_count % 2 == 0);
    }
}

int main()
{
    char data[100], received_data[100], parity_type;

    // Input binary data
    printf("Enter binary data to send: ");
    scanf("%s", data);

    // Select parity type
    printf("Choose parity type (O for Odd, E for Even): ");
    scanf(" %c", &parity_type);

    // Add parity bit at sender side
    char transmitted_data[105];
    strcpy(transmitted_data, data);
    add_parity(transmitted_data, parity_type);
    printf("Transmitted Data (with parity bit): %s\n", transmitted_data);

    // Simulating transmission to receiver
    printf("Enter received binary data: ");
    scanf("%s", received_data);

    // Check if data is corrupted
    if (check_parity(received_data, parity_type))
    {
        printf(" Data received correctly!\n");
    }
    else
    {
        printf(" Data corrupted!\n");
    }

    return 0;
}
