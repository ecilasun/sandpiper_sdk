/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GPIO_MACHINE_H
#define __LINUX_GPIO_MACHINE_H

#include <linux/types.h>

enum gpio_lookup_flags {
	GPIO_ACTIVE_HIGH		= (0 << 0),
	GPIO_ACTIVE_LOW			= (1 << 0),
	GPIO_OPEN_DRAIN			= (1 << 1),
	GPIO_OPEN_SOURCE		= (1 << 2),
	GPIO_PERSISTENT			= (0 << 3),
	GPIO_TRANSITORY			= (1 << 3),
	GPIO_PULL_UP			= (1 << 4),
	GPIO_PULL_DOWN			= (1 << 5),
	GPIO_PULL_DISABLE		= (1 << 6),

	GPIO_LOOKUP_FLAGS_DEFAULT	= GPIO_ACTIVE_HIGH | GPIO_PERSISTENT,
};


struct gpiod_lookup {
	const char *key;
	u16 chip_hwnum;
	const char *con_id;
	unsigned int idx;
	unsigned long flags;
};

struct gpiod_lookup_table {
	struct list_head list;
	const char *dev_id;
	struct gpiod_lookup table[];
};


struct gpiod_hog {
	struct list_head list;
	const char *chip_label;
	u16 chip_hwnum;
	const char *line_name;
	unsigned long lflags;
	int dflags;
};


#define GPIO_LOOKUP_SINGLE(_name, _dev_id, _key, _chip_hwnum, _con_id, _flags) \
static struct gpiod_lookup_table _name = {				\
	.dev_id = _dev_id,						\
	.table = {							\
		GPIO_LOOKUP(_key, _chip_hwnum, _con_id, _flags),	\
		{},							\
	},								\
}


#define GPIO_LOOKUP(_key, _chip_hwnum, _con_id, _flags) \
	GPIO_LOOKUP_IDX(_key, _chip_hwnum, _con_id, 0, _flags)


#define GPIO_LOOKUP_IDX(_key, _chip_hwnum, _con_id, _idx, _flags)         \
(struct gpiod_lookup) {                                                   \
	.key = _key,                                                      \
	.chip_hwnum = _chip_hwnum,                                        \
	.con_id = _con_id,                                                \
	.idx = _idx,                                                      \
	.flags = _flags,                                                  \
}


#define GPIO_HOG(_chip_label, _chip_hwnum, _line_name, _lflags, _dflags)  \
(struct gpiod_hog) {                                                      \
	.chip_label = _chip_label,                                        \
	.chip_hwnum = _chip_hwnum,                                        \
	.line_name = _line_name,                                          \
	.lflags = _lflags,                                                \
	.dflags = _dflags,                                                \
}

#ifdef CONFIG_GPIOLIB
void gpiod_add_lookup_table(struct gpiod_lookup_table *table);
void gpiod_add_lookup_tables(struct gpiod_lookup_table **tables, size_t n);
void gpiod_remove_lookup_table(struct gpiod_lookup_table *table);
void gpiod_add_hogs(struct gpiod_hog *hogs);
void gpiod_remove_hogs(struct gpiod_hog *hogs);
#else 
static inline
void gpiod_add_lookup_table(struct gpiod_lookup_table *table) {}
static inline
void gpiod_add_lookup_tables(struct gpiod_lookup_table **tables, size_t n) {}
static inline
void gpiod_remove_lookup_table(struct gpiod_lookup_table *table) {}
static inline void gpiod_add_hogs(struct gpiod_hog *hogs) {}
static inline void gpiod_remove_hogs(struct gpiod_hog *hogs) {}
#endif 

#endif 
