#pragme once
#include<stdbool.h>

struct ClientInfo
{
    int fd;
    int cout;
    pthread_t t;
    struct ClinetInfo* next;
};


struct ClientInfo* createlist();

struct ClientInfo* prependNode(struct ClientInfo* head,int fd);


bool deleteNode(struct ClientInfo* head,int fd);

void freeClist(struct ClientList* head);


