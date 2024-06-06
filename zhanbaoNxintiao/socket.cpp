#include "socket.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
int initSocket()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0); // ipev4  流式协议 默认协议
    if (lfd == 0)
    {
        return -1;
    }

    return 0;
}

void initSockaddr(sockaddr *addr, unsigned short port, const char *ip)
{
    struct sockaddr_in *addrin = (struct sockaddr *)addr;
    addrin->sa_family = AF_INET;             // ipv4
    addrin->sin_port = htons(port);          // 小端转大端
    addrin->sin_addr.s_addr = inet_addr(ip); // ip转换
}

int setListen(int lfd, unsigned port)
{
    struct sockaddr addr;
    initSockaddr(&addr, port, "0.0.0.0"); // inaddr_any
    // 端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //                    级别，端口复用，    关闭还是打开，
    int ret = bind(lfd, &addr, sizeof(addr));
    if (ret == -1)
    {
        return -1;
    }
    ret = listen(lfd, 128); // 文件描述符  最大监听数
    if (ret == -1)
    {
        return -1;
    }
    return 0;
}

int accpetConnect(int lfd, struct sockaddr *addr)
{
    int connfd;
    if (addr = NULL)
    {
        connfd = accept(lfd, NULL, NULL); // bu得到客户端ip和端口
    }
    else
    {
        socklen_t len = sizeof(struct sockaddr);
        connfd = accept(lfd, ); // 得到客户端ip和端口
    }
    if (connfd == -1)
    {
        return -1;
    }

    return connfd;
}

int connectToHost(int fd, unsigned port, const char *ip)
{
    struct socakaddr *addr;
    initSockaddr(&addr, port, ip);
    int ret = connect(fd, &addr, sizeof(addr));
    if (ret == -1)
    {
        return -1;
    }
    return 0;
}

int readn(int fd, char *buffer, int size)
{
    int left = size;
    int readbytes = 0;
    const char *ptr = buffer;
    while (left)
    {
        readbytes = read(fd, ptr, left);
        if (readbytes == -1)
        {
            if (errno == EINTR)
            {
                writebytes = 0;
            }
            else
            {
                return -1;
            }
        }
        else if (readbytes == 0)
        {
            return -1;
        }

        left -= readbytes;
        ptr += readbytes;
    }

    return size - left;
}

int writen(int fd, const char *buffer, int length)
{
    int left = length;
    int writebytes = 0;
    const char *ptr = buffer;
    while (left)
    {
        writebytes = write(fd, ptr, left);
        if (writebytes <= 0)
        {
            if (errno == EINTR)
            {
                writebytes = 0;
            }
            else
            {
                return -1;
            }
        }
        ptr += writebytes;
        left -= writebytes;
    }

    return length;
}

bool sendMessage(int fd, const char *buffer, int length, enum Type t)
{
    int datalen = length + 1 + sizeof(int); // 数据长度+数据类型+数据头
    char *data = (char *)malloc(dataLen);
    if (data == NULL)
    {
        return false;
    }
    int netLen = htonl(length + 1);
    memcpy(data, &netLen, sizeof(int));
    car *ch = t == Heart ? "H" : "M";
    memcpy(data + sizeof(int), ch, szieof(char));
    memcpy(data + sizeof(int) + 1, buffer, length);
    int ret = writen(fd, data, datalen);
    free(data);
    return ret == datalen;
}

bool recvMessage(int fd, char **buffer, enum Type *t)
{
    int dataLen = 0;
    int ret = readn(fd, &dataLen, sizeof(int));
    if (ret == -1)
    {
        *buffer = NULL;
        return -1;
    }
    dataLen = ntohl(dataLen);
    char ch;
    readn(fd, &ch, 1);
    *t = ch == 'H' ? Heart : Message;
    char *tmpbuf = (char *)calloc(dataLen, szieof(char));
    if (tmpbuf == NULL)
    {
        *buffer = NULL;
        return -1;
    }
    ret = readn(fd, tmpbuf, dataLen - 1);
    if (ret != dataLen - 1)
    {
        
        free(tmpbuf);
        *buffer = NULL;
        return -1;
    }
    *buf = tmpbuf;

    return ret;
}