#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function to calculate the number of redundant bits needed
int calculateRedundantBits(int m)
{
    int r = 0;
    while ((1 << r) < (m + r + 1))
    {
        r++;
    }
    return r;
}

// Function to calculate and set parity bits
void setParityBits(int *data, int m, int r)
{
    for (int i = 0; i < r; i++)
    {
        int pos = (1 << i); // Position of the parity bit
        int parity = 0;

        for (int j = 1; j <= (m + r); j++)
        {
            if (j & pos)
            {                      // Check if the bit at position `j` contributes to parity `i`
                parity ^= data[j]; // XOR operation to compute parity
            }
        }

        data[pos] = parity; // Assign computed parity bit
    }
}

// Function to insert data bits into the correct positions
void insertDataBits(int *data, int *input, int m, int r)
{
    int j = 0;
    for (int i = 1; i <= (m + r); i++)
    {
        if ((i & (i - 1)) == 0)
        {                // Check if `i` is a power of 2 (parity bit position)
            data[i] = 0; // Initialize parity bit
        }
        else
        {
            data[i] = input[j++];
        }
    }
}

// Function to check for errors in received data
int checkForErrors(int *received, int m, int r)
{
    int error_pos = 0;

    for (int i = 0; i < r; i++)
    {
        int pos = (1 << i);
        int parity = 0;

        for (int j = 1; j <= (m + r); j++)
        {
            if (j & pos)
            {
                parity ^= received[j]; // Recalculate parity
            }
        }

        if (parity != 0)
        {
            error_pos += pos; // Accumulate error position
        }
    }

    return error_pos; // Returns 0 if no error, otherwise the bit position
}

// Function to correct an error if detected
void correctError(int *received, int error_pos)
{
    if (error_pos != 0)
    {
        printf("\nError detected at bit position: %d\n", error_pos);
        received[error_pos] ^= 1; // Flip the erroneous bit
        printf("Error corrected!\n");
    }
    else
    {
        printf("\nNo error detected.\n");
    }
}

// Function to display the data bits
void displayDataBits(int *data, int m, int r)
{
    printf("Data transmitted (including parity bits): ");
    for (int i = 1; i <= (m + r); i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");
}

// Main function
int main()
{
    int m; // Number of data bits
    printf("Enter the number of data bits: ");
    scanf("%d", &m);

    int r = calculateRedundantBits(m); // Calculate required parity bits
    printf("Number of redundant bits required: %d\n", r);

    int *input = (int *)malloc(m * sizeof(int));
    if (input == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    printf("Enter the data bits (space-separated): ");
    for (int i = 0; i < m; i++)
    {
        scanf("%d", &input[i]);
    }

    int *data = (int *)malloc((m + r + 1) * sizeof(int)); // Hamming encoded data
    if (data == NULL)
    {
        printf("Memory allocation failed!\n");
        free(input);
        return 1;
    }

    insertDataBits(data, input, m, r);
    setParityBits(data, m, r);
    displayDataBits(data, m, r);

    // Simulate transmission by copying the data to received array
    int *received = (int *)malloc((m + r + 1) * sizeof(int));
    if (received == NULL)
    {
        printf("Memory allocation failed!\n");
        free(input);
        free(data);
        return 1;
    }

    for (int i = 1; i <= (m + r); i++)
    {
        received[i] = data[i];
    }

    // Simulating an error
    int error_pos;
    printf("\nEnter the position to introduce an error (0 for no error): ");
    scanf("%d", &error_pos);
    if (error_pos > 0 && error_pos <= (m + r))
    {
        received[error_pos] ^= 1; // Flip the bit to introduce error
    }

    printf("\nReceived data: ");
    for (int i = 1; i <= (m + r); i++)
    {
        printf("%d ", received[i]);
    }
    printf("\n");

    // Check and correct errors
    int detected_error_pos = checkForErrors(received, m, r);
    correctError(received, detected_error_pos);

    printf("\nCorrected data: ");
    for (int i = 1; i <= (m + r); i++)
    {
        printf("%d ", received[i]);
    }
    printf("\n");

    // Free dynamically allocated memory
    free(input);
    free(data);
    free(received);

    return 0;
}
