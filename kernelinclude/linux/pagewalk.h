/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PAGEWALK_H
#define _LINUX_PAGEWALK_H

#include <linux/mm.h>

struct mm_walk;


enum page_walk_lock {
	
	PGWALK_RDLOCK = 0,
	
	PGWALK_WRLOCK = 1,
	
	PGWALK_WRLOCK_VERIFY = 2,
};


struct mm_walk_ops {
	int (*pgd_entry)(pgd_t *pgd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*p4d_entry)(p4d_t *p4d, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pud_entry)(pud_t *pud, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pmd_entry)(pmd_t *pmd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pte_entry)(pte_t *pte, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pte_hole)(unsigned long addr, unsigned long next,
			int depth, struct mm_walk *walk);
	int (*hugetlb_entry)(pte_t *pte, unsigned long hmask,
			     unsigned long addr, unsigned long next,
			     struct mm_walk *walk);
	int (*test_walk)(unsigned long addr, unsigned long next,
			struct mm_walk *walk);
	int (*pre_vma)(unsigned long start, unsigned long end,
		       struct mm_walk *walk);
	void (*post_vma)(struct mm_walk *walk);
	enum page_walk_lock walk_lock;
};


enum page_walk_action {
	
	ACTION_SUBTREE = 0,
	
	ACTION_CONTINUE = 1,
	
	ACTION_AGAIN = 2
};


struct mm_walk {
	const struct mm_walk_ops *ops;
	struct mm_struct *mm;
	pgd_t *pgd;
	struct vm_area_struct *vma;
	enum page_walk_action action;
	bool no_vma;
	void *private;
};

int walk_page_range(struct mm_struct *mm, unsigned long start,
		unsigned long end, const struct mm_walk_ops *ops,
		void *private);
int walk_page_range_novma(struct mm_struct *mm, unsigned long start,
			  unsigned long end, const struct mm_walk_ops *ops,
			  pgd_t *pgd,
			  void *private);
int walk_page_range_vma(struct vm_area_struct *vma, unsigned long start,
			unsigned long end, const struct mm_walk_ops *ops,
			void *private);
int walk_page_vma(struct vm_area_struct *vma, const struct mm_walk_ops *ops,
		void *private);
int walk_page_mapping(struct address_space *mapping, pgoff_t first_index,
		      pgoff_t nr, const struct mm_walk_ops *ops,
		      void *private);

#endif 
