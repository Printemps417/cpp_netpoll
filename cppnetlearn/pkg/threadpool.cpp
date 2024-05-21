//
// Created by runoob on 2024/5/20.
//

#include "threadpool.h"

// 初始化线程池
threadpool::threadpool(int min, int max, int queueCap) {
    this->minNum = min;
    this->maxNum = max;
    this->busyNum = 0;
    this->liveNum = min;
    this->exitNum = 0;
    this->queueCapacity = queueCap;
    this->queueSize = 0;
    this->queueFront = 0;
    this->queueRear = 0;
    this->shutdown = 0;

    // 初始化任务队列
    taskQ = new Task[queueCap];
    if (taskQ == nullptr) {
        perror("taskQ new error");
        return;
    }

    // 初始化锁和条件变量
    if (pthread_mutex_init(&mutexPool, nullptr) != 0) {
        perror("mutexPool init error");
        return;
    }
    if (pthread_mutex_init(&mutexBusy, nullptr) != 0) {
        perror("mutexBusy init error");
        return;
    }
    if (pthread_cond_init(&notFull, nullptr) != 0) {
        perror("notFull init error");
        return;
    }
    if (pthread_cond_init(&notEmpty, nullptr) != 0) {
        perror("notEmpty init error");
        return;
    }

    // 创建线程
    threadIDs = new pthread_t[maxNum];
    if (threadIDs == nullptr) {
        perror("threadIDs new error");
        return;
    }
    for (int i = 0; i < minNum; i++) {
        pthread_create(&threadIDs[i], nullptr, worker, this);
    }
    pthread_create(&managerID, nullptr, manager, this);
}
