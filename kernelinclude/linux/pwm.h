/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_PWM_H
#define __LINUX_PWM_H

#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/of.h>

struct pwm_chip;


enum pwm_polarity {
	PWM_POLARITY_NORMAL,
	PWM_POLARITY_INVERSED,
};


struct pwm_args {
	u64 period;
	enum pwm_polarity polarity;
};

enum {
	PWMF_REQUESTED = 0,
	PWMF_EXPORTED = 1,
};


struct pwm_state {
	u64 period;
	u64 duty_cycle;
	enum pwm_polarity polarity;
	bool enabled;
	bool usage_power;
};


struct pwm_device {
	const char *label;
	unsigned long flags;
	unsigned int hwpwm;
	unsigned int pwm;
	struct pwm_chip *chip;
	void *chip_data;

	struct pwm_args args;
	struct pwm_state state;
	struct pwm_state last;
};


static inline void pwm_get_state(const struct pwm_device *pwm,
				 struct pwm_state *state)
{
	*state = pwm->state;
}

static inline bool pwm_is_enabled(const struct pwm_device *pwm)
{
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return state.enabled;
}

static inline void pwm_set_period(struct pwm_device *pwm, u64 period)
{
	if (pwm)
		pwm->state.period = period;
}

static inline u64 pwm_get_period(const struct pwm_device *pwm)
{
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return state.period;
}

static inline void pwm_set_duty_cycle(struct pwm_device *pwm, unsigned int duty)
{
	if (pwm)
		pwm->state.duty_cycle = duty;
}

static inline u64 pwm_get_duty_cycle(const struct pwm_device *pwm)
{
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return state.duty_cycle;
}

static inline enum pwm_polarity pwm_get_polarity(const struct pwm_device *pwm)
{
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return state.polarity;
}

static inline void pwm_get_args(const struct pwm_device *pwm,
				struct pwm_args *args)
{
	*args = pwm->args;
}


static inline void pwm_init_state(const struct pwm_device *pwm,
				  struct pwm_state *state)
{
	struct pwm_args args;

	
	pwm_get_state(pwm, state);

	
	pwm_get_args(pwm, &args);

	state->period = args.period;
	state->polarity = args.polarity;
	state->duty_cycle = 0;
	state->usage_power = false;
}


static inline unsigned int
pwm_get_relative_duty_cycle(const struct pwm_state *state, unsigned int scale)
{
	if (!state->period)
		return 0;

	return DIV_ROUND_CLOSEST_ULL((u64)state->duty_cycle * scale,
				     state->period);
}


static inline int
pwm_set_relative_duty_cycle(struct pwm_state *state, unsigned int duty_cycle,
			    unsigned int scale)
{
	if (!scale || duty_cycle > scale)
		return -EINVAL;

	state->duty_cycle = DIV_ROUND_CLOSEST_ULL((u64)duty_cycle *
						  state->period,
						  scale);

	return 0;
}


struct pwm_capture {
	unsigned int period;
	unsigned int duty_cycle;
};


struct pwm_ops {
	int (*request)(struct pwm_chip *chip, struct pwm_device *pwm);
	void (*free)(struct pwm_chip *chip, struct pwm_device *pwm);
	int (*capture)(struct pwm_chip *chip, struct pwm_device *pwm,
		       struct pwm_capture *result, unsigned long timeout);
	int (*apply)(struct pwm_chip *chip, struct pwm_device *pwm,
		     const struct pwm_state *state);
	int (*get_state)(struct pwm_chip *chip, struct pwm_device *pwm,
			 struct pwm_state *state);
	struct module *owner;
};


struct pwm_chip {
	struct device *dev;
	const struct pwm_ops *ops;
	int base;
	unsigned int npwm;

	struct pwm_device * (*of_xlate)(struct pwm_chip *chip,
					const struct of_phandle_args *args);
	unsigned int of_pwm_n_cells;

	
	struct list_head list;
	struct pwm_device *pwms;
};

#if IS_ENABLED(CONFIG_PWM)

int pwm_apply_might_sleep(struct pwm_device *pwm, const struct pwm_state *state);
int pwm_adjust_config(struct pwm_device *pwm);


static inline int pwm_config(struct pwm_device *pwm, int duty_ns,
			     int period_ns)
{
	struct pwm_state state;

	if (!pwm)
		return -EINVAL;

	if (duty_ns < 0 || period_ns < 0)
		return -EINVAL;

	pwm_get_state(pwm, &state);
	if (state.duty_cycle == duty_ns && state.period == period_ns)
		return 0;

	state.duty_cycle = duty_ns;
	state.period = period_ns;
	return pwm_apply_might_sleep(pwm, &state);
}


static inline int pwm_enable(struct pwm_device *pwm)
{
	struct pwm_state state;

	if (!pwm)
		return -EINVAL;

	pwm_get_state(pwm, &state);
	if (state.enabled)
		return 0;

	state.enabled = true;
	return pwm_apply_might_sleep(pwm, &state);
}


static inline void pwm_disable(struct pwm_device *pwm)
{
	struct pwm_state state;

	if (!pwm)
		return;

	pwm_get_state(pwm, &state);
	if (!state.enabled)
		return;

	state.enabled = false;
	pwm_apply_might_sleep(pwm, &state);
}


