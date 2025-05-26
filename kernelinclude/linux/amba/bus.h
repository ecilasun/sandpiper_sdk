/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ASMARM_AMBA_H
#define ASMARM_AMBA_H

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/err.h>
#include <linux/resource.h>
#include <linux/regulator/consumer.h>

#define AMBA_NR_IRQS	9
#define AMBA_CID	0xb105f00d
#define CORESIGHT_CID	0xb105900d




struct amba_cs_uci_id {
	unsigned int devarch;
	unsigned int devarch_mask;
	unsigned int devtype;
	void *data;
};


#define UCI_REG_DEVTYPE_OFFSET	0xFCC
#define UCI_REG_DEVARCH_OFFSET	0xFBC

struct clk;

struct amba_device {
	struct device		dev;
	struct resource		res;
	struct clk		*pclk;
	struct device_dma_parameters dma_parms;
	unsigned int		periphid;
	struct mutex		periphid_lock;
	unsigned int		cid;
	struct amba_cs_uci_id	uci;
	unsigned int		irq[AMBA_NR_IRQS];
	
	const char		*driver_override;
};

struct amba_driver {
	struct device_driver	drv;
	int			(*probe)(struct amba_device *, const struct amba_id *);
	void			(*remove)(struct amba_device *);
	void			(*shutdown)(struct amba_device *);
	const struct amba_id	*id_table;
	
	bool driver_managed_dma;
};


enum amba_vendor {
	AMBA_VENDOR_ARM = 0x41,
	AMBA_VENDOR_ST = 0x80,
	AMBA_VENDOR_QCOM = 0x51,
	AMBA_VENDOR_LSI = 0xb6,
};

extern struct bus_type amba_bustype;

#define to_amba_device(d)	container_of_const(d, struct amba_device, dev)

#define amba_get_drvdata(d)	dev_get_drvdata(&d->dev)
#define amba_set_drvdata(d,p)	dev_set_drvdata(&d->dev, p)

#ifdef CONFIG_ARM_AMBA
int amba_driver_register(struct amba_driver *);
void amba_driver_unregister(struct amba_driver *);
#else
static inline int amba_driver_register(struct amba_driver *drv)
{
	return -EINVAL;
}
static inline void amba_driver_unregister(struct amba_driver *drv)
{
}
#endif

struct amba_device *amba_device_alloc(const char *, resource_size_t, size_t);
void amba_device_put(struct amba_device *);
int amba_device_add(struct amba_device *, struct resource *);
int amba_device_register(struct amba_device *, struct resource *);
void amba_device_unregister(struct amba_device *);
int amba_request_regions(struct amba_device *, const char *);
void amba_release_regions(struct amba_device *);


#define AMBA_CONFIG_BITS(a) (((a) >> 24) & 0xff)
#define AMBA_REV_BITS(a) (((a) >> 20) & 0x0f)
#define AMBA_MANF_BITS(a) (((a) >> 12) & 0xff)
#define AMBA_PART_BITS(a) ((a) & 0xfff)

#define amba_config(d)	AMBA_CONFIG_BITS((d)->periphid)
#define amba_rev(d)	AMBA_REV_BITS((d)->periphid)
#define amba_manf(d)	AMBA_MANF_BITS((d)->periphid)
#define amba_part(d)	AMBA_PART_BITS((d)->periphid)

#define __AMBA_DEV(busid, data, mask)				\
	{							\
		.coherent_dma_mask = mask,			\
		.init_name = busid,				\
		.platform_data = data,				\
	}


#define AMBA_APB_DEVICE(name, busid, id, base, irqs, data)	\
struct amba_device name##_device = {				\
	.dev = __AMBA_DEV(busid, data, 0),			\
	.res = DEFINE_RES_MEM(base, SZ_4K),			\
	.irq = irqs,						\
	.periphid = id,						\
}


#define AMBA_AHB_DEVICE(name, busid, id, base, irqs, data)	\
struct amba_device name##_device = {				\
	.dev = __AMBA_DEV(busid, data, ~0ULL),			\
	.res = DEFINE_RES_MEM(base, SZ_4K),			\
	.irq = irqs,						\
	.periphid = id,						\
}


#define module_amba_driver(__amba_drv) \
	module_driver(__amba_drv, amba_driver_register, amba_driver_unregister)


#define builtin_amba_driver(__amba_drv) \
	builtin_driver(__amba_drv, amba_driver_register)

#endif
