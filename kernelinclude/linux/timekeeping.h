/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TIMEKEEPING_H
#define _LINUX_TIMEKEEPING_H

#include <linux/errno.h>
#include <linux/clocksource_ids.h>



void timekeeping_init(void);
extern int timekeeping_suspended;


extern void legacy_timer_tick(unsigned long ticks);


extern int do_settimeofday64(const struct timespec64 *ts);
extern int do_sys_settimeofday64(const struct timespec64 *tv,
				 const struct timezone *tz);





extern void ktime_get_raw_ts64(struct timespec64 *ts);
extern void ktime_get_ts64(struct timespec64 *ts);
extern void ktime_get_real_ts64(struct timespec64 *tv);
extern void ktime_get_coarse_ts64(struct timespec64 *ts);
extern void ktime_get_coarse_real_ts64(struct timespec64 *ts);

void getboottime64(struct timespec64 *ts);


extern time64_t ktime_get_seconds(void);
extern time64_t __ktime_get_real_seconds(void);
extern time64_t ktime_get_real_seconds(void);



enum tk_offsets {
	TK_OFFS_REAL,
	TK_OFFS_BOOT,
	TK_OFFS_TAI,
	TK_OFFS_MAX,
};

extern ktime_t ktime_get(void);
extern ktime_t ktime_get_with_offset(enum tk_offsets offs);
extern ktime_t ktime_get_coarse_with_offset(enum tk_offsets offs);
extern ktime_t ktime_mono_to_any(ktime_t tmono, enum tk_offsets offs);
extern ktime_t ktime_get_raw(void);
extern u32 ktime_get_resolution_ns(void);


static inline ktime_t ktime_get_real(void)
{
	return ktime_get_with_offset(TK_OFFS_REAL);
}

static inline ktime_t ktime_get_coarse_real(void)
{
	return ktime_get_coarse_with_offset(TK_OFFS_REAL);
}


static inline ktime_t ktime_get_boottime(void)
{
	return ktime_get_with_offset(TK_OFFS_BOOT);
}

static inline ktime_t ktime_get_coarse_boottime(void)
{
	return ktime_get_coarse_with_offset(TK_OFFS_BOOT);
}


static inline ktime_t ktime_get_clocktai(void)
{
	return ktime_get_with_offset(TK_OFFS_TAI);
}

static inline ktime_t ktime_get_coarse_clocktai(void)
{
	return ktime_get_coarse_with_offset(TK_OFFS_TAI);
}

static inline ktime_t ktime_get_coarse(void)
{
	struct timespec64 ts;

	ktime_get_coarse_ts64(&ts);
	return timespec64_to_ktime(ts);
}

static inline u64 ktime_get_coarse_ns(void)
{
	return ktime_to_ns(ktime_get_coarse());
}

static inline u64 ktime_get_coarse_real_ns(void)
{
	return ktime_to_ns(ktime_get_coarse_real());
}

static inline u64 ktime_get_coarse_boottime_ns(void)
{
	return ktime_to_ns(ktime_get_coarse_boottime());
}

static inline u64 ktime_get_coarse_clocktai_ns(void)
{
	return ktime_to_ns(ktime_get_coarse_clocktai());
}


static inline ktime_t ktime_mono_to_real(ktime_t mono)
{
	return ktime_mono_to_any(mono, TK_OFFS_REAL);
}

static inline u64 ktime_get_ns(void)
{
	return ktime_to_ns(ktime_get());
}

static inline u64 ktime_get_real_ns(void)
{
	return ktime_to_ns(ktime_get_real());
}

static inline u64 ktime_get_boottime_ns(void)
{
	return ktime_to_ns(ktime_get_boottime());
}

static inline u64 ktime_get_clocktai_ns(void)
{
	return ktime_to_ns(ktime_get_clocktai());
}

static inline u64 ktime_get_raw_ns(void)
{
	return ktime_to_ns(ktime_get_raw());
}

extern u64 ktime_get_mono_fast_ns(void);
extern u64 ktime_get_raw_fast_ns(void);
extern u64 ktime_get_boot_fast_ns(void);
extern u64 ktime_get_tai_fast_ns(void);
extern u64 ktime_get_real_fast_ns(void);


static inline void ktime_get_boottime_ts64(struct timespec64 *ts)
{
	*ts = ktime_to_timespec64(ktime_get_boottime());
}

static inline void ktime_get_coarse_boottime_ts64(struct timespec64 *ts)
{
	*ts = ktime_to_timespec64(ktime_get_coarse_boottime());
}

static inline time64_t ktime_get_boottime_seconds(void)
{
	return ktime_divns(ktime_get_coarse_boottime(), NSEC_PER_SEC);
}

static inline void ktime_get_clocktai_ts64(struct timespec64 *ts)
{
	*ts = ktime_to_timespec64(ktime_get_clocktai());
}

static inline void ktime_get_coarse_clocktai_ts64(struct timespec64 *ts)
{
	*ts = ktime_to_timespec64(ktime_get_coarse_clocktai());
}

static inline time64_t ktime_get_clocktai_seconds(void)
{
	return ktime_divns(ktime_get_coarse_clocktai(), NSEC_PER_SEC);
}


extern bool timekeeping_rtc_skipsuspend(void);
extern bool timekeeping_rtc_skipresume(void);

extern void timekeeping_inject_sleeptime64(const struct timespec64 *delta);


struct ktime_timestamps {
	u64		mono;
	u64		boot;
	u64		real;
};


struct system_time_snapshot {
	u64			cycles;
	ktime_t			real;
	ktime_t			raw;
	enum clocksource_ids	cs_id;
	unsigned int		clock_was_set_seq;
	u8			cs_was_changed_seq;
};


struct system_device_crosststamp {
	ktime_t device;
	ktime_t sys_realtime;
	ktime_t sys_monoraw;
};


struct system_counterval_t {
	u64			cycles;
	struct clocksource	*cs;
};


extern int get_device_system_crosststamp(
			int (*get_time_fn)(ktime_t *device_time,
				struct system_counterval_t *system_counterval,
				void *ctx),
			void *ctx,
			struct system_time_snapshot *history,
			struct system_device_crosststamp *xtstamp);


extern void ktime_get_snapshot(struct system_time_snapshot *systime_snapshot);


extern void ktime_get_fast_timestamps(struct ktime_timestamps *snap);


extern int persistent_clock_is_local;

extern void read_persistent_clock64(struct timespec64 *ts);
void read_persistent_wall_and_boot_offset(struct timespec64 *wall_clock,
					  struct timespec64 *boot_offset);
#ifdef CONFIG_GENERIC_CMOS_UPDATE
extern int update_persistent_clock64(struct timespec64 now);
#endif

#endif
