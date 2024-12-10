#include "epoll_mgr.h"

#include <android-base/logging.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <utils/Log.h>

//private

//public
epoll_mgr::epoll_mgr()
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        ALOGE("Failed to create epoll; %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

epoll_mgr::~epoll_mgr()
{
    for (const auto& pair : subscribers) {
        ALOGI("Closing subscriber %d", pair.first);
        remove_subscriber(pair.second);
    }
    close(epoll_fd);
}

void epoll_mgr::add_subscriber(std::shared_ptr<epoll_subscriber> sub)
{
    for (int fd : sub->get_event_fds()) {
        if (subscribers.count(fd)) {
            ALOGE("epoll_mgr already contains event_fd; cannot add twice");
            exit(EXIT_FAILURE);
        }

        struct epoll_event event = {0};
        event.events = EPOLLIN;
        event.data.fd = fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
            ALOGE("Failed to add fd to epoll; errno=%d", errno);
            exit(EXIT_FAILURE);
        }
        ALOGI("adding epoll_subscriber: fd=%d", fd);
        subscribers[fd] = sub;
    }
}

void epoll_mgr::remove_subscriber(std::shared_ptr<epoll_subscriber> sub)
{
    for (int fd : sub->get_event_fds()) {
        if (!subscribers.count(fd)) {
            ALOGE("epoll_mgr doesn't contain event_fd; cannot remove: %d", fd);
            exit(EXIT_FAILURE);
        }
        if (subscribers[fd] != sub) {
            ALOGE("subscriber to be removed matches fd of other subscriber");
            exit(EXIT_FAILURE);
        }

        struct epoll_event event = {0};
        event.events = EPOLLIN;
        event.data.fd = fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event)) {
            ALOGE("Failed to remove fd from epoll; errno=%d", errno);
            exit(EXIT_FAILURE);
        }
        subscribers.erase(fd);
    }
}

static const int MAX_EVENTS = 10;
static const int TIMEOUT = 500;
void epoll_mgr::loop()
{
    struct epoll_event events[MAX_EVENTS];
    int nfds;

    nfds = epoll_pwait(epoll_fd, events, MAX_EVENTS, TIMEOUT, nullptr);
    if (nfds == -1) {
        ALOGE("epoll_pwait failure");
        return;
    }

    for (int i = 0; i < nfds; i++) {
        int e_fd = events[i].data.fd;
        if (subscribers.count(e_fd))
            (*subscribers[e_fd])(e_fd);
        else
            ALOGE("fd not found in subscribers map");
    }
}

