//
// Created by runoob on 2024/5/21.
//

#include "TaskQueue.h"
Task::Task() : func(nullptr), arg(nullptr) {};
Task::Task(callback func, void* arg) : func(func), arg(arg) {};

TaskQueue::TaskQueue() : queueCapacity(100){
    pthread_mutex_init(&mutex, nullptr);
}
TaskQueue::TaskQueue(int queueCapacity) : queueCapacity(queueCapacity) {
    pthread_mutex_init(&mutex, nullptr);
}
TaskQueue::~TaskQueue() {
    pthread_mutex_destroy(&mutex);
}

void TaskQueue::addTask(Task task) {
    pthread_mutex_lock(&mutex);
    if (taskQ.size() >= queueCapacity) {
        pthread_mutex_unlock(&mutex);
        std::cout << "task queue is full" << std::endl;
        return;
    }
    taskQ.push(task);
    pthread_mutex_unlock(&mutex);
}

int TaskQueue::taskNumber() {
    pthread_mutex_lock(&mutex);
    int size = taskQ.size();
    pthread_mutex_unlock(&mutex);
    return size;
}
Task TaskQueue::takeTask() {
    pthread_mutex_lock(&mutex);
    while (taskQ.empty()) {
        pthread_mutex_unlock(&mutex);
        std::cout << "task queue is empty" << std::endl;
        return Task();
    }
    Task task = taskQ.front();
    taskQ.pop();
    pthread_mutex_unlock(&mutex);
    return task;
}
void TaskQueue::destroyTaskQueue() {
    pthread_mutex_lock(&mutex);
    taskQ = std::queue<Task>();
    pthread_mutex_unlock(&mutex);
}