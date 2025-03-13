#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

#define MIN(a,b) (a < b ? a : b)
#define MAX(a,b) (a < b ? b : a)

typedef struct test_t
{
    int times;
    char buff[64];
} test_t;

void task1(void *args)
{
    test_t *test = (test_t *)args;
    printf("process task1 pid = %lld, times = %d, buff = %s\n",pthread_self(), test->times, test->buff);
    free(args);
}

void task2(void *args)
{
    test_t *test = (test_t *)args;
    printf("process task2 pid = %lld, times = %d, buff = %s\n",pthread_self(), test->times, test->buff); 
    free(args);
}

void task3(void *args)
{
    test_t *test = (test_t *)args;
    printf("process task3 pid = %lld, times = %d, buff = %s\n",pthread_self(), test->times, test->buff); 
    free(args);
}


int main(int argc, char *argv[])
{
    
    threadpool_t *pool = create_threadpool(4, 128);
    test_t *test;
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            test = (test_t *)malloc(sizeof(test_t));
            test->times = j;
            sprintf(test->buff, "task for test1");
            add_task_threadpool(pool, task1, test);  //任务添加到线程池中


            test = (test_t *)malloc(sizeof(test_t));
            test->times = j;
            sprintf(test->buff, "task for test2");
            add_task_threadpool(pool, task2, test);

            test = (test_t *)malloc(sizeof(test_t));
            test->times = j;
            sprintf(test->buff, "task for test3");
            add_task_threadpool(pool, task3, test);


        }
        sleep(1);
    }

    destroy_threadpool(pool);
    return 0;
}