#include <stdio.h>

#define BUCKET_SIZE 10   // Maximum queue size in bytes
#define PACKET_SIZE 4    // Each incoming packet size
#define OUTGOING_RATE 1  // Bytes sent per second
#define TOTAL_PACKETS 11 // Number of unique arrival times

int main()
{
    // Time and count of packets arriving at those times
    int arrival_times[TOTAL_PACKETS] = {1, 2, 3, 5, 5, 6, 8, 11, 12, 15, 15};
    int packet_count[TOTAL_PACKETS] = {1, 1, 2, 4, 1, 3, 4, 1, 2, 5, 1};

    int bucket_content = 0; // Current bucket size
    int current_index = 0;  // Tracks current packet time index

    printf("\nTime\tPacket\tBucket\tStatus\n");
    printf("-------------------------------------\n");

    // Simulate time from 0 to last packet arrival + extra buffer time
    for (int current_time = 0; current_time <= arrival_times[TOTAL_PACKETS - 1] + 5; current_time++)
    {
        // Leak packets at a constant rate
        if (bucket_content > 0)
        {
            bucket_content -= OUTGOING_RATE;
            if (bucket_content < 0)
                bucket_content = 0;
        }

        // Process packets arriving at the current time
        if (current_index < TOTAL_PACKETS && arrival_times[current_index] == current_time)
        {
            for (int j = 0; j < packet_count[current_index]; j++)
            { // Process each packet separately
                int incoming_size = PACKET_SIZE;

                if (bucket_content + incoming_size <= BUCKET_SIZE)
                {
                    bucket_content += incoming_size;
                    printf("%2d\t%4d bytes\t%4d bytes\tAccepted\n", current_time, incoming_size, bucket_content);
                }
                else
                {
                    printf("%2d\t%4d bytes\t%4d bytes\tDropped\n", current_time, incoming_size, bucket_content);
                }
            }
            current_index++; // Move to next packet time
        }
    }

    return 0;
}
