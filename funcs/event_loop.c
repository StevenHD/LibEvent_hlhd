//
// Created by hlhd on 2021/5/7.
//

#include "../all.h"

void stdInCallbackLP(evutil_socket_t fd, short what, void* arg)
{
    static int i = 0;
    char buf[1024];
    printf("stdin_callback start\n");
    int ret = read(fd, buf, sizeof(buf));
    buf[ret] = '\0';
    printf("buf is %s\n", buf);
    printf("stdin_callback end\n");

    struct event_base* base = (struct event_base*)arg;
    // if (++ i >= 5) event_base_loopbreak(base);

    if (++ i >= 1)
    {
        struct timeval timeout = {1, 0};
        event_base_loopexit(base, &timeout);  // "loopexit" is more portable than "loopbreak"
    }
}

int mainLP()
{
    struct event_base* base = event_base_new();

    struct event* ev0 = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, stdInCallbackLP, base);
    event_add(ev0, NULL);

    struct event* ev1 = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST, stdInCallbackLP, base);
    event_add(ev1, NULL);

    /* event loop */
    /* event_base_dispatch(base); = event_base_loop(base, 0); */
    // event_base_loop(base, EVLOOP_ONCE);
    // event_base_loop(base, EVLOOP_NONBLOCK);
    event_base_dispatch(base);

    if (event_base_got_break(base))     puts("event base loop break");
    else if (event_base_got_exit(base)) puts("event base loop exit");

    event_base_free(base);
    event_free(ev0);
    event_free(ev1);

    return 0;
}
