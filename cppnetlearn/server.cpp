//
// Created by runoob on 2024/5/20.
//
#include <bits/stdc++.h>
#include <arpa/inet.h>

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
    //阻塞并等待客户端连接
    struct sockaddr_in client_addr;
    int addrlen=sizeof(client_addr);
    int cfd= accept(fd,(struct sockaddr*)&client_addr,(socklen_t*)&addrlen);
    if(cfd==-1){
        perror("accept error");
        return -1;
    }

    //连接成功，打印客户端的IP和端口信息
    char ip[32];
    std::cout<<"client IP:"<<inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,ip,sizeof(ip))<<std::endl;
    std::cout<<"port:"<<ntohs(client_addr.sin_port)<<std::endl;
    while(1){
        //接收客户端的数据
        char buf[1024];
        int len=recv(cfd,buf,sizeof(buf),0);
        if(len>0){
            std::cout<<"client say:"<<buf<<std::endl;
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
    close(fd);
    return 0;
}
