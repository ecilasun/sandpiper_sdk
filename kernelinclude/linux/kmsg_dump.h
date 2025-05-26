
#ifndef _LINUX_KMSG_DUMP_H
#define _LINUX_KMSG_DUMP_H

#include <linux/errno.h>
#include <linux/list.h>


enum kmsg_dump_reason {
	KMSG_DUMP_UNDEF,
	KMSG_DUMP_PANIC,
	KMSG_DUMP_OOPS,
	KMSG_DUMP_EMERG,
	KMSG_DUMP_SHUTDOWN,
	KMSG_DUMP_MAX
};


struct kmsg_dump_iter {
	u64	cur_seq;
	u64	next_seq;
};


struct kmsg_dumper {
	struct list_head list;
	void (*dump)(struct kmsg_dumper *dumper, enum kmsg_dump_reason reason);
	enum kmsg_dump_reason max_reason;
	bool registered;
};

#ifdef CONFIG_PRINTK
void kmsg_dump(enum kmsg_dump_reason reason);

bool kmsg_dump_get_line(struct kmsg_dump_iter *iter, bool syslog,
			char *line, size_t size, size_t *len);

bool kmsg_dump_get_buffer(struct kmsg_dump_iter *iter, bool syslog,
			  char *buf, size_t size, size_t *len_out);

void kmsg_dump_rewind(struct kmsg_dump_iter *iter);

int kmsg_dump_register(struct kmsg_dumper *dumper);

int kmsg_dump_unregister(struct kmsg_dumper *dumper);

const char *kmsg_dump_reason_str(enum kmsg_dump_reason reason);
#else
static inline void kmsg_dump(enum kmsg_dump_reason reason)
{
}

static inline bool kmsg_dump_get_line(struct kmsg_dump_iter *iter, bool syslog,
				const char *line, size_t size, size_t *len)
{
	return false;
}

static inline bool kmsg_dump_get_buffer(struct kmsg_dump_iter *iter, bool syslog,
					char *buf, size_t size, size_t *len)
{
	return false;
}

static inline void kmsg_dump_rewind(struct kmsg_dump_iter *iter)
{
}

static inline int kmsg_dump_register(struct kmsg_dumper *dumper)
{
	return -EINVAL;
}

static inline int kmsg_dump_unregister(struct kmsg_dumper *dumper)
{
	return -EINVAL;
}

static inline const char *kmsg_dump_reason_str(enum kmsg_dump_reason reason)
{
	return "Disabled";
}
#endif

#endif 
