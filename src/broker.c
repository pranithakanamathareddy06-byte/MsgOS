
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

#include "broker.h"
#include "subscriber.h"
#include "signal_handler.h"
#include "memory.h"
#include "logger.h"

#define BUFFER_SIZE 300
#define TOPIC_SIZE 64

typedef struct Topic
{
    char name[TOPIC_SIZE];
    int pipefd[2];
    pid_t pid;
    struct Topic *next;
} Topic;

static Topic *topics = NULL;

static Topic *find_topic(const char *name)
{
    Topic *current = topics;

    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
            return current;

        current = current->next;
    }

    return NULL;
}

static Topic *create_topic(const char *name, int fifo_fd)
{
    Topic *topic = malloc(sizeof(Topic));

    if (topic == NULL)
    {
        perror("[Broker] malloc");
        return NULL;
    }

    if (strlen(name) >= sizeof(topic->name))
    {
        printf("[Broker] Topic name is too long.\n");
        free(topic);
        return NULL;
    }

    strcpy(topic->name, name);
    topic->next = NULL;

    if (pipe(topic->pipefd) == -1)
    {
        perror("[Broker] pipe");
        free(topic);
        return NULL;
    }

    topic->pid = fork();

    if (topic->pid == -1)
    {
        perror("[Broker] fork");
        close(topic->pipefd[0]);
        close(topic->pipefd[1]);
        free(topic);
        return NULL;
    }

    if (topic->pid == 0)
    {
        Topic *current = topics;

        close(fifo_fd);

        while (current != NULL)
        {
            if (current->pipefd[0] >= 0)
                close(current->pipefd[0]);

            if (current->pipefd[1] >= 0)
                close(current->pipefd[1]);

            current = current->next;
        }

        close(topic->pipefd[1]);

        subscriber_start(topic->pipefd[0], topic->name);
        _exit(0);
    }

    close(topic->pipefd[0]);
    topic->pipefd[0] = -1;

    topic->next = topics;
    topics = topic;

    printf("[Broker] New topic registered: %s\n", name);

    return topic;
}

static void process_message(char *buffer, int fifo_fd)
{
    char *separator = strchr(buffer, '|');

    if (separator == NULL)
    {
        printf("[Broker] Invalid format. Use topic|message\n");
        return;
    }

    *separator = '\0';

    char *name = buffer;
    char *message = separator + 1;

    while (isspace((unsigned char)*name))
        name++;

    char *end = name + strlen(name);

    while (end > name && isspace((unsigned char)end[-1]))
        *--end = '\0';

    while (isspace((unsigned char)*message))
        message++;

    if (*name == '\0')
    {
        printf("[Broker] Topic cannot be empty.\n");
        return;
    }

    if (strlen(name) >= TOPIC_SIZE)
    {
        printf("[Broker] Topic name is too long (maximum 63 characters).\n");
        return;
    }

    Topic *topic = find_topic(name);

    if (topic == NULL)
        topic = create_topic(name, fifo_fd);

    if (topic == NULL)
        return;

    printf("[Broker] Topic: %s\n", name);

    if (logger_log(name, message) != 0)
        printf("[Broker] Warning: message could not be logged.\n");

    size_t length = strlen(message) + 1;
    size_t sent = 0;

    while (sent < length)
    {
        ssize_t n = write(topic->pipefd[1],
                          message + sent, length - sent);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("[Broker] write to subscriber");
            return;
        }

        sent += (size_t)n;
    }

    printf("[Broker] Message sent to subscriber: %s\n", name);
}

void broker_start(const char *fifo_name)
{
    int fifo_fd;
    char buffer[BUFFER_SIZE];
    size_t used = 0;

    printf("\n===== Broker =====\n");
    printf("[Broker] PID = %d\n", getpid());
    printf("[Broker] Process Group ID = %d\n", getpgrp());
    printf("[Broker] Session ID = %d\n", getsid(0));

    setup_signals();
    memory_demo();

    fifo_fd = open(fifo_name, O_RDONLY);

    if (fifo_fd == -1)
    {
        perror("[Broker] FIFO open failed");
        return;
    }

    printf("[Broker] Waiting for messages...\n");

    while (1)
    {
        char ch;
        ssize_t n = read(fifo_fd, &ch, 1);

        if (n == -1)
        {
            if (errno == EINTR)
                continue;

            perror("[Broker] FIFO read");
            break;
        }

        if (n == 0)
            break;

        if (ch == '\0')
        {
            buffer[used] = '\0';
            process_message(buffer, fifo_fd);
            used = 0;
        }
        else if (used < sizeof(buffer) - 1)
        {
            buffer[used++] = ch;
        }
        else
        {
            printf("[Broker] Message too long; discarded.\n");
            used = 0;

            /* Discard the rest of this message. */
            while (ch != '\0')
            {
                n = read(fifo_fd, &ch, 1);

                if (n <= 0)
                    break;
            }

            if (n <= 0)
                break;
        }
    }

    close(fifo_fd);

    Topic *current = topics;

    while (current != NULL)
    {
        close(current->pipefd[1]);
        current = current->next;
    }

    current = topics;

    while (current != NULL)
    {
        waitpid(current->pid, NULL, 0);
        printf("[Broker] Subscriber %s terminated.\n",
               current->name);

        Topic *next = current->next;
        free(current);
        current = next;
    }

    topics = NULL;

    printf("[Broker] All subscribers terminated.\n");
}

