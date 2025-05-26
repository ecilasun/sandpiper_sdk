/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SCATTERLIST_H
#define _LINUX_SCATTERLIST_H

#include <linux/string.h>
#include <linux/types.h>
#include <linux/bug.h>
#include <linux/mm.h>
#include <asm/io.h>

struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	unsigned int	dma_length;
#endif
#ifdef CONFIG_NEED_SG_DMA_FLAGS
	unsigned int    dma_flags;
#endif
};


#define sg_dma_address(sg)	((sg)->dma_address)

#ifdef CONFIG_NEED_SG_DMA_LENGTH
#define sg_dma_len(sg)		((sg)->dma_length)
#else
#define sg_dma_len(sg)		((sg)->length)
#endif

struct sg_table {
	struct scatterlist *sgl;	
	unsigned int nents;		
	unsigned int orig_nents;	
};

struct sg_append_table {
	struct sg_table sgt;		
	struct scatterlist *prv;	
	unsigned int total_nents;	
};



#define SG_CHAIN	0x01UL
#define SG_END		0x02UL


#define SG_PAGE_LINK_MASK (SG_CHAIN | SG_END)

static inline unsigned int __sg_flags(struct scatterlist *sg)
{
	return sg->page_link & SG_PAGE_LINK_MASK;
}

static inline struct scatterlist *sg_chain_ptr(struct scatterlist *sg)
{
	return (struct scatterlist *)(sg->page_link & ~SG_PAGE_LINK_MASK);
}

static inline bool sg_is_chain(struct scatterlist *sg)
{
	return __sg_flags(sg) & SG_CHAIN;
}

static inline bool sg_is_last(struct scatterlist *sg)
{
	return __sg_flags(sg) & SG_END;
}


static inline void sg_assign_page(struct scatterlist *sg, struct page *page)
{
	unsigned long page_link = sg->page_link & (SG_CHAIN | SG_END);

	
	BUG_ON((unsigned long)page & SG_PAGE_LINK_MASK);
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg_is_chain(sg));
#endif
	sg->page_link = page_link | (unsigned long) page;
}


static inline void sg_set_page(struct scatterlist *sg, struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg_assign_page(sg, page);
	sg->offset = offset;
	sg->length = len;
}


static inline void sg_set_folio(struct scatterlist *sg, struct folio *folio,
			       size_t len, size_t offset)
{
	WARN_ON_ONCE(len > UINT_MAX);
	WARN_ON_ONCE(offset > UINT_MAX);
	sg_assign_page(sg, &folio->page);
	sg->offset = offset;
	sg->length = len;
}

static inline struct page *sg_page(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg_is_chain(sg));
#endif
	return (struct page *)((sg)->page_link & ~SG_PAGE_LINK_MASK);
}


static inline void sg_set_buf(struct scatterlist *sg, const void *buf,
			      unsigned int buflen)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(!virt_addr_valid(buf));
#endif
	sg_set_page(sg, virt_to_page(buf), buflen, offset_in_page(buf));
}


#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = sg_next(sg))


#define for_each_sgtable_sg(sgt, sg, i)		\
	for_each_sg((sgt)->sgl, sg, (sgt)->orig_nents, i)


#define for_each_sgtable_dma_sg(sgt, sg, i)	\
	for_each_sg((sgt)->sgl, sg, (sgt)->nents, i)

static inline void __sg_chain(struct scatterlist *chain_sg,
			      struct scatterlist *sgl)
{
	
	chain_sg->offset = 0;
	chain_sg->length = 0;

	
	chain_sg->page_link = ((unsigned long) sgl | SG_CHAIN) & ~SG_END;
}


static inline void sg_chain(struct scatterlist *prv, unsigned int prv_nents,
			    struct scatterlist *sgl)
{
	__sg_chain(&prv[prv_nents - 1], sgl);
}


static inline void sg_mark_end(struct scatterlist *sg)
{
	
	sg->page_link |= SG_END;
	sg->page_link &= ~SG_CHAIN;
}


static inline void sg_unmark_end(struct scatterlist *sg)
{
	sg->page_link &= ~SG_END;
}


#ifdef CONFIG_NEED_SG_DMA_FLAGS

#define SG_DMA_BUS_ADDRESS	(1 << 0)
#define SG_DMA_SWIOTLB		(1 << 1)


