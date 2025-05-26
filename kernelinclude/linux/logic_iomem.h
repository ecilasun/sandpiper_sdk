/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LOGIC_IOMEM_H
#define __LOGIC_IOMEM_H
#include <linux/types.h>
#include <linux/ioport.h>


struct logic_iomem_ops {
	unsigned long (*read)(void *priv, unsigned int offset, int size);
	void (*write)(void *priv, unsigned int offset, int size,
		      unsigned long val);

	void (*set)(void *priv, unsigned int offset, u8 value, int size);
	void (*copy_from)(void *priv, void *buffer, unsigned int offset,
			  int size);
	void (*copy_to)(void *priv, unsigned int offset, const void *buffer,
			int size);

	void (*unmap)(void *priv);
};


struct logic_iomem_region_ops {
	long (*map)(unsigned long offset, size_t size,
		    const struct logic_iomem_ops **ops,
		    void **priv);
};


int logic_iomem_add_region(struct resource *resource,
			   const struct logic_iomem_region_ops *ops);

#endif 
