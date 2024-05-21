//
// Created by runoob on 2024/5/21.
//

#ifndef CPPNETLEARN_TASKQUEUE_H
#define CPPNETLEARN_TASKQUEUE_H
#include <queue>
#include <pthread.h>
using  callback= void(*)(void* arg);
// 任务结构体
class Task
{
private:
    callback func;
    void* arg;
public:
    Task();
    Task(callback func, void* arg);
};

class TaskQueue{
private:
    pthread_mutex_t mutex;
    std::queue<Task> taskQ;
    int queueCapacity;  // 容量
public:
    TaskQueue(int queueCapacity);
    ~TaskQueue();
    // 添加任务
    void addTask(Task task);
    // 取任务
    Task takeTask();
    // 获取任务个数
    inline int taskNumber();
    // 销毁任务队列
    void destroyTaskQueue();
};


#endif //CPPNETLEARN_TASKQUEUE_H
