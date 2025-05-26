/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FPROBE_H
#define _LINUX_FPROBE_H

#include <linux/compiler.h>
#include <linux/ftrace.h>
#include <linux/rethook.h>


struct fprobe {
#ifdef CONFIG_FUNCTION_TRACER
	
	struct ftrace_ops	ops;
#endif
	unsigned long		nmissed;
	unsigned int		flags;
	struct rethook		*rethook;
	size_t			entry_data_size;
	int			nr_maxactive;

	int (*entry_handler)(struct fprobe *fp, unsigned long entry_ip,
			     unsigned long ret_ip, struct pt_regs *regs,
			     void *entry_data);
	void (*exit_handler)(struct fprobe *fp, unsigned long entry_ip,
			     unsigned long ret_ip, struct pt_regs *regs,
			     void *entry_data);
};


#define FPROBE_FL_DISABLED	1


#define FPROBE_FL_KPROBE_SHARED	2

static inline bool fprobe_disabled(struct fprobe *fp)
{
	return (fp) ? fp->flags & FPROBE_FL_DISABLED : false;
}

static inline bool fprobe_shared_with_kprobes(struct fprobe *fp)
{
	return (fp) ? fp->flags & FPROBE_FL_KPROBE_SHARED : false;
}

#ifdef CONFIG_FPROBE
int register_fprobe(struct fprobe *fp, const char *filter, const char *notfilter);
int register_fprobe_ips(struct fprobe *fp, unsigned long *addrs, int num);
int register_fprobe_syms(struct fprobe *fp, const char **syms, int num);
int unregister_fprobe(struct fprobe *fp);
bool fprobe_is_registered(struct fprobe *fp);
#else
static inline int register_fprobe(struct fprobe *fp, const char *filter, const char *notfilter)
{
	return -EOPNOTSUPP;
}
static inline int register_fprobe_ips(struct fprobe *fp, unsigned long *addrs, int num)
{
	return -EOPNOTSUPP;
}
static inline int register_fprobe_syms(struct fprobe *fp, const char **syms, int num)
{
	return -EOPNOTSUPP;
}
static inline int unregister_fprobe(struct fprobe *fp)
{
	return -EOPNOTSUPP;
}
static inline bool fprobe_is_registered(struct fprobe *fp)
{
	return false;
}
#endif


static inline void disable_fprobe(struct fprobe *fp)
{
	if (fp)
		fp->flags |= FPROBE_FL_DISABLED;
}


static inline void enable_fprobe(struct fprobe *fp)
{
	if (fp)
		fp->flags &= ~FPROBE_FL_DISABLED;
}

#endif
