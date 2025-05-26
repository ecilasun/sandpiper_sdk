/* SPDX-License-Identifier: (GPL-2.0-only OR BSD-3-Clause) */


#ifndef _QED_CHAIN_H
#define _QED_CHAIN_H

#include <linux/types.h>
#include <asm/byteorder.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/qed/common_hsi.h>

enum qed_chain_mode {
	
	QED_CHAIN_MODE_NEXT_PTR,

	
	QED_CHAIN_MODE_SINGLE,

	
	QED_CHAIN_MODE_PBL,
};

enum qed_chain_use_mode {
	QED_CHAIN_USE_TO_PRODUCE,			
	QED_CHAIN_USE_TO_CONSUME,			
	QED_CHAIN_USE_TO_CONSUME_PRODUCE,		
};

enum qed_chain_cnt_type {
	
	QED_CHAIN_CNT_TYPE_U16,

	
	QED_CHAIN_CNT_TYPE_U32,
};

struct qed_chain_next {
	struct regpair					next_phys;
	void						*next_virt;
};

struct qed_chain_pbl_u16 {
	u16						prod_page_idx;
	u16						cons_page_idx;
};

struct qed_chain_pbl_u32 {
	u32						prod_page_idx;
	u32						cons_page_idx;
};

struct qed_chain_u16 {
	
	u16						prod_idx;
	u16						cons_idx;
};

struct qed_chain_u32 {
	
	u32						prod_idx;
	u32						cons_idx;
};

struct addr_tbl_entry {
	void						*virt_addr;
	dma_addr_t					dma_map;
};

struct qed_chain {
	

	
	void						*p_prod_elem;
	void						*p_cons_elem;

	

	struct {
		
		struct addr_tbl_entry			*pp_addr_tbl;

		union {
			struct qed_chain_pbl_u16	u16;
			struct qed_chain_pbl_u32	u32;
		}					c;
	}						pbl;

	union {
		struct qed_chain_u16			chain16;
		struct qed_chain_u32			chain32;
	}						u;

	
	u32						capacity;
	u32						page_cnt;

	enum qed_chain_mode				mode;

	
	u16						elem_per_page;
	u16						elem_per_page_mask;
	u16						elem_size;
	u16						next_page_mask;
	u16						usable_per_page;
	u8						elem_unusable;

	enum qed_chain_cnt_type				cnt_type;

	

	u32						page_size;

	
	struct {
		__le64					*table_virt;
		dma_addr_t				table_phys;
		size_t					table_size;
	}						pbl_sp;

	
	void						*p_virt_addr;
	dma_addr_t					p_phys_addr;

	
	u32						size;

	enum qed_chain_use_mode				intended_use;

	bool						b_external_pbl;
};

struct qed_chain_init_params {
	enum qed_chain_mode				mode;
	enum qed_chain_use_mode				intended_use;
	enum qed_chain_cnt_type				cnt_type;

	u32						page_size;
	u32						num_elems;
	size_t						elem_size;

	void						*ext_pbl_virt;
	dma_addr_t					ext_pbl_phys;
};

#define QED_CHAIN_PAGE_SIZE				SZ_4K

#define ELEMS_PER_PAGE(elem_size, page_size)				     \
	((page_size) / (elem_size))

#define UNUSABLE_ELEMS_PER_PAGE(elem_size, mode)			     \
	(((mode) == QED_CHAIN_MODE_NEXT_PTR) ?				     \
	 (u8)(1 + ((sizeof(struct qed_chain_next) - 1) / (elem_size))) :     \
	 0)

#define USABLE_ELEMS_PER_PAGE(elem_size, page_size, mode)		     \
	((u32)(ELEMS_PER_PAGE((elem_size), (page_size)) -		     \
	       UNUSABLE_ELEMS_PER_PAGE((elem_size), (mode))))

#define QED_CHAIN_PAGE_CNT(elem_cnt, elem_size, page_size, mode)	     \
	DIV_ROUND_UP((elem_cnt),					     \
		     USABLE_ELEMS_PER_PAGE((elem_size), (page_size), (mode)))

#define is_chain_u16(p)							     \
	((p)->cnt_type == QED_CHAIN_CNT_TYPE_U16)
#define is_chain_u32(p)							     \
	((p)->cnt_type == QED_CHAIN_CNT_TYPE_U32)



