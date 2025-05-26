/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_REGSET_H
#define _LINUX_REGSET_H	1

#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/bug.h>
#include <linux/uaccess.h>
struct task_struct;
struct user_regset;

struct membuf {
	void *p;
	size_t left;
};

static inline int membuf_zero(struct membuf *s, size_t size)
{
	if (s->left) {
		if (size > s->left)
			size = s->left;
		memset(s->p, 0, size);
		s->p += size;
		s->left -= size;
	}
	return s->left;
}

static inline int membuf_write(struct membuf *s, const void *v, size_t size)
{
	if (s->left) {
		if (size > s->left)
			size = s->left;
		memcpy(s->p, v, size);
		s->p += size;
		s->left -= size;
	}
	return s->left;
}

static inline struct membuf membuf_at(const struct membuf *s, size_t offs)
{
	struct membuf n = *s;

	if (offs > n.left)
		offs = n.left;
	n.p += offs;
	n.left -= offs;

	return n;
}


#define membuf_store(s, v)				\
({							\
	struct membuf *__s = (s);			\
        if (__s->left) {				\
		typeof(v) __v = (v);			\
		size_t __size = sizeof(__v);		\
		if (unlikely(__size > __s->left)) {	\
			__size = __s->left;		\
			memcpy(__s->p, &__v, __size);	\
		} else {				\
			*(typeof(__v + 0) *)__s->p = __v;	\
		}					\
		__s->p += __size;			\
		__s->left -= __size;			\
	}						\
	__s->left;})


typedef int user_regset_active_fn(struct task_struct *target,
				  const struct user_regset *regset);

typedef int user_regset_get2_fn(struct task_struct *target,
			       const struct user_regset *regset,
			       struct membuf to);


typedef int user_regset_set_fn(struct task_struct *target,
			       const struct user_regset *regset,
			       unsigned int pos, unsigned int count,
			       const void *kbuf, const void __user *ubuf);


typedef int user_regset_writeback_fn(struct task_struct *target,
				     const struct user_regset *regset,
				     int immediate);


struct user_regset {
	user_regset_get2_fn		*regset_get;
	user_regset_set_fn		*set;
	user_regset_active_fn		*active;
	user_regset_writeback_fn	*writeback;
	unsigned int			n;
	unsigned int 			size;
	unsigned int 			align;
	unsigned int 			bias;
	unsigned int 			core_note_type;
};


struct user_regset_view {
	const char *name;
	const struct user_regset *regsets;
	unsigned int n;
	u32 e_flags;
	u16 e_machine;
	u8 ei_osabi;
};



const struct user_regset_view *task_user_regset_view(struct task_struct *tsk);

static inline int user_regset_copyin(unsigned int *pos, unsigned int *count,
				     const void **kbuf,
				     const void __user **ubuf, void *data,
				     const int start_pos, const int end_pos)
{
	if (*count == 0)
		return 0;
	BUG_ON(*pos < start_pos);
	if (end_pos < 0 || *pos < end_pos) {
		unsigned int copy = (end_pos < 0 ? *count
				     : min(*count, end_pos - *pos));
		data += *pos - start_pos;
		if (*kbuf) {
			memcpy(data, *kbuf, copy);
			*kbuf += copy;
		} else if (__copy_from_user(data, *ubuf, copy))
			return -EFAULT;
		else
			*ubuf += copy;
		*pos += copy;
		*count -= copy;
	}
	return 0;
}

static inline void user_regset_copyin_ignore(unsigned int *pos,
					     unsigned int *count,
					     const void **kbuf,
					     const void __user **ubuf,
					     const int start_pos,
					     const int end_pos)
{
	if (*count == 0)
		return;
	BUG_ON(*pos < start_pos);
	if (end_pos < 0 || *pos < end_pos) {
		unsigned int copy = (end_pos < 0 ? *count
				     : min(*count, end_pos - *pos));
		if (*kbuf)
			*kbuf += copy;
		else
			*ubuf += copy;
		*pos += copy;
		*count -= copy;
	}
}

extern int regset_get(struct task_struct *target,
		      const struct user_regset *regset,
		      unsigned int size, void *data);

extern int regset_get_alloc(struct task_struct *target,
			    const struct user_regset *regset,
			    unsigned int size,
			    void **data);

extern int copy_regset_to_user(struct task_struct *target,
			       const struct user_regset_view *view,
			       unsigned int setno, unsigned int offset,
			       unsigned int size, void __user *data);


static inline int copy_regset_from_user(struct task_struct *target,
					const struct user_regset_view *view,
					unsigned int setno,
					unsigned int offset, unsigned int size,
					const void __user *data)
{
	const struct user_regset *regset = &view->regsets[setno];

	if (!regset->set)
		return -EOPNOTSUPP;

	if (!access_ok(data, size))
		return -EFAULT;

	return regset->set(target, regset, offset, size, NULL, data);
}

#endif	
