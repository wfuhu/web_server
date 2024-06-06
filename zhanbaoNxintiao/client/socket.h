#pragma once
#include<stdbool.h>
enum Type{Heart,Message};
//数据包类型 H 心跳包     M 数据块
//数据包格式 数据长度     数据包类型 数据块   
//             int          char    cahr*
//              4           1       n
int initSocket();

void initSockaddr(struct sockaddr* addr,unsigned short port,const char* ip);

int setListen(int lfd,unsigned port);

int accpetConnect(int lfd,struct sockaddr* addr);


//客户端
int connectToHost(int fd,unsigned port,const char* ip);

//读出指定字节数
int readn(int fd,char* buffer,int size);
//写入指定字节数
int writen(int fd,const char* buffer,int length);


//发送数据
bool sendMessage(int fd,const char* buffer,int length,enum Type t);


//接收数据
bool recvMessage(int fd,char** buffer,enum Type* t);


