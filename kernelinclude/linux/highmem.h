/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_HIGHMEM_H
#define _LINUX_HIGHMEM_H

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/cacheflush.h>
#include <linux/kmsan.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/hardirq.h>

#include "highmem-internal.h"


static inline void *kmap(struct page *page);


static inline void kunmap(struct page *page);


static inline struct page *kmap_to_page(void *addr);


static inline void kmap_flush_unused(void);


static inline void *kmap_local_page(struct page *page);


static inline void *kmap_local_folio(struct folio *folio, size_t offset);


static inline void *kmap_atomic(struct page *page);


static inline unsigned int nr_free_highpages(void);
static inline unsigned long totalhigh_pages(void);

#ifndef ARCH_HAS_FLUSH_ANON_PAGE
static inline void flush_anon_page(struct vm_area_struct *vma, struct page *page, unsigned long vmaddr)
{
}
#endif

#ifndef ARCH_IMPLEMENTS_FLUSH_KERNEL_VMAP_RANGE
static inline void flush_kernel_vmap_range(void *vaddr, int size)
{
}
static inline void invalidate_kernel_vmap_range(void *vaddr, int size)
{
}
#endif


#ifndef clear_user_highpage
static inline void clear_user_highpage(struct page *page, unsigned long vaddr)
{
	void *addr = kmap_local_page(page);
	clear_user_page(addr, vaddr, page);
	kunmap_local(addr);
}
#endif

#ifndef vma_alloc_zeroed_movable_folio

static inline
struct folio *vma_alloc_zeroed_movable_folio(struct vm_area_struct *vma,
				   unsigned long vaddr)
{
	struct folio *folio;

	folio = vma_alloc_folio(GFP_HIGHUSER_MOVABLE, 0, vma, vaddr, false);
	if (folio)
		clear_user_highpage(&folio->page, vaddr);

	return folio;
}
#endif

static inline void clear_highpage(struct page *page)
{
	void *kaddr = kmap_local_page(page);
	clear_page(kaddr);
	kunmap_local(kaddr);
}

static inline void clear_highpage_kasan_tagged(struct page *page)
{
	void *kaddr = kmap_local_page(page);

	clear_page(kasan_reset_tag(kaddr));
	kunmap_local(kaddr);
}

#ifndef __HAVE_ARCH_TAG_CLEAR_HIGHPAGE

static inline void tag_clear_highpage(struct page *page)
{
}

#endif


#ifdef CONFIG_HIGHMEM
void zero_user_segments(struct page *page, unsigned start1, unsigned end1,
		unsigned start2, unsigned end2);
#else
static inline void zero_user_segments(struct page *page,
		unsigned start1, unsigned end1,
		unsigned start2, unsigned end2)
{
	void *kaddr = kmap_local_page(page);
	unsigned int i;

	BUG_ON(end1 > page_size(page) || end2 > page_size(page));

	if (end1 > start1)
		memset(kaddr + start1, 0, end1 - start1);

	if (end2 > start2)
		memset(kaddr + start2, 0, end2 - start2);

	kunmap_local(kaddr);
	for (i = 0; i < compound_nr(page); i++)
		flush_dcache_page(page + i);
}
#endif

static inline void zero_user_segment(struct page *page,
	unsigned start, unsigned end)
{
	zero_user_segments(page, start, end, 0, 0);
}

static inline void zero_user(struct page *page,
	unsigned start, unsigned size)
{
	zero_user_segments(page, start, start + size, 0, 0);
}

#ifndef __HAVE_ARCH_COPY_USER_HIGHPAGE

static inline void copy_user_highpage(struct page *to, struct page *from,
	unsigned long vaddr, struct vm_area_struct *vma)
{
	char *vfrom, *vto;

	vfrom = kmap_local_page(from);
	vto = kmap_local_page(to);
	copy_user_page(vto, vfrom, vaddr, to);
	kmsan_unpoison_memory(page_address(to), PAGE_SIZE);
	kunmap_local(vto);
	kunmap_local(vfrom);
}

#endif

#ifndef __HAVE_ARCH_COPY_HIGHPAGE

static inline void copy_highpage(struct page *to, struct page *from)
{
	char *vfrom, *vto;

	vfrom = kmap_local_page(from);
	vto = kmap_local_page(to);
	copy_page(vto, vfrom);
	kmsan_copy_page_meta(to, from);
	kunmap_local(vto);
	kunmap_local(vfrom);
}

#endif

#ifdef copy_mc_to_kernel

static inline int copy_mc_user_highpage(struct page *to, struct page *from,
					unsigned long vaddr, struct vm_area_struct *vma)
{
	unsigned long ret;
	char *vfrom, *vto;

	vfrom = kmap_local_page(from);
	vto = kmap_local_page(to);
	ret = copy_mc_to_kernel(vto, vfrom, PAGE_SIZE);
	if (!ret)
		kmsan_unpoison_memory(page_address(to), PAGE_SIZE);
	kunmap_local(vto);
	kunmap_local(vfrom);

	return ret;
}

