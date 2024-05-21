//
// Created by runoob on 2024/5/21.
//

#ifndef CPPNETLEARN_TASKQUEUE_H
#define CPPNETLEARN_TASKQUEUE_H
#include <queue>
#include <pthread.h>
using  callback= void(*)(void* arg);
// 任务结构体
template <typename T>
class Task
{
public:
    callback func;
    T* arg;
    Task<T>();
    Task<T>(callback func, T* arg);
};
template <typename T>
class TaskQueue{
private:
    pthread_mutex_t mutex;
    std::queue<Task<T>> taskQ;
    int queueCapacity;  // 容量
public:
    TaskQueue();
    TaskQueue(int queueCapacity);
    ~TaskQueue();
    // 添加任务
    void addTask(Task<T> task);
    // 取任务
    Task<T> takeTask();
    // 获取任务个数
    size_t taskNumber();
    // 销毁任务队列
    void destroyTaskQueue();
    int getQueueCapacity() { return queueCapacity; }
};


#endif //CPPNETLEARN_TASKQUEUE_H
