#include<stdio.h>
#include"Service.h"
int main(int argc,char*argv[])
{
    if(argc<3)
    {
        printf("./a.out port path\n");
        return -1;
    }
    unsigned short prot=atoi(argv[1]);
    //切换服务器的路径
    chair(argv[2]);

    //初始化套接子
    int lfd=initListenFd(prot);

    return 0;
}