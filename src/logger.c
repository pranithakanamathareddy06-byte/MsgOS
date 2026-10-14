
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "logger.h"

#define LOG_FILE "msgbus.log"

int logger_log(const char *topic, const char *message)
{
    int fd;
    char entry[600];

    int length = snprintf(entry, sizeof(entry),
                          "Topic: %s | Message: %s\n",
                          topic, message);

    if (length < 0 || length >= (int)sizeof(entry))
    {
        printf("[Logger] Message too long.\n");
        return -1;
    }

    fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (fd == -1)
    {
        perror("[Logger] open");
        return -1;
    }

    int total = 0;

    while (total < length)
    {
        ssize_t n = write(fd, entry + total,
                          (size_t)(length - total));

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("[Logger] write");
            close(fd);
            return -1;
        }

        if (n == 0)
        {
            close(fd);
            return -1;
        }

        total += (int)n;
    }

    close(fd);

    printf("[Logger] Message saved to %s\n", LOG_FILE);

    return 0;
}

void logger_show_info(void)
{
    struct stat info;

    if (stat(LOG_FILE, &info) == -1)
    {
        perror("[Logger] stat");
        return;
    }

    printf("\n===== File Information =====\n");
    printf("File: %s\n", LOG_FILE);
    printf("Size: %lld bytes\n", (long long)info.st_size);
    printf("Inode: %llu\n", (unsigned long long)info.st_ino);
    printf("Permissions: %03o\n",
           (unsigned int)(info.st_mode & 0777));
    printf("============================\n");
}
