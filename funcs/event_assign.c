//
// Created by hlhd on 2021/5/7.
//

#include "../all.h"

void stdInCallback(evutil_socket_t fd, short what, void* arg)
{
    struct event* ev = (struct event*)arg;
    char buf[1024];
    printf("stdin_callback start\n");
    int ret = read(fd, buf, sizeof(buf));
    buf[ret] = '\0';
    printf("buf is %s\n", buf);
    printf("stdin_callback end\n");
    event_del(ev);
}

int mainea()
{
    struct event_base* base = event_base_new();
    event_base_priority_init(base, 4);

    struct event* ev0 = event_new(base, STDIN_FILENO, 0, NULL, NULL);
    event_assign(ev0, base, STDIN_FILENO, EV_READ | EV_PERSIST, stdInCallback, ev0);
    event_priority_set(ev0, 3);
    event_add(ev0, NULL);

    // two event -- same fd
    struct event* ev1 = event_new(base, STDIN_FILENO, 0, NULL, NULL);
    event_assign(ev1, base, STDIN_FILENO, EV_READ | EV_PERSIST, stdInCallback, ev1);
    event_priority_set(ev1, 2);
    event_add(ev1, NULL);

    /* event loop */
    event_base_dispatch(base);

    event_base_free(base);
    event_free(ev0);             // "event_base" first free, "event" then free. But why??
    event_free(ev1);

    return 0;
}

