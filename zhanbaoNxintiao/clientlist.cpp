#include "clientlist.h"
#include<stdio.h>

struct ClientInfo *createlist()
{
    struct ClienInfo* head=(struct ClientInfo*)malloc(sizeof(struct ClientInfo));

    return head;
}

struct ClientInfo *prependNode(struct ClientInfo* head, int fd)
{
    struct ClientInfo* node=(struct ClientInfo*)malloc( sizeof(struct ClientInfo));
    node->fd=fd;
    node->next=head->next;
    head->next=node;

    return node;
}

bool deleteNode(ClientInfo *head, int fd)
{
     struct ClientInfo* p=head;
     struct ClientInfo* q=head->next;
     while (q)
     {
        if(q->fd==fd)
        {
            p->next=q->next;
            free(q);
            printf("删除%d\n",q->fd);
            return true;
        }
        p=p->next;
        q=q->next;
     }
     
    return false;
}

void freeClist(struct ClientList *head)
{
    struct ClientList* p=head;
    while (/* condition */)
    {
        struct ClientList* tmp=p;
        p=p->next;
        free(tmp);
    }
    
}
