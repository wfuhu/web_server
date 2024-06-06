#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Server.h"
int main(int argc,char *argv[])
{
    printf("111初始化：端口%s，目录：%s\n", argv[1],argv[2] );
    if (argc < 3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short prot = atoi(argv[1]);
    // 切换服务器的路径
    chdir(argv[2]);
    // 初始化套接子
    int lfd = initListenFd(prot);

    epollRun(lfd);
    return 0;
}