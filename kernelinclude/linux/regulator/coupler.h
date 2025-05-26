/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_REGULATOR_COUPLER_H_
#define __LINUX_REGULATOR_COUPLER_H_

#include <linux/kernel.h>
#include <linux/suspend.h>

struct regulator_coupler;
struct regulator_dev;


struct regulator_coupler {
	struct list_head list;

	int (*attach_regulator)(struct regulator_coupler *coupler,
				struct regulator_dev *rdev);
	int (*detach_regulator)(struct regulator_coupler *coupler,
				struct regulator_dev *rdev);
	int (*balance_voltage)(struct regulator_coupler *coupler,
			       struct regulator_dev *rdev,
			       suspend_state_t state);
};

#ifdef CONFIG_REGULATOR
int regulator_coupler_register(struct regulator_coupler *coupler);
int regulator_check_consumers(struct regulator_dev *rdev,
			      int *min_uV, int *max_uV,
			      suspend_state_t state);
int regulator_check_voltage(struct regulator_dev *rdev,
			    int *min_uV, int *max_uV);
int regulator_get_voltage_rdev(struct regulator_dev *rdev);
int regulator_set_voltage_rdev(struct regulator_dev *rdev,
			       int min_uV, int max_uV,
			       suspend_state_t state);
int regulator_do_balance_voltage(struct regulator_dev *rdev,
				 suspend_state_t state, bool skip_coupled);
#else
static inline int regulator_coupler_register(struct regulator_coupler *coupler)
{
	return 0;
}
static inline int regulator_check_consumers(struct regulator_dev *rdev,
					    int *min_uV, int *max_uV,
					    suspend_state_t state)
{
	return -EINVAL;
}
static inline int regulator_check_voltage(struct regulator_dev *rdev,
					  int *min_uV, int *max_uV)
{
	return -EINVAL;
}
static inline int regulator_get_voltage_rdev(struct regulator_dev *rdev)
{
	return -EINVAL;
}
static inline int regulator_set_voltage_rdev(struct regulator_dev *rdev,
					     int min_uV, int max_uV,
					     suspend_state_t state)
{
	return -EINVAL;
}
static inline int regulator_do_balance_voltage(struct regulator_dev *rdev,
					       suspend_state_t state,
					       bool skip_coupled)
{
	return -EINVAL;
}
#endif

#endif
