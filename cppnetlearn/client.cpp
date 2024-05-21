//
// Created by runoob on 2024/5/20.
//
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <thread>
int main(){
    //创建用于通信的socket
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        perror("socket error");
        return -1;
    }
    //连接服务器IP socket
    struct sockaddr_in addr;
//    地址族协议
    addr.sin_family=AF_INET;
//    端口号
    addr.sin_port=htons(8888);
//    服务器的IP地址
    inet_pton(AF_INET,"172.22.61.177",&addr.sin_addr.s_addr);
    int ret=connect(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret==-1){
        perror("连接失败");
        return -1;
    }
    else{
        std::cout<<"连接成功"<<std::endl;
    }
    int number=0;
    while(1){
        //发送数据
        char buf[1024];
        sprintf(buf,"hello server: %d...\n",number++);
        send(fd,buf,strlen(buf)+1,0);

//        接收数据
        memset(buf,0,sizeof(buf));
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        // 转换为time_t以便我们可以使用它与C标准库函数
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        // 打印当前时间
        std::cout << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "";
        int len=recv(fd,buf,sizeof(buf),0);
        if(len>0){
            std::cout<<"server say:"<<buf<<std::endl;
        }
        else if(len==0){
            std::cout<<"server close"<<std::endl;
            break;
        }
        else{
            perror("recv error");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    close(fd);
    close(cfd);
    return 0;
}
