//
// Created by hlhd on 2021/5/8.
//

//
// Created by hlhd on 2021/5/8.
//

#include "../all.h"

#define _SERVER_PORT_ 8888
#define _SERVER_IP_   "127.0.0.1"

void readStdInCallbackBuffer(evutil_socket_t fd, short what, void* arg)
{
    struct bufferevent* bev = (struct bufferevent*)arg;
    char buf[1024];
    int socketFd = (int)arg;
    int ret;
    ret = read(STDIN_FILENO, buf, sizeof(buf));
    // write(socketFd, buf, ret);
    bufferevent_write(bev, buf, ret);
}

void readBufCallbak(struct bufferevent* bev, void* ctx)
{
    puts("readBufCallbak");
    char buf[1024];
    int ret = bufferevent_read(bev, buf, sizeof(buf));
    write(STDOUT_FILENO, buf, ret);
}

void writeBufCallbak(struct bufferevent* bev, void* ctx)
{
    puts("writeBufferCb");
}

void eventBufCallbak(struct bufferevent* bev, short what, void* ctx)
{
    struct event_base* base = (struct event_base*)ctx;
    if (BEV_EVENT_READING & what) puts("BEV_EVENT_READING");
    if (BEV_EVENT_WRITING & what) puts("BEV_EVENT_WRITING");

    if (BEV_EVENT_EOF & what)
    {
        puts("BEV_EVENT_EOF");
        event_base_loopexit(base, NULL);
    }

    if (BEV_EVENT_ERROR & what)
    {
        printf("BEV_EVENT_ERROR %d\n", EVUTIL_SOCKET_ERROR());
        event_base_loopexit(base, NULL);
    }

    if (BEV_EVENT_TIMEOUT & what)
    {
        puts("BEV_EVENT_TIMEOUT");
        event_base_loopexit(base, NULL);
    }

    if (BEV_EVENT_CONNECTED & what)
    {
        puts("BEV_EVENT_CONNECTED");
    }
}

void mainLoopBufferClient(int socketFd)
{
    struct event_base* base       = event_base_new();
    struct bufferevent* bev = bufferevent_socket_new(base, socketFd,
                        BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
    bufferevent_setcb(bev, readBufCallbak, writeBufCallbak, eventBufCallbak, base);
    bufferevent_setwatermark(bev, EV_READ, 10, 0);
    bufferevent_enable(bev, EV_READ);

    struct event* stdInEvent     = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST | EV_ET,
                                             readStdInCallbackBuffer, (void*)bev);
    event_add(stdInEvent, NULL);

    event_base_dispatch(base);
    bufferevent_free(bev);

    event_del(stdInEvent);
    event_free(stdInEvent);

    event_base_free(base);
}

int main()
{
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family          = AF_INET;
    server_addr.sin_port            = htons(_SERVER_PORT_);
    inet_pton(AF_INET, _SERVER_IP_, &server_addr.sin_addr.s_addr);      // ?

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    connect(socketFd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    mainLoopBufferClient(socketFd);

    return 0;
}
