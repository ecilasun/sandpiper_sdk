/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_BOOTMEM_INFO_H
#define __LINUX_BOOTMEM_INFO_H

#include <linux/mm.h>
#include <linux/kmemleak.h>


enum {
	MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE = 12,
	SECTION_INFO = MEMORY_HOTPLUG_MIN_BOOTMEM_TYPE,
	MIX_SECTION_INFO,
	NODE_INFO,
	MEMORY_HOTPLUG_MAX_BOOTMEM_TYPE = NODE_INFO,
};

#ifdef CONFIG_HAVE_BOOTMEM_INFO_NODE
void __init register_page_bootmem_info_node(struct pglist_data *pgdat);

void get_page_bootmem(unsigned long info, struct page *page,
		      unsigned long type);
void put_page_bootmem(struct page *page);


static inline void free_bootmem_page(struct page *page)
{
	unsigned long magic = page->index;

	
	VM_BUG_ON_PAGE(page_ref_count(page) != 2, page);

	if (magic == SECTION_INFO || magic == MIX_SECTION_INFO)
		put_page_bootmem(page);
	else
		VM_BUG_ON_PAGE(1, page);
}
#else
static inline void register_page_bootmem_info_node(struct pglist_data *pgdat)
{
}

static inline void put_page_bootmem(struct page *page)
{
}

static inline void get_page_bootmem(unsigned long info, struct page *page,
				    unsigned long type)
{
}

static inline void free_bootmem_page(struct page *page)
{
	kmemleak_free_part(page_to_virt(page), PAGE_SIZE);
	free_reserved_page(page);
}
#endif

#endif 