static inline u16 qed_chain_get_prod_idx(const struct qed_chain *chain)
{
	return chain->u.chain16.prod_idx;
}

static inline u16 qed_chain_get_cons_idx(const struct qed_chain *chain)
{
	return chain->u.chain16.cons_idx;
}

static inline u32 qed_chain_get_prod_idx_u32(const struct qed_chain *chain)
{
	return chain->u.chain32.prod_idx;
}

static inline u32 qed_chain_get_cons_idx_u32(const struct qed_chain *chain)
{
	return chain->u.chain32.cons_idx;
}

static inline u16 qed_chain_get_elem_used(const struct qed_chain *chain)
{
	u32 prod = qed_chain_get_prod_idx(chain);
	u32 cons = qed_chain_get_cons_idx(chain);
	u16 elem_per_page = chain->elem_per_page;
	u16 used;

	if (prod < cons)
		prod += (u32)U16_MAX + 1;

	used = (u16)(prod - cons);
	if (chain->mode == QED_CHAIN_MODE_NEXT_PTR)
		used -= (u16)(prod / elem_per_page - cons / elem_per_page);

	return used;
}

static inline u16 qed_chain_get_elem_left(const struct qed_chain *chain)
{
	return (u16)(chain->capacity - qed_chain_get_elem_used(chain));
}

static inline u32 qed_chain_get_elem_used_u32(const struct qed_chain *chain)
{
	u64 prod = qed_chain_get_prod_idx_u32(chain);
	u64 cons = qed_chain_get_cons_idx_u32(chain);
	u16 elem_per_page = chain->elem_per_page;
	u32 used;

	if (prod < cons)
		prod += (u64)U32_MAX + 1;

	used = (u32)(prod - cons);
	if (chain->mode == QED_CHAIN_MODE_NEXT_PTR)
		used -= (u32)(prod / elem_per_page - cons / elem_per_page);

	return used;
}

static inline u32 qed_chain_get_elem_left_u32(const struct qed_chain *chain)
{
	return chain->capacity - qed_chain_get_elem_used_u32(chain);
}

static inline u16 qed_chain_get_usable_per_page(const struct qed_chain *chain)
{
	return chain->usable_per_page;
}

static inline u8 qed_chain_get_unusable_per_page(const struct qed_chain *chain)
{
	return chain->elem_unusable;
}

static inline u32 qed_chain_get_page_cnt(const struct qed_chain *chain)
{
	return chain->page_cnt;
}

static inline dma_addr_t qed_chain_get_pbl_phys(const struct qed_chain *chain)
{
	return chain->pbl_sp.table_phys;
}


static inline void
qed_chain_advance_page(struct qed_chain *p_chain,
		       void **p_next_elem, void *idx_to_inc, void *page_to_inc)
{
	struct qed_chain_next *p_next = NULL;
	u32 page_index = 0;

	switch (p_chain->mode) {
	case QED_CHAIN_MODE_NEXT_PTR:
		p_next = *p_next_elem;
		*p_next_elem = p_next->next_virt;
		if (is_chain_u16(p_chain))
			*(u16 *)idx_to_inc += p_chain->elem_unusable;
		else
			*(u32 *)idx_to_inc += p_chain->elem_unusable;
		break;
	case QED_CHAIN_MODE_SINGLE:
		*p_next_elem = p_chain->p_virt_addr;
		break;

	case QED_CHAIN_MODE_PBL:
		if (is_chain_u16(p_chain)) {
			if (++(*(u16 *)page_to_inc) == p_chain->page_cnt)
				*(u16 *)page_to_inc = 0;
			page_index = *(u16 *)page_to_inc;
		} else {
			if (++(*(u32 *)page_to_inc) == p_chain->page_cnt)
				*(u32 *)page_to_inc = 0;
			page_index = *(u32 *)page_to_inc;
		}
		*p_next_elem = p_chain->pbl.pp_addr_tbl[page_index].virt_addr;
	}
}

#define is_unusable_idx(p, idx)	\
	(((p)->u.chain16.idx & (p)->elem_per_page_mask) == (p)->usable_per_page)

#define is_unusable_idx_u32(p, idx) \
	(((p)->u.chain32.idx & (p)->elem_per_page_mask) == (p)->usable_per_page)
#define is_unusable_next_idx(p, idx)				 \
	((((p)->u.chain16.idx + 1) & (p)->elem_per_page_mask) == \
	 (p)->usable_per_page)

