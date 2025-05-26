/* SPDX-License-Identifier: GPL-2.0-only */


#include <linux/device.h>
#include <linux/err.h>

#ifndef __OCMEM_H__
#define __OCMEM_H__

enum ocmem_client {
	
	OCMEM_GRAPHICS = 0x0,
	
	OCMEM_CLIENT_MAX,
};

struct ocmem;

struct ocmem_buf {
	unsigned long offset;
	unsigned long addr;
	unsigned long len;
};

#if IS_ENABLED(CONFIG_QCOM_OCMEM)

struct ocmem *of_get_ocmem(struct device *dev);
struct ocmem_buf *ocmem_allocate(struct ocmem *ocmem, enum ocmem_client client,
				 unsigned long size);
void ocmem_free(struct ocmem *ocmem, enum ocmem_client client,
		struct ocmem_buf *buf);

#else 

static inline struct ocmem *of_get_ocmem(struct device *dev)
{
	return ERR_PTR(-ENODEV);
}

static inline struct ocmem_buf *ocmem_allocate(struct ocmem *ocmem,
					       enum ocmem_client client,
					       unsigned long size)
{
	return ERR_PTR(-ENODEV);
}

static inline void ocmem_free(struct ocmem *ocmem, enum ocmem_client client,
			      struct ocmem_buf *buf)
{
}

#endif 

#endif 