static inline bool sg_dma_is_bus_address(struct scatterlist *sg)
{
	return sg->dma_flags & SG_DMA_BUS_ADDRESS;
}


static inline void sg_dma_mark_bus_address(struct scatterlist *sg)
{
	sg->dma_flags |= SG_DMA_BUS_ADDRESS;
}


static inline void sg_dma_unmark_bus_address(struct scatterlist *sg)
{
	sg->dma_flags &= ~SG_DMA_BUS_ADDRESS;
}


static inline bool sg_dma_is_swiotlb(struct scatterlist *sg)
{
	return sg->dma_flags & SG_DMA_SWIOTLB;
}


static inline void sg_dma_mark_swiotlb(struct scatterlist *sg)
{
	sg->dma_flags |= SG_DMA_SWIOTLB;
}

#else

static inline bool sg_dma_is_bus_address(struct scatterlist *sg)
{
	return false;
}
static inline void sg_dma_mark_bus_address(struct scatterlist *sg)
{
}
static inline void sg_dma_unmark_bus_address(struct scatterlist *sg)
{
}
static inline bool sg_dma_is_swiotlb(struct scatterlist *sg)
{
	return false;
}
static inline void sg_dma_mark_swiotlb(struct scatterlist *sg)
{
}

#endif	


static inline dma_addr_t sg_phys(struct scatterlist *sg)
{
	return page_to_phys(sg_page(sg)) + sg->offset;
}


static inline void *sg_virt(struct scatterlist *sg)
{
	return page_address(sg_page(sg)) + sg->offset;
}


static inline void sg_init_marker(struct scatterlist *sgl,
				  unsigned int nents)
{
	sg_mark_end(&sgl[nents - 1]);
}

int sg_nents(struct scatterlist *sg);
int sg_nents_for_len(struct scatterlist *sg, u64 len);
struct scatterlist *sg_next(struct scatterlist *);
struct scatterlist *sg_last(struct scatterlist *s, unsigned int);
void sg_init_table(struct scatterlist *, unsigned int);
void sg_init_one(struct scatterlist *, const void *, unsigned int);
int sg_split(struct scatterlist *in, const int in_mapped_nents,
	     const off_t skip, const int nb_splits,
	     const size_t *split_sizes,
	     struct scatterlist **out, int *out_mapped_nents,
	     gfp_t gfp_mask);

typedef struct scatterlist *(sg_alloc_fn)(unsigned int, gfp_t);
typedef void (sg_free_fn)(struct scatterlist *, unsigned int);

void __sg_free_table(struct sg_table *, unsigned int, unsigned int,
		     sg_free_fn *, unsigned int);
void sg_free_table(struct sg_table *);
void sg_free_append_table(struct sg_append_table *sgt);
int __sg_alloc_table(struct sg_table *, unsigned int, unsigned int,
		     struct scatterlist *, unsigned int, gfp_t, sg_alloc_fn *);
int sg_alloc_table(struct sg_table *, unsigned int, gfp_t);
int sg_alloc_append_table_from_pages(struct sg_append_table *sgt,
				     struct page **pages, unsigned int n_pages,
				     unsigned int offset, unsigned long size,
				     unsigned int max_segment,
				     unsigned int left_pages, gfp_t gfp_mask);
int sg_alloc_table_from_pages_segment(struct sg_table *sgt, struct page **pages,
				      unsigned int n_pages, unsigned int offset,
				      unsigned long size,
				      unsigned int max_segment, gfp_t gfp_mask);


static inline int sg_alloc_table_from_pages(struct sg_table *sgt,
					    struct page **pages,
					    unsigned int n_pages,
					    unsigned int offset,
					    unsigned long size, gfp_t gfp_mask)
{
	return sg_alloc_table_from_pages_segment(sgt, pages, n_pages, offset,
						 size, UINT_MAX, gfp_mask);
}

#ifdef CONFIG_SGL_ALLOC
struct scatterlist *sgl_alloc_order(unsigned long long length,
				    unsigned int order, bool chainable,
				    gfp_t gfp, unsigned int *nent_p);
struct scatterlist *sgl_alloc(unsigned long long length, gfp_t gfp,
			      unsigned int *nent_p);
void sgl_free_n_order(struct scatterlist *sgl, int nents, int order);
void sgl_free_order(struct scatterlist *sgl, int order);
void sgl_free(struct scatterlist *sgl);
#endif 

