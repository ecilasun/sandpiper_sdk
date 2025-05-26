/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _PMBUS_H_
#define _PMBUS_H_

#include <linux/bits.h>




#define PMBUS_SKIP_STATUS_CHECK	BIT(0)


#define PMBUS_WRITE_PROTECTED	BIT(1)


#define PMBUS_NO_CAPABILITY			BIT(2)


#define PMBUS_READ_STATUS_AFTER_FAILED_CHECK	BIT(3)


#define PMBUS_NO_WRITE_PROTECT			BIT(4)


#define PMBUS_USE_COEFFICIENTS_CMD		BIT(5)

struct pmbus_platform_data {
	u32 flags;		

	
	int num_regulators;
	struct regulator_init_data *reg_init_data;
};

#endif 
