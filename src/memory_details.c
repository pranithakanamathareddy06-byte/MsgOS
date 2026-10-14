#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_data = 100;

int main(void)
{
    int local_data = 200;
    int *heap_data = malloc(sizeof(int));

    if (heap_data == NULL)
    {
        perror("malloc");
        return 1;
    }

    *heap_data = 300;

    printf("===== CO-4 Memory Demo =====\n");
    printf("Address of global data: %p\n", (void *)&global_data);
    printf("Address of stack data:  %p\n", (void *)&local_data);
    printf("Address of heap data:   %p\n", (void *)heap_data);
    printf("Process ID: %d\n", getpid());
    printf("Heap value: %d\n", *heap_data);
    printf("Virtual memory uses process-specific address spaces.\n");

    free(heap_data);
    printf("Heap memory released successfully.\n");

    return 0;
}
