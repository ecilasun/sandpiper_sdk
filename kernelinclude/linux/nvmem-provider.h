/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_NVMEM_PROVIDER_H
#define _LINUX_NVMEM_PROVIDER_H

#include <linux/device/driver.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/gpio/consumer.h>

struct nvmem_device;
typedef int (*nvmem_reg_read_t)(void *priv, unsigned int offset,
				void *val, size_t bytes);
typedef int (*nvmem_reg_write_t)(void *priv, unsigned int offset,
				 void *val, size_t bytes);

typedef int (*nvmem_cell_post_process_t)(void *priv, const char *id, int index,
					 unsigned int offset, void *buf,
					 size_t bytes);

enum nvmem_type {
	NVMEM_TYPE_UNKNOWN = 0,
	NVMEM_TYPE_EEPROM,
	NVMEM_TYPE_OTP,
	NVMEM_TYPE_BATTERY_BACKED,
	NVMEM_TYPE_FRAM,
};

#define NVMEM_DEVID_NONE	(-1)
#define NVMEM_DEVID_AUTO	(-2)


struct nvmem_keepout {
	unsigned int start;
	unsigned int end;
	unsigned char value;
};


struct nvmem_cell_info {
	const char		*name;
	unsigned int		offset;
	size_t			raw_len;
	unsigned int		bytes;
	unsigned int		bit_offset;
	unsigned int		nbits;
	struct device_node	*np;
	nvmem_cell_post_process_t read_post_process;
	void			*priv;
};


struct nvmem_config {
	struct device		*dev;
	const char		*name;
	int			id;
	struct module		*owner;
	const struct nvmem_cell_info	*cells;
	int			ncells;
	bool			add_legacy_fixed_of_cells;
	const struct nvmem_keepout *keepout;
	unsigned int		nkeepout;
	enum nvmem_type		type;
	bool			read_only;
	bool			root_only;
	bool			ignore_wp;
	struct nvmem_layout	*layout;
	struct device_node	*of_node;
	bool			no_of_node;
	nvmem_reg_read_t	reg_read;
	nvmem_reg_write_t	reg_write;
	int	size;
	int	word_size;
	int	stride;
	void	*priv;
	
	bool			compat;
	struct device		*base_dev;
};


struct nvmem_cell_table {
	const char		*nvmem_name;
	const struct nvmem_cell_info	*cells;
	size_t			ncells;
	struct list_head	node;
};


struct nvmem_layout {
	const char *name;
	const struct of_device_id *of_match_table;
	int (*add_cells)(struct device *dev, struct nvmem_device *nvmem,
			 struct nvmem_layout *layout);
	void (*fixup_cell_info)(struct nvmem_device *nvmem,
				struct nvmem_layout *layout,
				struct nvmem_cell_info *cell);

	
	struct module *owner;
	struct list_head node;
};

#if IS_ENABLED(CONFIG_NVMEM)

struct nvmem_device *nvmem_register(const struct nvmem_config *cfg);
void nvmem_unregister(struct nvmem_device *nvmem);

struct nvmem_device *devm_nvmem_register(struct device *dev,
					 const struct nvmem_config *cfg);

void nvmem_add_cell_table(struct nvmem_cell_table *table);
void nvmem_del_cell_table(struct nvmem_cell_table *table);

int nvmem_add_one_cell(struct nvmem_device *nvmem,
		       const struct nvmem_cell_info *info);

int __nvmem_layout_register(struct nvmem_layout *layout, struct module *owner);
#define nvmem_layout_register(layout) \
	__nvmem_layout_register(layout, THIS_MODULE)
void nvmem_layout_unregister(struct nvmem_layout *layout);

const void *nvmem_layout_get_match_data(struct nvmem_device *nvmem,
					struct nvmem_layout *layout);

#else

static inline struct nvmem_device *nvmem_register(const struct nvmem_config *c)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline void nvmem_unregister(struct nvmem_device *nvmem) {}

static inline struct nvmem_device *
devm_nvmem_register(struct device *dev, const struct nvmem_config *c)
{
	return nvmem_register(c);
}

static inline void nvmem_add_cell_table(struct nvmem_cell_table *table) {}
static inline void nvmem_del_cell_table(struct nvmem_cell_table *table) {}
static inline int nvmem_add_one_cell(struct nvmem_device *nvmem,
				     const struct nvmem_cell_info *info)
{
	return -EOPNOTSUPP;
}

static inline int nvmem_layout_register(struct nvmem_layout *layout)
{
	return -EOPNOTSUPP;
}

static inline void nvmem_layout_unregister(struct nvmem_layout *layout) {}

static inline const void *
nvmem_layout_get_match_data(struct nvmem_device *nvmem,
			    struct nvmem_layout *layout)
{
	return NULL;
}

#endif 

#define module_nvmem_layout_driver(__layout_driver)		\
	module_driver(__layout_driver, nvmem_layout_register,	\
		      nvmem_layout_unregister)

#endif  
