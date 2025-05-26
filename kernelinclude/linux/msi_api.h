/* SPDX-License-Identifier: GPL-2.0 */
#ifndef LINUX_MSI_API_H
#define LINUX_MSI_API_H



struct device;


enum msi_domain_ids {
	MSI_DEFAULT_DOMAIN,
	MSI_SECONDARY_DOMAIN,
	MSI_MAX_DEVICE_IRQDOMAINS,
};


union msi_instance_cookie {
	u64	value;
	void	*ptr;
};


struct msi_map {
	int	index;
	int	virq;
};


#define MSI_ANY_INDEX		UINT_MAX

unsigned int msi_domain_get_virq(struct device *dev, unsigned int domid, unsigned int index);


static inline unsigned int msi_get_virq(struct device *dev, unsigned int index)
{
	return msi_domain_get_virq(dev, MSI_DEFAULT_DOMAIN, index);
}

#endif
