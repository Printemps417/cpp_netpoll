//
// Created by runoob on 2024/5/20.
//

#include "threadpool.h"
#include "TaskQueue.h"
#include <pthread.h>
#include <unistd.h>
#include <bits/stdc++.h>
using namespace std;

template <typename T>
threadpool<T>::threadpool(int min, int max, int queueSize)
{
    do
    {
        // 创建任务队列
        taskQ = new TaskQueue<T>(queueSize);
        if (taskQ == nullptr)
        {
            cout << "malloc taskQ fail..." << endl;
            break;
        }
        threadIDs = new pthread_t[max];
        if (threadIDs == nullptr)
        {
            cout << "malloc threadIDs fail..." <<endl;
            break;
        }
        memset(threadIDs, 0, sizeof(pthread_t) * max);
        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;    // 和最小个数相等
        exitNum = 0;

        if (pthread_mutex_init(&mutexPool, NULL) != 0 ||
            pthread_cond_init(&notEmpty, NULL) != 0 ||
            pthread_cond_init(&notFull, NULL) != 0)
        {
            cout<<"mutex or condition init fail..."<<endl;
            break;
        }
        //默认不关闭
        shutdown = 0;

        // 创建线程
        pthread_create(&managerID, NULL, manager, this);
        for (int i = 0; i < min; ++i)
        {
            pthread_create(&threadIDs[i], NULL, worker, this);
        }
        return;
    } while (0);

    // 释放资源
    if (threadIDs) delete[] threadIDs;
    if (taskQ) delete taskQ;
}
template <typename T>
threadpool<T>::~threadpool()
{
    // 关闭线程池
    shutdown = true;
    // 阻塞回收管理者线程
    pthread_join(managerID, NULL);
    // 唤醒阻塞的消费者线程
    for (int i = 0; i < liveNum; ++i)
    {
        pthread_cond_signal(&notEmpty);
    }
    // 释放堆内存
    if (taskQ)
    {
        delete taskQ;
    }
    if (threadIDs)
    {
        delete[] threadIDs;
    }

    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&notEmpty);
    pthread_cond_destroy(&notFull);
}

template <typename T>
void threadpool<T>::addTask(Task<T> task)
{
    while (taskQ->taskNumber() >= taskQ->getQueueCapacity() && !shutdown)
    {
        // 阻塞生产者线程
        pthread_cond_wait(&notFull, &mutexPool);
    }
    if (shutdown)
    {
        return;
    }
    // 添加任务
    this->taskQ->addTask(task);

    pthread_cond_signal(&notEmpty);
}
template <typename T>
int threadpool<T>::getBusyNum()
{
    pthread_mutex_lock(&mutexPool);
    int busyNum = busyNum;
    pthread_mutex_unlock(&mutexPool);
    return busyNum;
}
template <typename T>
int threadpool<T>::getAliveNum()
{
    pthread_mutex_lock(&mutexPool);
    int aliveNum = this->liveNum;
    pthread_mutex_unlock(&mutexPool);
    return aliveNum;
}
template <typename T>
void* threadpool<T>::worker(void* arg)
{
    threadpool<T>* pool = static_cast<threadpool<T>*>(arg);

    while (1)
    {
        pthread_mutex_lock(&pool->mutexPool);
        // 当前任务队列是否为空
        while (pool->taskQ->taskNumber() == 0 && !pool->shutdown)
        {
            // 阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 判断是不是要销毁线程
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExit();
                    break;
                }
            }
        }

        // 判断线程池是否被关闭了
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExit();
            break;
        }

        // 从任务队列中取出一个任务
        Task<T> task = pool->taskQ->takeTask();

        pool->busyNum++;
        // 解锁
        pthread_cond_signal(&pool->notFull);
        pthread_mutex_unlock(&pool->mutexPool);

        cout << "thread " << pthread_self() << " start working..." << endl;
        task.func(task.arg);
        delete task.arg;
        task.arg = nullptr;
        cout << "thread " << pthread_self() << " end working..." << endl;
        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);
    }
    return NULL;
}
template <typename T>
void* threadpool<T>::manager(void* arg)
{
    threadpool<T>* pool = static_cast<threadpool<T>*>(arg);
    while (!pool->shutdown)
    {
        // 每隔3s检测一次
        sleep(3);

        // 取出线程池中任务的数量和当前线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->taskQ->taskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexPool);


        // 添加线程
        // 任务的个数>存活的线程个数 && 存活的线程数<最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < NUMBER
                            && pool->liveNum < pool->maxNum; ++i)
            {
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }
        // 销毁线程
        // 忙的线程*2 < 存活的线程数 && 存活的线程>最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = NUMBER;
            pthread_mutex_unlock(&pool->mutexPool);
            // 让工作的线程自杀
            for (int i = 0; i < NUMBER; ++i)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return NULL;
}
template <typename T>
void threadpool<T>::threadExit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxNum; ++i)
    {
        if (threadIDs[i] == tid)
        {
            threadIDs[i] = 0;
            cout<<"threadExit() called thread " << tid << " exit..." << endl;
            break;
        }
    }
    pthread_exit(NULL);
}
