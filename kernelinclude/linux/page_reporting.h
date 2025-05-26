/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PAGE_REPORTING_H
#define _LINUX_PAGE_REPORTING_H

#include <linux/mmzone.h>
#include <linux/scatterlist.h>


#define PAGE_REPORTING_CAPACITY		32

struct page_reporting_dev_info {
	
	int (*report)(struct page_reporting_dev_info *prdev,
		      struct scatterlist *sg, unsigned int nents);

	
	struct delayed_work work;

	
	atomic_t state;

	
	unsigned int order;
};


void page_reporting_unregister(struct page_reporting_dev_info *prdev);
int page_reporting_register(struct page_reporting_dev_info *prdev);
#endif 
