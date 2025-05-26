/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_RTC_H_
#define _LINUX_RTC_H_


#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/nvmem-provider.h>
#include <uapi/linux/rtc.h>

extern int rtc_month_days(unsigned int month, unsigned int year);
extern int rtc_year_days(unsigned int day, unsigned int month, unsigned int year);
extern int rtc_valid_tm(struct rtc_time *tm);
extern time64_t rtc_tm_to_time64(struct rtc_time *tm);
extern void rtc_time64_to_tm(time64_t time, struct rtc_time *tm);
ktime_t rtc_tm_to_ktime(struct rtc_time tm);
struct rtc_time rtc_ktime_to_tm(ktime_t kt);


static inline time64_t rtc_tm_sub(struct rtc_time *lhs, struct rtc_time *rhs)
{
	return rtc_tm_to_time64(lhs) - rtc_tm_to_time64(rhs);
}

#include <linux/device.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/mutex.h>
#include <linux/timerqueue.h>
#include <linux/workqueue.h>

extern struct class *rtc_class;


struct rtc_class_ops {
	int (*ioctl)(struct device *, unsigned int, unsigned long);
	int (*read_time)(struct device *, struct rtc_time *);
	int (*set_time)(struct device *, struct rtc_time *);
	int (*read_alarm)(struct device *, struct rtc_wkalrm *);
	int (*set_alarm)(struct device *, struct rtc_wkalrm *);
	int (*proc)(struct device *, struct seq_file *);
	int (*alarm_irq_enable)(struct device *, unsigned int enabled);
	int (*read_offset)(struct device *, long *offset);
	int (*set_offset)(struct device *, long offset);
	int (*param_get)(struct device *, struct rtc_param *param);
	int (*param_set)(struct device *, struct rtc_param *param);
};

struct rtc_device;

struct rtc_timer {
	struct timerqueue_node node;
	ktime_t period;
	void (*func)(struct rtc_device *rtc);
	struct rtc_device *rtc;
	int enabled;
};


#define RTC_DEV_BUSY 0
#define RTC_NO_CDEV  1

struct rtc_device {
	struct device dev;
	struct module *owner;

	int id;

	const struct rtc_class_ops *ops;
	struct mutex ops_lock;

	struct cdev char_dev;
	unsigned long flags;

	unsigned long irq_data;
	spinlock_t irq_lock;
	wait_queue_head_t irq_queue;
	struct fasync_struct *async_queue;

	int irq_freq;
	int max_user_freq;

	struct timerqueue_head timerqueue;
	struct rtc_timer aie_timer;
	struct rtc_timer uie_rtctimer;
	struct hrtimer pie_timer; 
	int pie_enabled;
	struct work_struct irqwork;

	
	unsigned long set_offset_nsec;

	unsigned long features[BITS_TO_LONGS(RTC_FEATURE_CNT)];

	time64_t range_min;
	timeu64_t range_max;
	timeu64_t alarm_offset_max;
	time64_t start_secs;
	time64_t offset_secs;
	bool set_start_time;

#ifdef CONFIG_RTC_INTF_DEV_UIE_EMUL
	struct work_struct uie_task;
	struct timer_list uie_timer;
	
	unsigned int oldsecs;
	unsigned int uie_irq_active:1;
	unsigned int stop_uie_polling:1;
	unsigned int uie_task_active:1;
	unsigned int uie_timer_active:1;
#endif
};
#define to_rtc_device(d) container_of(d, struct rtc_device, dev)

#define rtc_lock(d) mutex_lock(&d->ops_lock)
#define rtc_unlock(d) mutex_unlock(&d->ops_lock)


#define RTC_TIMESTAMP_BEGIN_0000	-62167219200ULL 
#define RTC_TIMESTAMP_BEGIN_1900	-2208988800LL 
#define RTC_TIMESTAMP_BEGIN_2000	946684800LL 
#define RTC_TIMESTAMP_END_2063		2966371199LL 
#define RTC_TIMESTAMP_END_2079		3471292799LL 
#define RTC_TIMESTAMP_END_2099		4102444799LL 
#define RTC_TIMESTAMP_END_2199		7258118399LL 
#define RTC_TIMESTAMP_END_9999		253402300799LL 

