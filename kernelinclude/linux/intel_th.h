/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _INTEL_TH_H_
#define _INTEL_TH_H_

#include <linux/scatterlist.h>


enum {
	MSC_MODE_SINGLE	= 0,
	MSC_MODE_MULTI,
	MSC_MODE_EXI,
	MSC_MODE_DEBUG,
};

struct msu_buffer {
	const char	*name;
	
	void	*(*assign)(struct device *dev, int *mode);
	
	void	(*unassign)(void *priv);
	
	int	(*alloc_window)(void *priv, struct sg_table **sgt,
				size_t size);
	void	(*free_window)(void *priv, struct sg_table *sgt);
	
	void	(*activate)(void *priv);
	
	void	(*deactivate)(void *priv);
	
	int	(*ready)(void *priv, struct sg_table *sgt, size_t bytes);
};

int intel_th_msu_buffer_register(const struct msu_buffer *mbuf,
				 struct module *owner);
void intel_th_msu_buffer_unregister(const struct msu_buffer *mbuf);
void intel_th_msc_window_unlock(struct device *dev, struct sg_table *sgt);

#define module_intel_th_msu_buffer(__buffer) \
static int __init __buffer##_init(void) \
{ \
	return intel_th_msu_buffer_register(&(__buffer), THIS_MODULE); \
} \
module_init(__buffer##_init); \
static void __exit __buffer##_exit(void) \
{ \
	intel_th_msu_buffer_unregister(&(__buffer)); \
} \
module_exit(__buffer##_exit);

#endif 