size_t sg_copy_buffer(struct scatterlist *sgl, unsigned int nents, void *buf,
		      size_t buflen, off_t skip, bool to_buffer);

size_t sg_copy_from_buffer(struct scatterlist *sgl, unsigned int nents,
			   const void *buf, size_t buflen);
size_t sg_copy_to_buffer(struct scatterlist *sgl, unsigned int nents,
			 void *buf, size_t buflen);

size_t sg_pcopy_from_buffer(struct scatterlist *sgl, unsigned int nents,
			    const void *buf, size_t buflen, off_t skip);
size_t sg_pcopy_to_buffer(struct scatterlist *sgl, unsigned int nents,
			  void *buf, size_t buflen, off_t skip);
size_t sg_zero_buffer(struct scatterlist *sgl, unsigned int nents,
		       size_t buflen, off_t skip);


#define SG_MAX_SINGLE_ALLOC		(PAGE_SIZE / sizeof(struct scatterlist))


#define SG_CHUNK_SIZE	128


#ifdef CONFIG_ARCH_NO_SG_CHAIN
#define SG_MAX_SEGMENTS	SG_CHUNK_SIZE
#else
#define SG_MAX_SEGMENTS	2048
#endif

#ifdef CONFIG_SG_POOL
void sg_free_table_chained(struct sg_table *table,
			   unsigned nents_first_chunk);
int sg_alloc_table_chained(struct sg_table *table, int nents,
			   struct scatterlist *first_chunk,
			   unsigned nents_first_chunk);
#endif


struct sg_page_iter {
	struct scatterlist	*sg;		
	unsigned int		sg_pgoffset;	

	
	unsigned int		__nents;	
	int			__pg_advance;	
};


struct sg_dma_page_iter {
	struct sg_page_iter base;
};

bool __sg_page_iter_next(struct sg_page_iter *piter);
bool __sg_page_iter_dma_next(struct sg_dma_page_iter *dma_iter);
void __sg_page_iter_start(struct sg_page_iter *piter,
			  struct scatterlist *sglist, unsigned int nents,
			  unsigned long pgoffset);

static inline struct page *sg_page_iter_page(struct sg_page_iter *piter)
{
	return nth_page(sg_page(piter->sg), piter->sg_pgoffset);
}


static inline dma_addr_t
sg_page_iter_dma_address(struct sg_dma_page_iter *dma_iter)
{
	return sg_dma_address(dma_iter->base.sg) +
	       (dma_iter->base.sg_pgoffset << PAGE_SHIFT);
}


#define for_each_sg_page(sglist, piter, nents, pgoffset)		   \
	for (__sg_page_iter_start((piter), (sglist), (nents), (pgoffset)); \
	     __sg_page_iter_next(piter);)


#define for_each_sg_dma_page(sglist, dma_iter, dma_nents, pgoffset)            \
	for (__sg_page_iter_start(&(dma_iter)->base, sglist, dma_nents,        \
				  pgoffset);                                   \
	     __sg_page_iter_dma_next(dma_iter);)


#define for_each_sgtable_page(sgt, piter, pgoffset)	\
	for_each_sg_page((sgt)->sgl, piter, (sgt)->orig_nents, pgoffset)


#define for_each_sgtable_dma_page(sgt, dma_iter, pgoffset)	\
	for_each_sg_dma_page((sgt)->sgl, dma_iter, (sgt)->nents, pgoffset)




#define SG_MITER_ATOMIC		(1 << 0)	 
#define SG_MITER_TO_SG		(1 << 1)	
#define SG_MITER_FROM_SG	(1 << 2)	

struct sg_mapping_iter {
	
	struct page		*page;		
	void			*addr;		
	size_t			length;		
	size_t			consumed;	
	struct sg_page_iter	piter;		

	
	unsigned int		__offset;	
	unsigned int		__remaining;	
	unsigned int		__flags;
};

void sg_miter_start(struct sg_mapping_iter *miter, struct scatterlist *sgl,
		    unsigned int nents, unsigned int flags);
bool sg_miter_skip(struct sg_mapping_iter *miter, off_t offset);
bool sg_miter_next(struct sg_mapping_iter *miter);
void sg_miter_stop(struct sg_mapping_iter *miter);

#endif 
