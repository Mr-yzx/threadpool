#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

/*
/*@brief 线程池头文件定义
/*
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <limits.h>
#include <stdbool.h>
#include "list.h"

/*
/*@brief 线程池操作枚举
/*
 */
typedef enum THREADPOOL_STATUS
{
    STATUS_SUCCESS = 0,
    STATUS_FAILED
} THREADPOOL_STATUS;

/*
/*@brief 任务回掉函数
/*
 */
typedef void (*task_func_t)(void *args);

/*
/*@brief 线程池基本结构定义
/*
 */
typedef struct threadpool_c
{
    struct list_head  tlist;  //任务链表
    int               thread_num;  //线程数量
    int               max_tasknum; //最大任务数
    int               cur_tasknum; //当前线程池中任务数
    bool              is_exited;   //线程是否退出线程池
    pthread_mutex_t   mutex;
    pthread_cond_t    cond;
    pthread_t         *threads;  //线程数组
} threadpool_t;

/*
/*@brief 任务节点信息
/*
 */
typedef struct task_c
{
    void              *args;
    task_func_t       func;
    struct list_head  node;
} task_t;

/*
/*@brief 创建线程池
/*
/*@param thread_num 线程数目
/*@param max_tasknum 最大任务数
/*@return threadpool_t*  线程池句柄
 */
threadpool_t *create_threadpool(int thread_num, int max_tasknum);

/*
/*@brief 销毁线程池
/*
/*@param pool 要销毁的线程池句柄
/*@return int 
 */
int destroy_threadpool(threadpool_t *pool);

/*
/*@brief 往线程池添加任务
/*
/*@param pool 线程池句柄
/*@param func 任务函数
/*@param args 任务参数
/*@return int 
 */
int add_task_threadpool(threadpool_t *pool, task_func_t func, void *args);

/*
/*@brief 获取线程池中当前任务数量
/*
/*@param pool 线程池句柄
/*@return int 
 */
int get_tasknum_threadpool(threadpool_t *pool);

#endif /* __THREADPOOL_H__ */