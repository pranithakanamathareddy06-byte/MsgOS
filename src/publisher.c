#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "publisher.h"

void publisher_start(const char *fifo_name)
{
    int fd;
    char input[300];

    fd = open(fifo_name, O_WRONLY);

    if (fd == -1)
    {
        perror("[Publisher] FIFO open failed");
        return;
    }

    printf("\n===== Publisher =====\n");
    printf("Format: topic|message\n");
    printf("Enter any topic: topic|message\n");
    printf("Type exit to stop.\n\n");

    while (1)
    {
        printf("Publish: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        write(fd, input, strlen(input) + 1);
    }

    close(fd);
}
