#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "subscriber.h"
#include "signal_handler.h"

void subscriber_start(int fd, const char *topic)
{
    char message[256];
    int bytes_read;

    setup_signals();

    printf("[Subscriber - %s] Started. PID = %d\n",
           topic, getpid());

    while (1)
    {
        bytes_read = read(fd, message, sizeof(message) - 1);

        if (bytes_read <= 0)
        {
            break;
        }

        message[bytes_read] = '\0';

        printf("[Subscriber - %s] Received: %s\n",
               topic, message);
    }

    close(fd);

    printf("[Subscriber - %s] Exiting.\n", topic);

    exit(0);
}
