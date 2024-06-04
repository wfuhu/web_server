#include "Service.h"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <memory.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/sendfile.h>
#include<dirent.h>
#include<sys/types.h>
#include <stdio.h>
#include <stdlib.h>

int initListenFd(unsigned short port)
{
    // 1.创建监听的fd
    printf("监听\n");
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }

    // 2.设置端口复用
    int opt = -1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);

    if (ret == -1)
    {
        perror("setscokopt");
        return -1;
    }
    // 3.绑定
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(lfd, (struct sockaddr *)&addr, sizeof addr);
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }
    // 4.设置监听
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    printf("监听\n");
    return lfd;
}
const char *getFileType(const char *name)
{
    printf("文件类型");
    // 自】右向左查找‘.’字符 不存在返回null
    const char *dot = strrchr(name, '.');
    if (dot == NULL)
    {
        return "text/plain; charset=utf-8";
    }
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    {
        return "text/html; charset=utf-8";
    }
    if (strcmp(dot, ".jepg") == 0 || strcmp(dot, ".jpg") == 0)
    {
        return "image/jpeg";
    }
    if (strcmp(dot, ".gif") == 0)
    {
        return "image/png";
    }
    if (strcmp(dot, ".css") == 0)
    {
        return "text/css";
    }
    if (strcmp(dot, ".au") == 0)
    {
        return "audio/basic";
    }
    if (strcmp(dot, ".wav") == 0)
    {
        return "audio/wav";
    }
    if (strcmp(dot, ".avi") == 0)
    {
        return "video/x-msvideo";
    }
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
    {
        return "video/quicktime";
    }
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
    {
        return "video/mpeg";
    }
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
    {
        return "model/vrml";
    }
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
    {
        return "audio/midi";
    }
    if (strcmp(dot, ".mp3") == 0)
    {
        return "audi/peg";
    }
    if (strcmp(dot, ".ogg") == 0)
    {
        return "applocation/ogg";
    }
    if (strcmp(dot, ".pac") == 0)
    {
        return "application/x-ns-proxy-autoconfig";
    }
    return "text/plain;charset=utf-8";
}



int sendFile(const char *fileName, int cfd)
{
    printf("发送文件");
    int fd = open(fileName, O_RDONLY);
    assert(fd > 0);
    int size = lseek(fd, 0, SEEK_END); // 文件大小
    sendfile(cfd, fd, NULL, size);     // 发送文件

    // while (1)
    // {
    //     char buf[1024];
    //     int len = read(fd, buf, sizeof buf);
    //     if (len > 0)
    //     {
    //         send(cfd, buf, len, 0);
    //         usleep(100); // 微秒 1000
    //     }
    //     else if (len == 0)
    //     {
    //         break; /* code */
    //     }
    //     else
    //     {
    //         perror("read");
    //     }
    // }

    return 0;
}

int sendHeadMsg(int cfd, int status, const char *descr, const char *type, int length)
{
    // 状态行
    printf("发送get头");
    char buf[4096];
    sprintf(buf, "http/1.1%d%s\r\n", status, descr);
    // 响应头
    sprintf(buf + strlen(buf), "content-type:%s\r\n", type);
    sprintf(buf + strlen(buf), "content-length:%d\r\n\r\n", length);

    send(cfd, buf, strlen(buf), 0);

    return 0;
}
int sendDir(const char *dirName, int cfd)
{
    printf("发送目录");
    char buf[4096];
    sprintf(buf,"<html><head><title>%s</title></head><body><table>",dirName);

    struct dirent **nameList;

    int num = scandir(dirName, &nameList, NULL, alphasort);
    for (size_t i = 0; i < num; i++)
    {
        char *name = nameList[i]->d_name; // 指针数组  struct dirent tem[]
        struct stat st;
        char subPath[1024]={'0'};
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode))
        {
            sprintf(buf+strlen(buf),
            "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
            name,name,st.st_size);
        }
        else
        {
            sprintf(buf+strlen(buf),
            "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
            name,name,st.st_size);
        }
        send(cfd,buf,sizeof buf,0);
        memset(buf,0,sizeof( buf));
        free(nameList[i]);
    }
    sprintf(buf,"</table></body></html>");
    send(cfd,buf,strlen(buf),0);
    free(nameList);

    return 0;
}
int parseRequestLine(const char *line, int cfd)
{
    printf("解析行");
    // 解析请求行 正则匹配  get方法  /xxx/1.jpg http/1.1
    char method[12]={'0'};
    char path[1024]={'0'};
    sscanf(line, "%[^ ] %[^ ]", method, path);
    printf("method:%s,path:%s\n",method,path);
    if (strcasecmp(method, "get") != 0)
    {
        return -1;
    }
    // 处理客户端请求的静态资源
    char *file = NULL;
    if (strcmp(path, "/") == 0)
    {
        file =(char*)"./";
    }
    else
    {
        file = path + 1;
    }
    // 获取文件的属性
    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1)
    {
        sendHeadMsg(cfd, 404, "not found", getFileType(".html"), -1);
        sendFile("404.html", cfd);
        return 0;
        // 文件不存在
    }
    // 判断文件类型
    if (S_ISDIR(st.st_mode))
    {
        // 目录传送给客户端
        sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
        sendDir(file,cfd);
    }
    else
    {
        // 文件传送客户端
        sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size);
        sendFile(file, cfd);
    }
    return 0;
}
int acceptClient(int lfd,int epfd)
{
    printf("接收信息");
    int cfd = accept(lfd, NULL, NULL);
    if (cfd == -1)
    {
        perror("accept");
        return -1;
    }
    // 阻塞
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);

    // cfd添加到epoll
    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN ;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);

    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }

    return 0;
}
int epollRun(int lfd)
{
    printf("epoll运行");
    // 1.创建实例
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_crate");
        return -1;
    }

    // 2.lfd上树
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    // 3 检测
    struct epoll_event evs[1024];

    int size = sizeof(evs) / sizeof(struct epoll_event);
    while (1)
    {

        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;
            if (fd == lfd)
            {
                // 连接
                acceptClient(lfd, epfd);
            }
            else
            {
                // 接收数据
                recvRequest(fd,epfd);
            }
        }
    }

    return 0;
}



int recvRequest(int cfd, int epfd)
{
    printf("接收数据");
    int len=0, totle=0;
    char buf[1024]={'0'};
    char buff[4096]={'0'};
    while ((len == recv(cfd, buf, sizeof buff, 0) )> 0)
    {
        if (totle + len < sizeof buff)
        {
            memcpy(buff + totle, buf, len);
        }
        totle += len;

    }
    // 数据接受完毕
    if (len == -1 && errno == EAGAIN)
    {
        // 解析
        char* pt=strstr(buf,"\r\n");
        int reqLen=pt -buf;
        buf[reqLen]='\0';
        parseRequestLine(buf,cfd);
    }
    else if (len == 0)
    {
        // 解绑
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
        close(cfd);
    }
    else
    {
        perror("recv");
    }
    return 0;
}


