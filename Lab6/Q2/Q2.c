#include <stdio.h>
#include <string.h>

// Function to perform CRC division
unsigned int compute_crc(char *data, unsigned int polynomial, int crc_bits)
{
    unsigned int crc = 0; // Initial CRC value
    int data_len = strlen(data);

    for (int i = 0; i < data_len; i++)
    {
        crc ^= (data[i] << (crc_bits - 8)); // XOR with input character

        for (int j = 0; j < 8; j++)
        {
            if (crc & (1 << (crc_bits - 1)))
            {                                  // If MSB is 1
                crc = (crc << 1) ^ polynomial; // XOR with polynomial
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc & ((1 << crc_bits) - 1); // Mask to ensure correct CRC length
}

// Function to select polynomial
unsigned int get_polynomial(int type)
{
    switch (type)
    {
    case 12:
        return 0x80F; // CRC-12 Polynomial: x^12 + x^11 + x^3 + x^2 + x + 1
    case 16:
        return 0x8005; // CRC-16 Polynomial: x^16 + x^15 + x^2 + 1
    case 32:
        return 0x04C11DB7; // CRC-CCIP Polynomial
    default:
        return 0;
    }
}

int main()
{
    char data[100], received_data[100];
    int crc_type;

    // Input message
    printf("Enter the data to send: ");
    scanf("%s", data);

    // Select CRC type
    printf("Select CRC Type (12 for CRC-12, 16 for CRC-16, 32 for CRC-CCIP): ");
    scanf("%d", &crc_type);

    unsigned int polynomial = get_polynomial(crc_type);
    if (polynomial == 0)
    {
        printf("Invalid CRC type selected.\n");
        return 1;
    }

    // Compute CRC at sender side
    unsigned int crc = compute_crc(data, polynomial, crc_type);
    printf("Generated CRC (%d-bit): %X\n", crc_type, crc);

    // Append CRC to data (simulate transmission)
    sprintf(received_data, "%s%X", data, crc);
    printf("Transmitted Data with CRC: %s\n", received_data);

    // Simulate receiving data
    printf("Enter received data: ");
    scanf("%s", received_data);

    // Compute CRC at receiver side
    unsigned int received_crc = compute_crc(received_data, polynomial, crc_type);

    // Check for corruption
    if (received_crc == 0)
    {
        printf(" Data received correctly (No corruption)\n");
    }
    else
    {
        printf(" Data corrupted! (CRC Mismatch)\n");
    }

    return 0;
}
