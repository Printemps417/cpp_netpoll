//
// Created by runoob on 2024/5/21.
//
#include "threadpool.h"
#include "threadpool.cpp"
#include <bits/stdc++.h>
#include <unistd.h>
void taskFunc(void* arg)
{
    int num = *(int*)arg;
    printf("thread %ld is working, number = %d\n",
           pthread_self(), num);
    sleep(1);
}

int main()
{
    // 创建线程池
    threadpool<int> pool(3, 10);
    for (int i = 0; i < 10; ++i)
    {
        int* num = new int(i+100);
        pool.addTask(Task<int>(taskFunc, num));
    }

    sleep(20);

    return 0;
}