int pwm_capture(struct pwm_device *pwm, struct pwm_capture *result,
		unsigned long timeout);
int pwm_set_chip_data(struct pwm_device *pwm, void *data);
void *pwm_get_chip_data(struct pwm_device *pwm);

int pwmchip_add(struct pwm_chip *chip);
void pwmchip_remove(struct pwm_chip *chip);

int devm_pwmchip_add(struct device *dev, struct pwm_chip *chip);

struct pwm_device *pwm_request_from_chip(struct pwm_chip *chip,
					 unsigned int index,
					 const char *label);

struct pwm_device *of_pwm_xlate_with_flags(struct pwm_chip *chip,
		const struct of_phandle_args *args);
struct pwm_device *of_pwm_single_xlate(struct pwm_chip *chip,
				       const struct of_phandle_args *args);

struct pwm_device *pwm_get(struct device *dev, const char *con_id);
void pwm_put(struct pwm_device *pwm);

struct pwm_device *devm_pwm_get(struct device *dev, const char *con_id);
struct pwm_device *devm_fwnode_pwm_get(struct device *dev,
				       struct fwnode_handle *fwnode,
				       const char *con_id);
#else
static inline int pwm_apply_might_sleep(struct pwm_device *pwm,
					const struct pwm_state *state)
{
	might_sleep();
	return -ENOTSUPP;
}

static inline int pwm_adjust_config(struct pwm_device *pwm)
{
	return -ENOTSUPP;
}

static inline int pwm_config(struct pwm_device *pwm, int duty_ns,
			     int period_ns)
{
	might_sleep();
	return -EINVAL;
}

static inline int pwm_enable(struct pwm_device *pwm)
{
	might_sleep();
	return -EINVAL;
}

static inline void pwm_disable(struct pwm_device *pwm)
{
	might_sleep();
}

static inline int pwm_capture(struct pwm_device *pwm,
			      struct pwm_capture *result,
			      unsigned long timeout)
{
	return -EINVAL;
}

static inline int pwm_set_chip_data(struct pwm_device *pwm, void *data)
{
	return -EINVAL;
}

static inline void *pwm_get_chip_data(struct pwm_device *pwm)
{
	return NULL;
}

static inline int pwmchip_add(struct pwm_chip *chip)
{
	return -EINVAL;
}

static inline int pwmchip_remove(struct pwm_chip *chip)
{
	return -EINVAL;
}

static inline int devm_pwmchip_add(struct device *dev, struct pwm_chip *chip)
{
	return -EINVAL;
}

static inline struct pwm_device *pwm_request_from_chip(struct pwm_chip *chip,
						       unsigned int index,
						       const char *label)
{
	might_sleep();
	return ERR_PTR(-ENODEV);
}

static inline struct pwm_device *pwm_get(struct device *dev,
					 const char *consumer)
{
	might_sleep();
	return ERR_PTR(-ENODEV);
}

static inline void pwm_put(struct pwm_device *pwm)
{
	might_sleep();
}

static inline struct pwm_device *devm_pwm_get(struct device *dev,
					      const char *consumer)
{
	might_sleep();
	return ERR_PTR(-ENODEV);
}

static inline struct pwm_device *
devm_fwnode_pwm_get(struct device *dev, struct fwnode_handle *fwnode,
		    const char *con_id)
{
	might_sleep();
	return ERR_PTR(-ENODEV);
}
#endif

static inline void pwm_apply_args(struct pwm_device *pwm)
{
	struct pwm_state state = { };

	

	state.enabled = false;
	state.polarity = pwm->args.polarity;
	state.period = pwm->args.period;
	state.usage_power = false;

	pwm_apply_might_sleep(pwm, &state);
}

struct pwm_lookup {
	struct list_head list;
	const char *provider;
	unsigned int index;
	const char *dev_id;
	const char *con_id;
	unsigned int period;
	enum pwm_polarity polarity;
	const char *module; 
};

#define PWM_LOOKUP_WITH_MODULE(_provider, _index, _dev_id, _con_id,	\
			       _period, _polarity, _module)		\
	{								\
		.provider = _provider,					\
		.index = _index,					\
		.dev_id = _dev_id,					\
		.con_id = _con_id,					\
		.period = _period,					\
		.polarity = _polarity,					\
		.module = _module,					\
	}

#define PWM_LOOKUP(_provider, _index, _dev_id, _con_id, _period, _polarity) \
	PWM_LOOKUP_WITH_MODULE(_provider, _index, _dev_id, _con_id, _period, \
			       _polarity, NULL)

#if IS_ENABLED(CONFIG_PWM)
void pwm_add_table(struct pwm_lookup *table, size_t num);
void pwm_remove_table(struct pwm_lookup *table, size_t num);
#else
static inline void pwm_add_table(struct pwm_lookup *table, size_t num)
{
}

static inline void pwm_remove_table(struct pwm_lookup *table, size_t num)
{
}
#endif

#ifdef CONFIG_PWM_SYSFS
void pwmchip_sysfs_export(struct pwm_chip *chip);
void pwmchip_sysfs_unexport(struct pwm_chip *chip);
#else
static inline void pwmchip_sysfs_export(struct pwm_chip *chip)
{
}

static inline void pwmchip_sysfs_unexport(struct pwm_chip *chip)
{
}
#endif 

#endif 
