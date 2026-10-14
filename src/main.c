#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "broker.h"
#include "publisher.h"
#include "ipc.h"

#define FIFO_NAME "/tmp/msgbus_fifo"

int main()
{
    pid_t broker_pid;

    printf("=================================\n");
    printf("       MsgBus Message Bus\n");
    printf("   Publish / Subscribe System\n");
    printf("=================================\n");

    create_fifo(FIFO_NAME);

    broker_pid = fork();

    if (broker_pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (broker_pid == 0)
    {
        broker_start(FIFO_NAME);
        return 0;
    }

    sleep(1);

    publisher_start(FIFO_NAME);

    waitpid(broker_pid, NULL, 0);

    unlink(FIFO_NAME);

    printf("\n[Main] MsgBus terminated successfully.\n");

    return 0;
}
