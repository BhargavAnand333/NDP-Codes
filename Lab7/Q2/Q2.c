#include <stdio.h>

#define REPLENISH_RATE 10   // KB per second
#define BUCKET_SIZE 50      // KB
#define PACKET_SIZE 15      // KB
#define PACKET_INTERVAL 0.5 // Seconds
#define MAX_RATE 20         // KBps (For part iii)

void simulate_token_bucket()
{
    double time = 0;             // Current time in seconds
    double tokens = BUCKET_SIZE; // Start with a full bucket
    int packets_sent = 0;        // Count of packets successfully sent

    printf("\nTime (s)\tTokens\t\tStatus\n");
    printf("--------------------------------------\n");

    // Simulate up to 5 seconds to observe behavior
    while (time <= 5.0)
    {
        // Replenish tokens at 10 KBps
        if (tokens < BUCKET_SIZE)
        {
            tokens += REPLENISH_RATE * PACKET_INTERVAL;
            if (tokens > BUCKET_SIZE)
                tokens = BUCKET_SIZE; // Cap tokens at bucket size
        }

        // A packet arrives every 0.5 seconds
        if ((int)(time * 10) % (int)(PACKET_INTERVAL * 10) == 0 && time != 0)
        {
            if (tokens >= PACKET_SIZE)
            {
                tokens -= PACKET_SIZE;
                packets_sent++;
                printf("%4.1f\t\t%4.1f KB\tSent Packet %d\n", time, tokens, packets_sent);
            }
            else
            {
                printf("%4.1f\t\t%4.1f KB\tQueued (Insufficient tokens)\n", time, tokens);
            }
        }

        // Increase time by 0.5 seconds
        time += PACKET_INTERVAL;
    }
}

int main()
{
    simulate_token_bucket();
    return 0;
}
