

#ifndef _TTM_CACHING_H_
#define _TTM_CACHING_H_

#define TTM_NUM_CACHING_TYPES	3


enum ttm_caching {
	
	ttm_uncached,

	
	ttm_write_combined,

	
	ttm_cached
};

pgprot_t ttm_prot_from_caching(enum ttm_caching caching, pgprot_t tmp);

#endif
