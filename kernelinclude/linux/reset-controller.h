/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RESET_CONTROLLER_H_
#define _LINUX_RESET_CONTROLLER_H_

#include <linux/list.h>

struct reset_controller_dev;


struct reset_control_ops {
	int (*reset)(struct reset_controller_dev *rcdev, unsigned long id);
	int (*assert)(struct reset_controller_dev *rcdev, unsigned long id);
	int (*deassert)(struct reset_controller_dev *rcdev, unsigned long id);
	int (*status)(struct reset_controller_dev *rcdev, unsigned long id);
};

struct module;
struct device_node;
struct of_phandle_args;


struct reset_control_lookup {
	struct list_head list;
	const char *provider;
	unsigned int index;
	const char *dev_id;
	const char *con_id;
};

#define RESET_LOOKUP(_provider, _index, _dev_id, _con_id)		\
	{								\
		.provider = _provider,					\
		.index = _index,					\
		.dev_id = _dev_id,					\
		.con_id = _con_id,					\
	}


struct reset_controller_dev {
	const struct reset_control_ops *ops;
	struct module *owner;
	struct list_head list;
	struct list_head reset_control_head;
	struct device *dev;
	struct device_node *of_node;
	int of_reset_n_cells;
	int (*of_xlate)(struct reset_controller_dev *rcdev,
			const struct of_phandle_args *reset_spec);
	unsigned int nr_resets;
};

#if IS_ENABLED(CONFIG_RESET_CONTROLLER)
int reset_controller_register(struct reset_controller_dev *rcdev);
void reset_controller_unregister(struct reset_controller_dev *rcdev);

struct device;
int devm_reset_controller_register(struct device *dev,
				   struct reset_controller_dev *rcdev);

void reset_controller_add_lookup(struct reset_control_lookup *lookup,
				 unsigned int num_entries);
#else
static inline int reset_controller_register(struct reset_controller_dev *rcdev)
{
	return 0;
}

static inline void reset_controller_unregister(struct reset_controller_dev *rcdev)
{
}

static inline int devm_reset_controller_register(struct device *dev,
						 struct reset_controller_dev *rcdev)
{
	return 0;
}

static inline void reset_controller_add_lookup(struct reset_control_lookup *lookup,
					       unsigned int num_entries)
{
}
#endif

#endif
