#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
    int fd;
    char buffer[100];
    struct stat info;
    const char *message = "MsgBus CO-5 file I/O demonstration.\n";

    printf("===== CO-5 File I/O Demo =====\n");

    fd = open("co5_demo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("open for writing");
        return 1;
    }

    if (write(fd, message, 36) == -1)
    {
        perror("write");
        close(fd);
        return 1;
    }
    close(fd);

    fd = open("co5_demo.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("open for reading");
        return 1;
    }

    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n == -1)
    {
        perror("read");
        close(fd);
        return 1;
    }

    buffer[n] = '\0';
    printf("File content: %s", buffer);
    close(fd);

    if (stat("co5_demo.txt", &info) == -1)
    {
        perror("stat");
        return 1;
    }

    printf("File size: %lld bytes\n", (long long)info.st_size);
    printf("Inode number: %llu\n", (unsigned long long)info.st_ino);
    printf("File descriptor closed successfully.\n");

    return 0;
}
