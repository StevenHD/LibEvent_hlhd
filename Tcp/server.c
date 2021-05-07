//
// Created by hlhd on 2021/5/7.
//

#include "../all.h"

#define _SERVER_PORT_ 8888

void reawdCallback(evutil_socket_t fd, short what, void* arg)
{
    char buf[1024];
    int ret, i;

    printf("readCallback\n");

    ret = read(fd, buf, sizeof(buf));

    if (ret == -1)
    {
        perror("read");
        close(fd);
    }
    else if (ret == 0)
    {
        printf("the other side has closed\n");
        close(fd);
    }
    else
    {
        printf("read socket fd\n");
        for (i = 0; i < ret; i ++ ) buf[i] = toupper(buf[i]);
        write(fd, buf, ret);
    }
}

void accecptCallback(evutil_socket_t fd, short what, void* arg)
{
    int clntFd;
    printf("accecptCallback\n");
    struct event_base* base = (struct event_base*)arg;
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    clntFd = accept(fd, (struct sockaddr_in*)&client_addr, &len);
    if (clntFd != -1)
    {
        struct event* clntEvent = event_new(base, clntFd, EV_READ | EV_PERSIST | EV_ET, reawdCallback, NULL);
        event_add(clntEvent, NULL);
    }
}

void main_loop(int lsnFd)
{
    struct event_base* base    = event_base_new();
    struct event* lsnEvent     = event_new(base, lsnFd, EV_READ | EV_PERSIST | EV_ET, accecptCallback, base);
    event_add(lsnEvent, NULL);
    event_base_dispatch(base);

    event_del(lsnEvent);

    event_free(lsnEvent);
    event_base_free(base);
}

int main()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family          = AF_INET;
    server_addr.sin_port            = htons(_SERVER_PORT_);
    server_addr.sin_addr.s_addr     = htonl(INADDR_ANY);

    int lsnFd = socket(AF_INET, SOCK_STREAM, 0);
    if (bind(lsnFd, (struct sockaddr_in*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(0);
    }
    listen(lsnFd, 128);
    main_loop(lsnFd);

    return 0;
}