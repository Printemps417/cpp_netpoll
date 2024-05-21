//
// Created by runoob on 2024/5/21.
//

#include "TaskQueue.h"
#include <iostream>
template <typename T>
Task<T>::Task() : func(nullptr), arg(nullptr) {};
template <typename T>
Task<T>::Task(callback func, T* arg) : func(func), arg(arg) {};
template <typename T>
TaskQueue<T>::TaskQueue() : queueCapacity(100){
    pthread_mutex_init(&mutex, nullptr);
}
template <typename T>
TaskQueue<T>::TaskQueue(int queueCapacity) : queueCapacity(queueCapacity) {
    pthread_mutex_init(&mutex, nullptr);
}
template <typename T>
TaskQueue<T>::~TaskQueue() {
    pthread_mutex_destroy(&mutex);
}
template <typename T>
size_t TaskQueue<T>::taskNumber() {
    pthread_mutex_lock(&mutex);
    int size = taskQ.size();
    pthread_mutex_unlock(&mutex);
    return size;
}
template <typename T>
Task<T> TaskQueue<T>::takeTask() {
    pthread_mutex_lock(&mutex);
    while (taskQ.empty()) {
        pthread_mutex_unlock(&mutex);
        std::cout << "task queue is empty" << std::endl;
        return Task<T>();
    }
    Task<T> task = taskQ.front();
    taskQ.pop();
    pthread_mutex_unlock(&mutex);
    return task;
}
template <typename T>
void TaskQueue<T>::addTask(Task<T> task) {
    pthread_mutex_lock(&mutex);
    if (taskQ.size() >= queueCapacity) {
        pthread_mutex_unlock(&mutex);
        std::cout << "task queue is full" << std::endl;
        return;
    }
    taskQ.push(task);
    pthread_mutex_unlock(&mutex);
}
template <typename T>
void TaskQueue<T>::destroyTaskQueue() {
    pthread_mutex_lock(&mutex);
    taskQ = std::queue<Task<T>>();
    pthread_mutex_unlock(&mutex);
}