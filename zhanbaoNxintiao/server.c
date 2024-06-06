#include <stdio.h>
#include "socket.h"
#include <string.h>
#include <pthread.h>
#include "clientlist.cpp"



void *parseRecvMessage(void *arg)
{
    struct ClientInfo *info = (struct ClientInfo *)arg;
    while (1)
    {
        char *buf;
        enum Type t;
        recvMessage(info->fd, buf, &t);
        if (buf == NULl)
        {
            printf("tuichu%s\n", info->fd);
            pthread_exit(NULL);
        }
        else
        {
            if (t == Heart)
            {
                printf("xiantiaobao");
                pthread_mutex_lock(&mutex);
                info->count = 0;
                pthread_mutex_unlock(&mutex);
                sendMessage(info->fd, buf, Heart);
            }
            else
            {
                const char* pt="11111";
                printf("shuju bao:%s\n", buf);
                sendMessage(info->fd, pt,strlen(pt), Message);
            }
            free(buf);
        }
    }

    return NULL;
}

void *heartBeat(void *arg)
{
    struct ClientInfo *head = *(struct ClientInfo *)arg;
    struct ClientInfo *p=NULL;
    while (1)
    {
        p=head->next;
        while (p)
        {
        pthread_mutex_lock(&mutex);
        head->count++;//没收到服务器回复
        if (head->count > 5)
        {
            printf("客户端断开了连接");
            close(head->fd);
            pthread_cancel(p->t);
            deleteNode(head,p->fd);

        }
        pthread_mutex_unlock(&mutex);
        p=p->next;
        
        }
        
        sleep(2);
    }
}

int main()
{
    unsigned short port = 8888;

    int lfd = initSocket();
    setListen(lfd, port);
    // lianbiao
    struct ClientInfo *head = createlist();

    pthread_mutex_init(&mutex, NULL);
    connectToHost(info.fd, port, ip);

    // 心跳包子线成
    pthread_t pid1;
    pthread_create(&pid, NULL, heartBeat, head);
    while (1)
    {
        int socketFd = accpetConnect(lfd, NULL);
        if (socketFd == -1)
        {
            continue;
        }
        struct ClientInfo *node = prependNode(head, socketFd);
        prependNode(head, socketFd);
        // 接收数据子线成
        
        pthread_create(&node->t, NULL, parseRecvMessage, node);
        pthread_detach(node->t);
    }
    pthread_join(pid1, NULL);
    pthread_mutex_destroy(&mutex);
    close(lfd);
    return 0;
}