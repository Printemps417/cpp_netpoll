//
// Created by runoob on 2024/5/20.
//
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <pthread.h>

struct sockinfo{
    int fd;
    struct sockaddr_in addr;
};
std::vector<sockinfo> infos(128);

void* working(void* arg);
int main(){
    //创建用于监听socket
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){
        perror("socket error");
        return -1;
    }
    //套接字绑定本地IP socket
    struct sockaddr_in addr;
//    地址族协议
    addr.sin_family=AF_INET;
//    端口号
    addr.sin_port=htons(8888);
//    IP地址,INADDR_ANY表示本地任意IP，代表自动读网卡的实际IP，与实际IP进行绑定
    addr.sin_addr.s_addr=INADDR_ANY;
    int ret=bind(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret==-1){
        perror("bind error");
        return -1;
    }
    //监听,第一个参数是socket描述符，第二个参数是最大监听数
    ret=listen(fd,128);
    if(ret==-1){
        perror("listen error");
        return -1;
    }
    std::cout<<"listen success,server start"<<std::endl;
    for(int i=0;i<128;i++){
        infos[i].fd=-1;
    }

    while(true){
        sockinfo* pinfo;
        for (int i = 0; i < 128; ++i) {
            if(infos[i].fd==-1){
                pinfo=&infos[i];
                break;
            }
        }
        int addrlen=sizeof(struct sockaddr_in);
        //阻塞并等待客户端连接
        int cfd= accept(fd,(struct sockaddr*)&pinfo->addr,(socklen_t*)&addrlen);
        if(cfd==-1){
            perror("accept error");
            continue;
        }
        else{
            pinfo->fd=cfd;
        }
        pthread_t tid;
        pthread_create(&tid,NULL,working,pinfo);
//        和子线程分离
        pthread_detach(tid);
    }
    close(fd);

    return 0;
}
//    子线程工作函数
void* working(void* arg){
    struct sockinfo* pinfo=(struct sockinfo*)arg;
    char ip[32];
    struct sockaddr_in client_addr=pinfo->addr;
    int cfd=pinfo->fd;
    std::cout<<"client IP:"<<inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip,sizeof(ip))<<std::endl;
    std::cout<<"port:"<<ntohs(client_addr.sin_port)<<std::endl;
    while(1){
        //接收客户端的数据
        char buf[1024];
        int len=recv(cfd,buf,sizeof(buf),0);
        if(len>0){
            std::cout<<"client:"<<cfd<<" say:"<<buf<<std::endl;
            sprintf(buf,"hello client, I hear you:...\n");
            send(cfd,buf,strlen(buf),0);
        }
        else if(len==0){
            std::cout<<"client close"<<std::endl;
            break;
        }
        else{
            perror("recv error");
            break;
        }
    }
    close(cfd);
    pinfo->fd=-1;
    return nullptr;
}