static inline int copy_mc_highpage(struct page *to, struct page *from)
{
	unsigned long ret;
	char *vfrom, *vto;

	vfrom = kmap_local_page(from);
	vto = kmap_local_page(to);
	ret = copy_mc_to_kernel(vto, vfrom, PAGE_SIZE);
	if (!ret)
		kmsan_copy_page_meta(to, from);
	kunmap_local(vto);
	kunmap_local(vfrom);

	return ret;
}
#else
static inline int copy_mc_user_highpage(struct page *to, struct page *from,
					unsigned long vaddr, struct vm_area_struct *vma)
{
	copy_user_highpage(to, from, vaddr, vma);
	return 0;
}

static inline int copy_mc_highpage(struct page *to, struct page *from)
{
	copy_highpage(to, from);
	return 0;
}
#endif

static inline void memcpy_page(struct page *dst_page, size_t dst_off,
			       struct page *src_page, size_t src_off,
			       size_t len)
{
	char *dst = kmap_local_page(dst_page);
	char *src = kmap_local_page(src_page);

	VM_BUG_ON(dst_off + len > PAGE_SIZE || src_off + len > PAGE_SIZE);
	memcpy(dst + dst_off, src + src_off, len);
	kunmap_local(src);
	kunmap_local(dst);
}

static inline void memset_page(struct page *page, size_t offset, int val,
			       size_t len)
{
	char *addr = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memset(addr + offset, val, len);
	kunmap_local(addr);
}

static inline void memcpy_from_page(char *to, struct page *page,
				    size_t offset, size_t len)
{
	char *from = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memcpy(to, from + offset, len);
	kunmap_local(from);
}

static inline void memcpy_to_page(struct page *page, size_t offset,
				  const char *from, size_t len)
{
	char *to = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memcpy(to + offset, from, len);
	flush_dcache_page(page);
	kunmap_local(to);
}

static inline void memzero_page(struct page *page, size_t offset, size_t len)
{
	char *addr = kmap_local_page(page);

	VM_BUG_ON(offset + len > PAGE_SIZE);
	memset(addr + offset, 0, len);
	flush_dcache_page(page);
	kunmap_local(addr);
}

static inline void memcpy_from_folio(char *to, struct folio *folio,
		size_t offset, size_t len)
{
	VM_BUG_ON(offset + len > folio_size(folio));

	do {
		const char *from = kmap_local_folio(folio, offset);
		size_t chunk = len;

		if (folio_test_highmem(folio) &&
		    chunk > PAGE_SIZE - offset_in_page(offset))
			chunk = PAGE_SIZE - offset_in_page(offset);
		memcpy(to, from, chunk);
		kunmap_local(from);

		to += chunk;
		offset += chunk;
		len -= chunk;
	} while (len > 0);
}

static inline void memcpy_to_folio(struct folio *folio, size_t offset,
		const char *from, size_t len)
{
	VM_BUG_ON(offset + len > folio_size(folio));

	do {
		char *to = kmap_local_folio(folio, offset);
		size_t chunk = len;

		if (folio_test_highmem(folio) &&
		    chunk > PAGE_SIZE - offset_in_page(offset))
			chunk = PAGE_SIZE - offset_in_page(offset);
		memcpy(to, from, chunk);
		kunmap_local(to);

		from += chunk;
		offset += chunk;
		len -= chunk;
	} while (len > 0);

	flush_dcache_folio(folio);
}


static inline size_t memcpy_from_file_folio(char *to, struct folio *folio,
		loff_t pos, size_t len)
{
	size_t offset = offset_in_folio(folio, pos);
	char *from = kmap_local_folio(folio, offset);

	if (folio_test_highmem(folio)) {
		offset = offset_in_page(offset);
		len = min_t(size_t, len, PAGE_SIZE - offset);
	} else
		len = min(len, folio_size(folio) - offset);

	memcpy(to, from, len);
	kunmap_local(from);

	return len;
}


static inline void folio_zero_segments(struct folio *folio,
		size_t start1, size_t xend1, size_t start2, size_t xend2)
{
	zero_user_segments(&folio->page, start1, xend1, start2, xend2);
}


static inline void folio_zero_segment(struct folio *folio,
		size_t start, size_t xend)
{
	zero_user_segments(&folio->page, start, xend, 0, 0);
}


static inline void folio_zero_range(struct folio *folio,
		size_t start, size_t length)
{
	zero_user_segments(&folio->page, start, start + length, 0, 0);
}

static inline void unmap_and_put_page(struct page *page, void *addr)
{
	kunmap_local(addr);
	put_page(page);
}

#endif 