#define is_unusable_next_idx_u32(p, idx)			 \
	((((p)->u.chain32.idx + 1) & (p)->elem_per_page_mask) == \
	 (p)->usable_per_page)

#define test_and_skip(p, idx)						   \
	do {						\
		if (is_chain_u16(p)) {					   \
			if (is_unusable_idx(p, idx))			   \
				(p)->u.chain16.idx += (p)->elem_unusable;  \
		} else {						   \
			if (is_unusable_idx_u32(p, idx))		   \
				(p)->u.chain32.idx += (p)->elem_unusable;  \
		}					\
	} while (0)


static inline void qed_chain_return_produced(struct qed_chain *p_chain)
{
	if (is_chain_u16(p_chain))
		p_chain->u.chain16.cons_idx++;
	else
		p_chain->u.chain32.cons_idx++;
	test_and_skip(p_chain, cons_idx);
}


static inline void *qed_chain_produce(struct qed_chain *p_chain)
{
	void *p_ret = NULL, *p_prod_idx, *p_prod_page_idx;

	if (is_chain_u16(p_chain)) {
		if ((p_chain->u.chain16.prod_idx &
		     p_chain->elem_per_page_mask) == p_chain->next_page_mask) {
			p_prod_idx = &p_chain->u.chain16.prod_idx;
			p_prod_page_idx = &p_chain->pbl.c.u16.prod_page_idx;
			qed_chain_advance_page(p_chain, &p_chain->p_prod_elem,
					       p_prod_idx, p_prod_page_idx);
		}
		p_chain->u.chain16.prod_idx++;
	} else {
		if ((p_chain->u.chain32.prod_idx &
		     p_chain->elem_per_page_mask) == p_chain->next_page_mask) {
			p_prod_idx = &p_chain->u.chain32.prod_idx;
			p_prod_page_idx = &p_chain->pbl.c.u32.prod_page_idx;
			qed_chain_advance_page(p_chain, &p_chain->p_prod_elem,
					       p_prod_idx, p_prod_page_idx);
		}
		p_chain->u.chain32.prod_idx++;
	}

	p_ret = p_chain->p_prod_elem;
	p_chain->p_prod_elem = (void *)(((u8 *)p_chain->p_prod_elem) +
					p_chain->elem_size);

	return p_ret;
}


static inline u32 qed_chain_get_capacity(struct qed_chain *p_chain)
{
	return p_chain->capacity;
}


static inline void qed_chain_recycle_consumed(struct qed_chain *p_chain)
{
	test_and_skip(p_chain, prod_idx);
	if (is_chain_u16(p_chain))
		p_chain->u.chain16.prod_idx++;
	else
		p_chain->u.chain32.prod_idx++;
}


static inline void *qed_chain_consume(struct qed_chain *p_chain)
{
	void *p_ret = NULL, *p_cons_idx, *p_cons_page_idx;

	if (is_chain_u16(p_chain)) {
		if ((p_chain->u.chain16.cons_idx &
		     p_chain->elem_per_page_mask) == p_chain->next_page_mask) {
			p_cons_idx = &p_chain->u.chain16.cons_idx;
			p_cons_page_idx = &p_chain->pbl.c.u16.cons_page_idx;
			qed_chain_advance_page(p_chain, &p_chain->p_cons_elem,
					       p_cons_idx, p_cons_page_idx);
		}
		p_chain->u.chain16.cons_idx++;
	} else {
		if ((p_chain->u.chain32.cons_idx &
		     p_chain->elem_per_page_mask) == p_chain->next_page_mask) {
			p_cons_idx = &p_chain->u.chain32.cons_idx;
			p_cons_page_idx = &p_chain->pbl.c.u32.cons_page_idx;
			qed_chain_advance_page(p_chain, &p_chain->p_cons_elem,
					       p_cons_idx, p_cons_page_idx);
		}
		p_chain->u.chain32.cons_idx++;
	}

	p_ret = p_chain->p_cons_elem;
	p_chain->p_cons_elem = (void *)(((u8 *)p_chain->p_cons_elem) +
					p_chain->elem_size);

	return p_ret;
}


