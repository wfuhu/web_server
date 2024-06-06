#include <stdio.h>
#include "socket.h"
#include <string.h>
#include <pthread.h>

pthread_mutex_t mutex;
struct fdInfo
{
    int fd;
    int count;
};

void *parseRecvMessage(void *arg)
{
    struct fdInfo *info = (struct fdInfo *)arg;
    while (1)
    {
        char *buf;
        enum Type t;
        recvMessage(info->fd, buf, &t);
        if (buf == NULl)
        {
            continue;
        }
        else
            (
                if (t == Heart) {
                    printf("xiantiaobao");
                    pthread_mutex_lock(&mutex);
                    info->count = 0;
                    pthread_mutex_unlock(&mutex);
                } else {
                    printf("shuju bao:%s\n", buf);
                } free(buf);)
    }

    return NULL;
}

void *heartBeat(void *arg)
{
    struct fdInfo *info = *(struct fdInfo *)arg;
    while (1)
    {
        pthread_mutex_lock(&mutex);
        info->count++;
        if (info->count > 5)
        {
            printf("duankailianjie");
            close(info->fd);
            exit(9);
        }
        pthread_mutex_unlock(&mutex);
        sleep(2);
        sendMessage(info->fd, "hello", Heart);
    }
}

int main()
{
    struct fdInfo info;
    unsigned short port = 8888;
    const char *ip = "127.0.0.1";
    info.fd = initSocket();
    info.count = 0;
    pthread_mutex_init(&mutex, NULL);
    connectToHost(info.fd, port, ip);
    // 接收数据子线成
    pthread_id pid;
    pthread_create(&pid, NULL, parseRecvMessage, &info);

    // 心跳包子线成
    pthread_t pid1;
    pthread_create(&pid, NULL, heartBeat, &info);
    while (1)
    {
        const char *data = "ni hao ,wfuhu";
        // fa song shuju
        sendMessage(info.fd, data, strlrn(data), Message);
        sleep(5);
    }
    pthread_join(pid, NULL);
    pthread_join(pid1, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;
}