extern struct rtc_device *devm_rtc_device_register(struct device *dev,
					const char *name,
					const struct rtc_class_ops *ops,
					struct module *owner);
struct rtc_device *devm_rtc_allocate_device(struct device *dev);
int __devm_rtc_register_device(struct module *owner, struct rtc_device *rtc);

extern int rtc_read_time(struct rtc_device *rtc, struct rtc_time *tm);
extern int rtc_set_time(struct rtc_device *rtc, struct rtc_time *tm);
int __rtc_read_alarm(struct rtc_device *rtc, struct rtc_wkalrm *alarm);
extern int rtc_read_alarm(struct rtc_device *rtc,
			struct rtc_wkalrm *alrm);
extern int rtc_set_alarm(struct rtc_device *rtc,
				struct rtc_wkalrm *alrm);
extern int rtc_initialize_alarm(struct rtc_device *rtc,
				struct rtc_wkalrm *alrm);
extern void rtc_update_irq(struct rtc_device *rtc,
			unsigned long num, unsigned long events);

extern struct rtc_device *rtc_class_open(const char *name);
extern void rtc_class_close(struct rtc_device *rtc);

extern int rtc_irq_set_state(struct rtc_device *rtc, int enabled);
extern int rtc_irq_set_freq(struct rtc_device *rtc, int freq);
extern int rtc_update_irq_enable(struct rtc_device *rtc, unsigned int enabled);
extern int rtc_alarm_irq_enable(struct rtc_device *rtc, unsigned int enabled);
extern int rtc_dev_update_irq_enable_emul(struct rtc_device *rtc,
						unsigned int enabled);

void rtc_handle_legacy_irq(struct rtc_device *rtc, int num, int mode);
void rtc_aie_update_irq(struct rtc_device *rtc);
void rtc_uie_update_irq(struct rtc_device *rtc);
enum hrtimer_restart rtc_pie_update_irq(struct hrtimer *timer);

void rtc_timer_init(struct rtc_timer *timer, void (*f)(struct rtc_device *r),
		    struct rtc_device *rtc);
int rtc_timer_start(struct rtc_device *rtc, struct rtc_timer *timer,
		    ktime_t expires, ktime_t period);
void rtc_timer_cancel(struct rtc_device *rtc, struct rtc_timer *timer);
int rtc_read_offset(struct rtc_device *rtc, long *offset);
int rtc_set_offset(struct rtc_device *rtc, long offset);
void rtc_timer_do_work(struct work_struct *work);

static inline bool is_leap_year(unsigned int year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

#define devm_rtc_register_device(device) \
	__devm_rtc_register_device(THIS_MODULE, device)

#ifdef CONFIG_RTC_HCTOSYS_DEVICE
extern int rtc_hctosys_ret;
#else
#define rtc_hctosys_ret -ENODEV
#endif

#ifdef CONFIG_RTC_NVMEM
int devm_rtc_nvmem_register(struct rtc_device *rtc,
			    struct nvmem_config *nvmem_config);
#else
static inline int devm_rtc_nvmem_register(struct rtc_device *rtc,
					  struct nvmem_config *nvmem_config)
{
	return 0;
}
#endif

#ifdef CONFIG_RTC_INTF_SYSFS
int rtc_add_group(struct rtc_device *rtc, const struct attribute_group *grp);
int rtc_add_groups(struct rtc_device *rtc, const struct attribute_group **grps);
#else
static inline
int rtc_add_group(struct rtc_device *rtc, const struct attribute_group *grp)
{
	return 0;
}

static inline
int rtc_add_groups(struct rtc_device *rtc, const struct attribute_group **grps)
{
	return 0;
}
#endif
#endif 
