#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

void memory_demo(void)
{
    char *message;

    message = malloc(100);

    if (message == NULL)
    {
        printf("[Memory] Allocation failed.\n");
        return;
    }

    strcpy(message, "Dynamic memory is working.");

    printf("[Memory] %s\n", message);

    free(message);

    printf("[Memory] Memory released using free().\n");
}
