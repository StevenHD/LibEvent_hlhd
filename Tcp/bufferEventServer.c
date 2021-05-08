//
// Created by hlhd on 2021/5/8.
//

#include "../all.h"

#define _SERVER_PORT_ 8888

void readBufferCb(struct bufferevent* bev, void* ctx)
{
    puts("readBufferCb");
    char buf[1024];
    int ret = bufferevent_read(bev, buf, sizeof(buf));
    for (int i = 0; i < ret; i ++ ) buf[i] = toupper(buf[i]);
    bufferevent_write(bev, buf, ret);
}

void writeBufferCb(struct bufferevent* bev, void* ctx)
{
    puts("writeBufferCb");
    // bufferevent_write(bev, "hello", sizeof("hello"));
}

void eventCallbak(struct bufferevent* bev, short what, void* ctx)
{
    if (BEV_EVENT_READING & what) puts("BEV_EVENT_READING");
    if (BEV_EVENT_WRITING & what) puts("BEV_EVENT_WRITING");

    if (BEV_EVENT_EOF & what)
    {
        puts("BEV_EVENT_EOF");
        bufferevent_free(bev);
    }

    if (BEV_EVENT_ERROR & what)
    {
        printf("BEV_EVENT_ERROR %d\n", EVUTIL_SOCKET_ERROR());
        bufferevent_free(bev);
    }

    if (BEV_EVENT_TIMEOUT & what)
    {
        puts("BEV_EVENT_TIMEOUT");
        bufferevent_free(bev);
    }
}

void accecptBufferCallback(evutil_socket_t fd, short what, void* arg)
{
    /* client's "connection request" */
    int clntFd;
    struct event_base* base = (struct event_base*)arg;
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t len = sizeof(client_addr);

    printf("accecptBufferCallback\n");
    clntFd = accept(fd, (struct sockaddr*)&client_addr, &len);
    if (clntFd != -1)
    {
        /* first set "socket" as nonblock */
        evutil_make_socket_nonblocking(clntFd);

        struct bufferevent* bev = bufferevent_socket_new(base, clntFd, BEV_OPT_CLOSE_ON_FREE
                                    | BEV_OPT_DEFER_CALLBACKS);
        bufferevent_setcb(bev, readBufferCb, writeBufferCb, eventCallbak, NULL);
        bufferevent_setwatermark(bev, EV_READ, 10, 0);

        /* set timeout */
        struct timeval timeout = {5, 0};
        bufferevent_set_timeouts(bev, &timeout, NULL);

        bufferevent_enable(bev, EV_READ);
    }
}

void main_loop_bufEvent(int lsnFd)
{
    struct event_base* base    = event_base_new();
    struct event* lsnEvent     = event_new(base, lsnFd, EV_READ | EV_PERSIST | EV_ET, accecptBufferCallback, base);
    event_add(lsnEvent, NULL);

    /* Launch event-loop */
    event_base_dispatch(base);

    event_del(lsnEvent);

    event_free(lsnEvent);
    event_base_free(base);
}

int main()
{
    /* Server's address struct init */
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family          = AF_INET;
    server_addr.sin_port            = htons(_SERVER_PORT_);
    server_addr.sin_addr.s_addr     = htonl(INADDR_ANY);

    /* Create socket and bind */
    int lsnFd = socket(AF_INET, SOCK_STREAM, 0);  /* lsnFd do not need buffer, clntFd need buffer */
    evutil_make_listen_socket_reuseable(lsnFd);
    if (bind(lsnFd, (struct sockaddr_in*)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(0);
    }
    listen(lsnFd, 128);

    /* event loop */
    main_loop_bufEvent(lsnFd);

    return 0;
}