static inline void qed_chain_reset(struct qed_chain *p_chain)
{
	u32 i;

	if (is_chain_u16(p_chain)) {
		p_chain->u.chain16.prod_idx = 0;
		p_chain->u.chain16.cons_idx = 0;
	} else {
		p_chain->u.chain32.prod_idx = 0;
		p_chain->u.chain32.cons_idx = 0;
	}
	p_chain->p_cons_elem = p_chain->p_virt_addr;
	p_chain->p_prod_elem = p_chain->p_virt_addr;

	if (p_chain->mode == QED_CHAIN_MODE_PBL) {
		
		u32 reset_val = p_chain->page_cnt - 1;

		if (is_chain_u16(p_chain)) {
			p_chain->pbl.c.u16.prod_page_idx = (u16)reset_val;
			p_chain->pbl.c.u16.cons_page_idx = (u16)reset_val;
		} else {
			p_chain->pbl.c.u32.prod_page_idx = reset_val;
			p_chain->pbl.c.u32.cons_page_idx = reset_val;
		}
	}

	switch (p_chain->intended_use) {
	case QED_CHAIN_USE_TO_CONSUME:
		
		for (i = 0; i < p_chain->capacity; i++)
			qed_chain_recycle_consumed(p_chain);
		break;

	case QED_CHAIN_USE_TO_CONSUME_PRODUCE:
	case QED_CHAIN_USE_TO_PRODUCE:
	default:
		
		break;
	}
}


static inline void *qed_chain_get_last_elem(struct qed_chain *p_chain)
{
	struct qed_chain_next *p_next = NULL;
	void *p_virt_addr = NULL;
	u32 size, last_page_idx;

	if (!p_chain->p_virt_addr)
		goto out;

	switch (p_chain->mode) {
	case QED_CHAIN_MODE_NEXT_PTR:
		size = p_chain->elem_size * p_chain->usable_per_page;
		p_virt_addr = p_chain->p_virt_addr;
		p_next = (struct qed_chain_next *)((u8 *)p_virt_addr + size);
		while (p_next->next_virt != p_chain->p_virt_addr) {
			p_virt_addr = p_next->next_virt;
			p_next = (struct qed_chain_next *)((u8 *)p_virt_addr +
							   size);
		}
		break;
	case QED_CHAIN_MODE_SINGLE:
		p_virt_addr = p_chain->p_virt_addr;
		break;
	case QED_CHAIN_MODE_PBL:
		last_page_idx = p_chain->page_cnt - 1;
		p_virt_addr = p_chain->pbl.pp_addr_tbl[last_page_idx].virt_addr;
		break;
	}
	
	size = p_chain->elem_size * (p_chain->usable_per_page - 1);
	p_virt_addr = (u8 *)p_virt_addr + size;
out:
	return p_virt_addr;
}


static inline void qed_chain_set_prod(struct qed_chain *p_chain,
				      u32 prod_idx, void *p_prod_elem)
{
	if (p_chain->mode == QED_CHAIN_MODE_PBL) {
		u32 cur_prod, page_mask, page_cnt, page_diff;

		cur_prod = is_chain_u16(p_chain) ? p_chain->u.chain16.prod_idx :
			   p_chain->u.chain32.prod_idx;

		
		page_mask = ~p_chain->elem_per_page_mask;

		
		page_diff = (((cur_prod - 1) & page_mask) -
			     ((prod_idx - 1) & page_mask)) /
			    p_chain->elem_per_page;

		page_cnt = qed_chain_get_page_cnt(p_chain);
		if (is_chain_u16(p_chain))
			p_chain->pbl.c.u16.prod_page_idx =
				(p_chain->pbl.c.u16.prod_page_idx -
				 page_diff + page_cnt) % page_cnt;
		else
			p_chain->pbl.c.u32.prod_page_idx =
				(p_chain->pbl.c.u32.prod_page_idx -
				 page_diff + page_cnt) % page_cnt;
	}

	if (is_chain_u16(p_chain))
		p_chain->u.chain16.prod_idx = (u16) prod_idx;
	else
		p_chain->u.chain32.prod_idx = prod_idx;
	p_chain->p_prod_elem = p_prod_elem;
}


static inline void qed_chain_pbl_zero_mem(struct qed_chain *p_chain)
{
	u32 i, page_cnt;

	if (p_chain->mode != QED_CHAIN_MODE_PBL)
		return;

	page_cnt = qed_chain_get_page_cnt(p_chain);

	for (i = 0; i < page_cnt; i++)
		memset(p_chain->pbl.pp_addr_tbl[i].virt_addr, 0,
		       p_chain->page_size);
}

#endif
