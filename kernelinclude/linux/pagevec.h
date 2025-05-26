/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_PAGEVEC_H
#define _LINUX_PAGEVEC_H

#include <linux/types.h>


#define PAGEVEC_SIZE	15

struct folio;


struct folio_batch {
	unsigned char nr;
	bool percpu_pvec_drained;
	struct folio *folios[PAGEVEC_SIZE];
};


static inline void folio_batch_init(struct folio_batch *fbatch)
{
	fbatch->nr = 0;
	fbatch->percpu_pvec_drained = false;
}

static inline void folio_batch_reinit(struct folio_batch *fbatch)
{
	fbatch->nr = 0;
}

static inline unsigned int folio_batch_count(struct folio_batch *fbatch)
{
	return fbatch->nr;
}

static inline unsigned int folio_batch_space(struct folio_batch *fbatch)
{
	return PAGEVEC_SIZE - fbatch->nr;
}


static inline unsigned folio_batch_add(struct folio_batch *fbatch,
		struct folio *folio)
{
	fbatch->folios[fbatch->nr++] = folio;
	return folio_batch_space(fbatch);
}

void __folio_batch_release(struct folio_batch *pvec);

static inline void folio_batch_release(struct folio_batch *fbatch)
{
	if (folio_batch_count(fbatch))
		__folio_batch_release(fbatch);
}

void folio_batch_remove_exceptionals(struct folio_batch *fbatch);
#endif 
