/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _PTP_CLOCK_KERNEL_H_
#define _PTP_CLOCK_KERNEL_H_

#include <linux/device.h>
#include <linux/pps_kernel.h>
#include <linux/ptp_clock.h>
#include <linux/timecounter.h>
#include <linux/skbuff.h>

#define PTP_CLOCK_NAME_LEN	32


struct ptp_clock_request {
	enum {
		PTP_CLK_REQ_EXTTS,
		PTP_CLK_REQ_PEROUT,
		PTP_CLK_REQ_PPS,
	} type;
	union {
		struct ptp_extts_request extts;
		struct ptp_perout_request perout;
	};
};

struct system_device_crosststamp;


struct ptp_system_timestamp {
	struct timespec64 pre_ts;
	struct timespec64 post_ts;
};



struct ptp_clock_info {
	struct module *owner;
	char name[PTP_CLOCK_NAME_LEN];
	s32 max_adj;
	int n_alarm;
	int n_ext_ts;
	int n_per_out;
	int n_pins;
	int pps;
	struct ptp_pin_desc *pin_config;
	int (*adjfine)(struct ptp_clock_info *ptp, long scaled_ppm);
	int (*adjphase)(struct ptp_clock_info *ptp, s32 phase);
	s32 (*getmaxphase)(struct ptp_clock_info *ptp);
	int (*adjtime)(struct ptp_clock_info *ptp, s64 delta);
	int (*gettime64)(struct ptp_clock_info *ptp, struct timespec64 *ts);
	int (*gettimex64)(struct ptp_clock_info *ptp, struct timespec64 *ts,
			  struct ptp_system_timestamp *sts);
	int (*getcrosststamp)(struct ptp_clock_info *ptp,
			      struct system_device_crosststamp *cts);
	int (*settime64)(struct ptp_clock_info *p, const struct timespec64 *ts);
	int (*getcycles64)(struct ptp_clock_info *ptp, struct timespec64 *ts);
	int (*getcyclesx64)(struct ptp_clock_info *ptp, struct timespec64 *ts,
			    struct ptp_system_timestamp *sts);
	int (*getcrosscycles)(struct ptp_clock_info *ptp,
			      struct system_device_crosststamp *cts);
	int (*enable)(struct ptp_clock_info *ptp,
		      struct ptp_clock_request *request, int on);
	int (*verify)(struct ptp_clock_info *ptp, unsigned int pin,
		      enum ptp_pin_function func, unsigned int chan);
	long (*do_aux_work)(struct ptp_clock_info *ptp);
};

struct ptp_clock;

enum ptp_clock_events {
	PTP_CLOCK_ALARM,
	PTP_CLOCK_EXTTS,
	PTP_CLOCK_PPS,
	PTP_CLOCK_PPSUSR,
};



struct ptp_clock_event {
	int type;
	int index;
	union {
		u64 timestamp;
		struct pps_event_time pps_times;
	};
};


static inline long scaled_ppm_to_ppb(long ppm)
{
	
	s64 ppb = 1 + ppm;

	ppb *= 125;
	ppb >>= 13;
	return (long)ppb;
}


static inline bool diff_by_scaled_ppm(u64 base, long scaled_ppm, u64 *diff)
{
	bool negative = false;

	if (scaled_ppm < 0) {
		negative = true;
		scaled_ppm = -scaled_ppm;
	}

	*diff = mul_u64_u64_div_u64(base, (u64)scaled_ppm, 1000000ULL << 16);

	return negative;
}


static inline u64 adjust_by_scaled_ppm(u64 base, long scaled_ppm)
{
	u64 diff;

	if (diff_by_scaled_ppm(base, scaled_ppm, &diff))
		return base - diff;

	return base + diff;
}

#if IS_ENABLED(CONFIG_PTP_1588_CLOCK)



extern struct ptp_clock *ptp_clock_register(struct ptp_clock_info *info,
					    struct device *parent);



extern int ptp_clock_unregister(struct ptp_clock *ptp);



extern void ptp_clock_event(struct ptp_clock *ptp,
			    struct ptp_clock_event *event);



extern int ptp_clock_index(struct ptp_clock *ptp);



int ptp_find_pin(struct ptp_clock *ptp,
		 enum ptp_pin_function func, unsigned int chan);



int ptp_find_pin_unlocked(struct ptp_clock *ptp,
			  enum ptp_pin_function func, unsigned int chan);



int ptp_schedule_worker(struct ptp_clock *ptp, unsigned long delay);


void ptp_cancel_worker_sync(struct ptp_clock *ptp);

#else
static inline struct ptp_clock *ptp_clock_register(struct ptp_clock_info *info,
						   struct device *parent)
{ return NULL; }
static inline int ptp_clock_unregister(struct ptp_clock *ptp)
{ return 0; }
static inline void ptp_clock_event(struct ptp_clock *ptp,
				   struct ptp_clock_event *event)
{ }
static inline int ptp_clock_index(struct ptp_clock *ptp)
{ return -1; }
static inline int ptp_find_pin(struct ptp_clock *ptp,
			       enum ptp_pin_function func, unsigned int chan)
{ return -1; }
static inline int ptp_find_pin_unlocked(struct ptp_clock *ptp,
					enum ptp_pin_function func,
					unsigned int chan)
{ return -1; }
static inline int ptp_schedule_worker(struct ptp_clock *ptp,
				      unsigned long delay)
{ return -EOPNOTSUPP; }
static inline void ptp_cancel_worker_sync(struct ptp_clock *ptp)
{ }
#endif

#if IS_BUILTIN(CONFIG_PTP_1588_CLOCK)



int ptp_get_vclocks_index(int pclock_index, int **vclock_index);


ktime_t ptp_convert_timestamp(const ktime_t *hwtstamp, int vclock_index);
#else
static inline int ptp_get_vclocks_index(int pclock_index, int **vclock_index)
{ return 0; }
static inline ktime_t ptp_convert_timestamp(const ktime_t *hwtstamp,
					    int vclock_index)
{ return 0; }

#endif

static inline void ptp_read_system_prets(struct ptp_system_timestamp *sts)
{
	if (sts)
		ktime_get_real_ts64(&sts->pre_ts);
}

static inline void ptp_read_system_postts(struct ptp_system_timestamp *sts)
{
	if (sts)
		ktime_get_real_ts64(&sts->post_ts);
}

#endif
