/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_EVENTPOLL_H
#define _LINUX_EVENTPOLL_H

#include <uapi/linux/eventpoll.h>
#include <uapi/linux/kcmp.h>



struct file;


#ifdef CONFIG_EPOLL

#ifdef CONFIG_KCMP
struct file *get_epoll_tfile_raw_ptr(struct file *file, int tfd, unsigned long toff);
#endif


void eventpoll_release_file(struct file *file);


static inline void eventpoll_release(struct file *file)
{

	
	if (likely(!file->f_ep))
		return;

	
	eventpoll_release_file(file);
}

int do_epoll_ctl(int epfd, int op, int fd, struct epoll_event *epds,
		 bool nonblock);


static inline int ep_op_has_event(int op)
{
	return op != EPOLL_CTL_DEL;
}

#else

static inline void eventpoll_release(struct file *file) {}

#endif

#if defined(CONFIG_ARM) && defined(CONFIG_OABI_COMPAT)

extern struct epoll_event __user *
epoll_put_uevent(__poll_t revents, __u64 data,
		 struct epoll_event __user *uevent);
#else
static inline struct epoll_event __user *
epoll_put_uevent(__poll_t revents, __u64 data,
		 struct epoll_event __user *uevent)
{
	if (__put_user(revents, &uevent->events) ||
	    __put_user(data, &uevent->data))
		return NULL;

	return uevent+1;
}
#endif

#endif 
