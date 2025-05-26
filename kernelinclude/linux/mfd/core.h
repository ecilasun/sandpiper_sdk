/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef MFD_CORE_H
#define MFD_CORE_H

#include <linux/platform_device.h>

#define MFD_RES_SIZE(arr) (sizeof(arr) / sizeof(struct resource))

#define MFD_CELL_ALL(_name, _res, _pdata, _pdsize, _id, _compat, _of_reg, _use_of_reg, _match) \
	{								\
		.name = (_name),					\
		.resources = (_res),					\
		.num_resources = MFD_RES_SIZE((_res)),			\
		.platform_data = (_pdata),				\
		.pdata_size = (_pdsize),				\
		.of_compatible = (_compat),				\
		.of_reg = (_of_reg),					\
		.use_of_reg = (_use_of_reg),				\
		.acpi_match = (_match),					\
		.id = (_id),						\
	}

#define MFD_CELL_OF_REG(_name, _res, _pdata, _pdsize, _id, _compat, _of_reg) \
	MFD_CELL_ALL(_name, _res, _pdata, _pdsize, _id, _compat, _of_reg, true, NULL)

#define MFD_CELL_OF(_name, _res, _pdata, _pdsize, _id, _compat) \
	MFD_CELL_ALL(_name, _res, _pdata, _pdsize, _id, _compat, 0, false, NULL)

#define MFD_CELL_ACPI(_name, _res, _pdata, _pdsize, _id, _match) \
	MFD_CELL_ALL(_name, _res, _pdata, _pdsize, _id, NULL, 0, false, _match)

#define MFD_CELL_BASIC(_name, _res, _pdata, _pdsize, _id) \
	MFD_CELL_ALL(_name, _res, _pdata, _pdsize, _id, NULL, 0, false, NULL)

#define MFD_CELL_RES(_name, _res) \
	MFD_CELL_ALL(_name, _res, NULL, 0, 0, NULL, 0, false, NULL)

#define MFD_CELL_NAME(_name) \
	MFD_CELL_ALL(_name, NULL, NULL, 0, 0, NULL, 0, false, NULL)

#define MFD_DEP_LEVEL_NORMAL 0
#define MFD_DEP_LEVEL_HIGH 1

struct irq_domain;
struct software_node;


struct mfd_cell_acpi_match {
	const char			*pnpid;
	const unsigned long long	adr;
};


struct mfd_cell {
	const char		*name;
	int			id;
	int			level;

	int			(*suspend)(struct platform_device *dev);
	int			(*resume)(struct platform_device *dev);

	
	void			*platform_data;
	size_t			pdata_size;

	
	const struct mfd_cell_acpi_match	*acpi_match;

	
	const struct software_node *swnode;

	
	const char		*of_compatible;

	
	u64 of_reg;

	
	bool use_of_reg;

	
	int			num_resources;
	const struct resource	*resources;

	
	bool			ignore_resource_conflicts;

	
	bool			pm_runtime_no_callbacks;

	
	int			num_parent_supplies;
	const char * const	*parent_supplies;
};


static inline const struct mfd_cell *mfd_get_cell(struct platform_device *pdev)
{
	return pdev->mfd_cell;
}

extern int mfd_add_devices(struct device *parent, int id,
			   const struct mfd_cell *cells, int n_devs,
			   struct resource *mem_base,
			   int irq_base, struct irq_domain *irq_domain);

static inline int mfd_add_hotplug_devices(struct device *parent,
		const struct mfd_cell *cells, int n_devs)
{
	return mfd_add_devices(parent, PLATFORM_DEVID_AUTO, cells, n_devs,
			NULL, 0, NULL);
}

extern void mfd_remove_devices(struct device *parent);
extern void mfd_remove_devices_late(struct device *parent);

extern int devm_mfd_add_devices(struct device *dev, int id,
				const struct mfd_cell *cells, int n_devs,
				struct resource *mem_base,
				int irq_base, struct irq_domain *irq_domain);
#endif
