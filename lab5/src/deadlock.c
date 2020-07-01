#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void do_one_thing()
{
    int i;
    pthread_mutex_lock(&mut);
    for(i = 0; i < 100; i++)
        printf("%d ", i);
    pthread_mutex_lock(&mut2);
    pthread_mutex_unlock(&mut);
    pthread_mutex_unlock(&mut2);
}


void do_another_thing()
{
    int i;
    pthread_mutex_lock(&mut2);
    for(i = 0; i < 100; i++)
        printf("%d ", i);
    pthread_mutex_lock(&mut);
    pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut);
}

int main() 
{
    pthread_t thread1, thread2;

    if (pthread_create(&thread1, NULL, (void *)do_one_thing,NULL)!= 0) 
    {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_create(&thread2, NULL, (void *)do_another_thing, NULL) != 0) 
    {
        perror("pthread_create");
        exit(1);
    }

    if (pthread_join(thread1, NULL) != 0) 
    {
        perror("pthread_join");
        exit(1);
    }

    if (pthread_join(thread2, NULL) != 0) 
    {
        perror("pthread_join");
        exit(1);
    }

    return 0;
}