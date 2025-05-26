/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __REGULATOR_FIXED_H
#define __REGULATOR_FIXED_H

struct regulator_init_data;


struct fixed_voltage_config {
	const char *supply_name;
	const char *input_supply;
	int microvolts;
	unsigned startup_delay;
	unsigned int off_on_delay;
	unsigned enabled_at_boot:1;
	struct regulator_init_data *init_data;
};

struct regulator_consumer_supply;

#if IS_ENABLED(CONFIG_REGULATOR)
struct platform_device *regulator_register_always_on(int id, const char *name,
		struct regulator_consumer_supply *supplies, int num_supplies, int uv);
#else
static inline struct platform_device *regulator_register_always_on(int id, const char *name,
		struct regulator_consumer_supply *supplies, int num_supplies, int uv)
{
	return NULL;
}
#endif

#define regulator_register_fixed(id, s, ns) regulator_register_always_on(id, \
						"fixed-dummy", s, ns, 0)

#endif
