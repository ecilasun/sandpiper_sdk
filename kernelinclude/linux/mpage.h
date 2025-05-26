/* SPDX-License-Identifier: GPL-2.0 */



#ifdef CONFIG_BLOCK

struct writeback_control;
struct readahead_control;

void mpage_readahead(struct readahead_control *, get_block_t get_block);
int mpage_read_folio(struct folio *folio, get_block_t get_block);
int mpage_writepages(struct address_space *mapping,
		struct writeback_control *wbc, get_block_t get_block);

#endif
