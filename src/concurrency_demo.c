#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define ITERATIONS 100000

int shared_counter = 0;
int completed_workers = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t all_done = PTHREAD_COND_INITIALIZER;
sem_t resource_sem;

void *worker(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < ITERATIONS; i++)
    {
        pthread_mutex_lock(&lock);
        shared_counter++;
        pthread_mutex_unlock(&lock);
    }

    sem_wait(&resource_sem);
    printf("Thread %d completed its work.\n", id);
    sem_post(&resource_sem);

    pthread_mutex_lock(&lock);
    completed_workers++;

    if (completed_workers == 2)
        pthread_cond_signal(&all_done);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;

    sem_init(&resource_sem, 0, 1);

    printf("===== CO-6 Concurrency Demo =====\n");

    pthread_create(&t1, NULL, worker, &id1);
    pthread_create(&t2, NULL, worker, &id2);

    pthread_mutex_lock(&lock);
    while (completed_workers < 2)
        pthread_cond_wait(&all_done, &lock);
    pthread_mutex_unlock(&lock);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Final counter: %d\n", shared_counter);
    printf("Expected counter: %d\n", 2 * ITERATIONS);
    printf("Both threads completed successfully.\n");

    sem_destroy(&resource_sem);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&all_done);

    return 0;
}
