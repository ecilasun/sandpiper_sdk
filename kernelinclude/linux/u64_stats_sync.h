/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_U64_STATS_SYNC_H
#define _LINUX_U64_STATS_SYNC_H


#include <linux/seqlock.h>

struct u64_stats_sync {
#if BITS_PER_LONG == 32
	seqcount_t	seq;
#endif
};

#if BITS_PER_LONG == 64
#include <asm/local64.h>

typedef struct {
	local64_t	v;
} u64_stats_t ;

static inline u64 u64_stats_read(const u64_stats_t *p)
{
	return local64_read(&p->v);
}

static inline void u64_stats_set(u64_stats_t *p, u64 val)
{
	local64_set(&p->v, val);
}

static inline void u64_stats_add(u64_stats_t *p, unsigned long val)
{
	local64_add(val, &p->v);
}

static inline void u64_stats_inc(u64_stats_t *p)
{
	local64_inc(&p->v);
}

static inline void u64_stats_init(struct u64_stats_sync *syncp) { }
static inline void __u64_stats_update_begin(struct u64_stats_sync *syncp) { }
static inline void __u64_stats_update_end(struct u64_stats_sync *syncp) { }
static inline unsigned long __u64_stats_irqsave(void) { return 0; }
static inline void __u64_stats_irqrestore(unsigned long flags) { }
static inline unsigned int __u64_stats_fetch_begin(const struct u64_stats_sync *syncp)
{
	return 0;
}
static inline bool __u64_stats_fetch_retry(const struct u64_stats_sync *syncp,
					   unsigned int start)
{
	return false;
}

#else 

typedef struct {
	u64		v;
} u64_stats_t;

static inline u64 u64_stats_read(const u64_stats_t *p)
{
	return p->v;
}

static inline void u64_stats_set(u64_stats_t *p, u64 val)
{
	p->v = val;
}

static inline void u64_stats_add(u64_stats_t *p, unsigned long val)
{
	p->v += val;
}

static inline void u64_stats_inc(u64_stats_t *p)
{
	p->v++;
}

#define u64_stats_init(syncp)				\
	do {						\
		struct u64_stats_sync *__s = (syncp);	\
		seqcount_init(&__s->seq);		\
	} while (0)

static inline void __u64_stats_update_begin(struct u64_stats_sync *syncp)
{
	preempt_disable_nested();
	write_seqcount_begin(&syncp->seq);
}

static inline void __u64_stats_update_end(struct u64_stats_sync *syncp)
{
	write_seqcount_end(&syncp->seq);
	preempt_enable_nested();
}

static inline unsigned long __u64_stats_irqsave(void)
{
	unsigned long flags;

	local_irq_save(flags);
	return flags;
}

static inline void __u64_stats_irqrestore(unsigned long flags)
{
	local_irq_restore(flags);
}

static inline unsigned int __u64_stats_fetch_begin(const struct u64_stats_sync *syncp)
{
	return read_seqcount_begin(&syncp->seq);
}

static inline bool __u64_stats_fetch_retry(const struct u64_stats_sync *syncp,
					   unsigned int start)
{
	return read_seqcount_retry(&syncp->seq, start);
}
#endif 

static inline void u64_stats_update_begin(struct u64_stats_sync *syncp)
{
	__u64_stats_update_begin(syncp);
}

static inline void u64_stats_update_end(struct u64_stats_sync *syncp)
{
	__u64_stats_update_end(syncp);
}

static inline unsigned long u64_stats_update_begin_irqsave(struct u64_stats_sync *syncp)
{
	unsigned long flags = __u64_stats_irqsave();

	__u64_stats_update_begin(syncp);
	return flags;
}

static inline void u64_stats_update_end_irqrestore(struct u64_stats_sync *syncp,
						   unsigned long flags)
{
	__u64_stats_update_end(syncp);
	__u64_stats_irqrestore(flags);
}

static inline unsigned int u64_stats_fetch_begin(const struct u64_stats_sync *syncp)
{
	return __u64_stats_fetch_begin(syncp);
}

static inline bool u64_stats_fetch_retry(const struct u64_stats_sync *syncp,
					 unsigned int start)
{
	return __u64_stats_fetch_retry(syncp, start);
}

#endif 
