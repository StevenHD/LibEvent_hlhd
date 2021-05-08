//
// Created by hlhd on 2021/5/8.
//

#include "../all.h"

#define _SERVER_PORT_ 8888
#define _SERVER_IP_   "127.0.0.1"

void readSocketCallback(evutil_socket_t fd, short what, void* arg)
{
    char buf[1024];
    if (what & EV_TIMEOUT)
    {
        puts("This is a EV_TIMEOUT event");
        goto CLOSE;
    }

    struct event_base* base = (struct event_base*)arg;
    int ret = read(fd, buf, sizeof(buf));
    if (ret <= 0)
    {
        puts("close socket");
        goto CLOSE;
    }
    else
    {
        write(STDOUT_FILENO, buf, ret);
    }

    return;

CLOSE:
    close(fd);
    event_base_loopexit(base, NULL);
}

void readStdInCallback(evutil_socket_t fd, short what, void* arg)
{
    char buf[1024];
    int socketFd = (int)arg;
    int ret;
    ret = read(STDIN_FILENO, buf, sizeof(buf));
    write(socketFd, buf, ret);
}

void main_loop(int socketFd)
{
    struct event_base* base       = event_base_new();
    struct event* socketEvent     = event_new(base, socketFd, EV_READ | EV_PERSIST | EV_ET,
                                                readSocketCallback, base);

    struct timeval timeout = {4, 0};
    event_add(socketEvent, &timeout);

    struct event* stdInEvent     = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST | EV_ET,
                                                readStdInCallback, (void*)socketFd);
    event_add(stdInEvent, NULL);

    event_base_dispatch(base);

    event_del(socketEvent);
    event_free(socketEvent);

    event_del(stdInEvent);
    event_free(stdInEvent);

    event_base_free(base);
}

int maincl()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family          = AF_INET;
    server_addr.sin_port            = htons(_SERVER_PORT_);
    inet_pton(AF_INET, _SERVER_IP_, &server_addr.sin_addr.s_addr);      // ?

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    connect(socketFd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    main_loop(socketFd);

    return 0;
}