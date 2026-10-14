#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "ipc.h"

int create_fifo(const char *name)
{
    if (mkfifo(name, 0666) == -1)
    {
        /* FIFO may already exist */
        printf("[IPC] FIFO already exists or could not be created.\n");
    }
    else
    {
        printf("[IPC] Named FIFO created: %s\n", name);
    }

    return 0;
}
