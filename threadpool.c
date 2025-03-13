#include <assert.h>
#include "threadpool.h"


threadpool_t *create_threadpool(int thread_num, int max_tasknum)
{
    assert(thread_num > 0);

    threadpool_t *pool = (threadpool_t *)malloc(sizeof(threadpool_t));
    memset(pool, 0, sizeof(threadpool_t));

    pool->is_exited = false;
    pool->cur_tasknum = 0;
    pool->max_tasknum = max_tasknum;
    pool->thread_num = thread_num;
    init_list_head(&pool->tlist);

    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->cond, NULL);

    /*创建线程*/
    for (int i = 0; i < thread_num; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, task_process_thread, pool);
    }

    return pool;
}

int destroy_threadpool(threadpool_t *pool)
{
    if (!pool)
    {
        return STATUS_FAILED;
    }
    /*任务链表中还有任务没执行完*/
    while (!list_empty(&pool->tlist))
    {
        continue;
    }

    pool->is_exited = true;
    pthread_cond_broadcast(&pool->cond); //广播给所有线程

    for (int i = 0; i < pool->thread_num; ++i)
    {
        pthread_join(pool->threads[i], NULL); //等待所有线程执行完毕
    }

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->cond);

    free(pool->threads);

    free(pool);
    pool = NULL;
    
    return STATUS_SUCCESS;
}

int add_task_threadpool(threadpool_t *pool, task_func_t func, void *args)
{
    if (!pool)
    {
        return STATUS_FAILED;
    }
    if (!func)
    {
        return STATUS_FAILED;
    }
    if (pool->cur_tasknum > pool->max_tasknum)
    {
        return STATUS_FAILED;
    }

    task_t *task = (task_t *)malloc(sizeof(task_t));
    if (task == NULL)
    {
        return STATUS_FAILED;
    }

    task->func = func;
    task->args = args;

    /*加锁确保互斥*/
    pthread_mutex_lock(&pool->mutex);
    list_add_head(&task->node, &pool->tlist); //新任务添加到任务链表头部
    pool->cur_tasknum++;
    pthread_mutex_unlock(&pool->mutex);

    pthread_cond_signal(&pool->cond);         //通知任务处理线程可以执行
    return STATUS_SUCCESS;
}

int get_tasknum_threadpool(threadpool_t *pool)
{
    if (pool)
    {
        return pool->cur_tasknum;
    }
    else
    {
        return -1;
    }
}

static void *task_process_thread(void *args)
{
    if (!args)
    {
        return NULL;
    }
    threadpool_t *pool = (threadpool_t *)args;
    struct list_head *cur_node = NULL;
    task_t *task = NULL;

    /*处理函数一直从任务链表取任务进行处理*/
    while (true)
    {
        pthread_mutex_lock(&pool->mutex);
        while (list_empty(&pool->tlist) && !pool->is_exited)
        {
            /*阻塞等待直到任务链表有任务*/
            pthread_cond_wait(&pool->cond, &pool->mutex);
        }

        if (pool->is_exited)
        {
            /*线程退出线程池，跳出 while 循环*/
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        cur_node = pool->tlist.next_ptr;  //从链表中取出一个节点
        list_delete_entry(cur_node);      //从链表删除
        (pool->cur_tasknum)--;
        pthread_mutex_unlock(&pool->mutex);

        task = list_entry(cur_node, task_t, node); //从链表节点中取出任务节点
        task->func(task->args);  //执行任务
        free(task);

    }

    return NULL;
}
