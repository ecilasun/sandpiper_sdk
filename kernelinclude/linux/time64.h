/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIME64_H
#define _LINUX_TIME64_H

#include <linux/math64.h>
#include <vdso/time64.h>

typedef __s64 time64_t;
typedef __u64 timeu64_t;

#include <uapi/linux/time.h>

struct timespec64 {
	time64_t	tv_sec;			
	long		tv_nsec;		
};

struct itimerspec64 {
	struct timespec64 it_interval;
	struct timespec64 it_value;
};


#define PSEC_PER_NSEC			1000L


#define TIME64_MAX			((s64)~((u64)1 << 63))
#define TIME64_MIN			(-TIME64_MAX - 1)

#define KTIME_MAX			((s64)~((u64)1 << 63))
#define KTIME_MIN			(-KTIME_MAX - 1)
#define KTIME_SEC_MAX			(KTIME_MAX / NSEC_PER_SEC)
#define KTIME_SEC_MIN			(KTIME_MIN / NSEC_PER_SEC)


#define TIME_UPTIME_SEC_MAX		(30LL * 365 * 24 *3600)
#define TIME_SETTOD_SEC_MAX		(KTIME_SEC_MAX - TIME_UPTIME_SEC_MAX)

static inline int timespec64_equal(const struct timespec64 *a,
				   const struct timespec64 *b)
{
	return (a->tv_sec == b->tv_sec) && (a->tv_nsec == b->tv_nsec);
}


static inline int timespec64_compare(const struct timespec64 *lhs, const struct timespec64 *rhs)
{
	if (lhs->tv_sec < rhs->tv_sec)
		return -1;
	if (lhs->tv_sec > rhs->tv_sec)
		return 1;
	return lhs->tv_nsec - rhs->tv_nsec;
}

extern void set_normalized_timespec64(struct timespec64 *ts, time64_t sec, s64 nsec);

static inline struct timespec64 timespec64_add(struct timespec64 lhs,
						struct timespec64 rhs)
{
	struct timespec64 ts_delta;
	set_normalized_timespec64(&ts_delta, lhs.tv_sec + rhs.tv_sec,
				lhs.tv_nsec + rhs.tv_nsec);
	return ts_delta;
}


static inline struct timespec64 timespec64_sub(struct timespec64 lhs,
						struct timespec64 rhs)
{
	struct timespec64 ts_delta;
	set_normalized_timespec64(&ts_delta, lhs.tv_sec - rhs.tv_sec,
				lhs.tv_nsec - rhs.tv_nsec);
	return ts_delta;
}


static inline bool timespec64_valid(const struct timespec64 *ts)
{
	
	if (ts->tv_sec < 0)
		return false;
	
	if ((unsigned long)ts->tv_nsec >= NSEC_PER_SEC)
		return false;
	return true;
}

static inline bool timespec64_valid_strict(const struct timespec64 *ts)
{
	if (!timespec64_valid(ts))
		return false;
	
	if ((unsigned long long)ts->tv_sec >= KTIME_SEC_MAX)
		return false;
	return true;
}

static inline bool timespec64_valid_settod(const struct timespec64 *ts)
{
	if (!timespec64_valid(ts))
		return false;
	
	if ((unsigned long long)ts->tv_sec >= TIME_SETTOD_SEC_MAX)
		return false;
	return true;
}


static inline s64 timespec64_to_ns(const struct timespec64 *ts)
{
	
	if (ts->tv_sec >= KTIME_SEC_MAX)
		return KTIME_MAX;

	if (ts->tv_sec <= KTIME_SEC_MIN)
		return KTIME_MIN;

	return ((s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}


extern struct timespec64 ns_to_timespec64(s64 nsec);


static __always_inline void timespec64_add_ns(struct timespec64 *a, u64 ns)
{
	a->tv_sec += __iter_div_u64_rem(a->tv_nsec + ns, NSEC_PER_SEC, &ns);
	a->tv_nsec = ns;
}


extern struct timespec64 timespec64_add_safe(const struct timespec64 lhs,
					 const struct timespec64 rhs);

#endif 
