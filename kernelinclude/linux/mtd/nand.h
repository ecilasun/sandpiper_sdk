/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_MTD_NAND_H
#define __LINUX_MTD_NAND_H

#include <linux/mtd/mtd.h>

struct nand_device;


struct nand_memory_organization {
	unsigned int bits_per_cell;
	unsigned int pagesize;
	unsigned int oobsize;
	unsigned int pages_per_eraseblock;
	unsigned int eraseblocks_per_lun;
	unsigned int max_bad_eraseblocks_per_lun;
	unsigned int planes_per_lun;
	unsigned int luns_per_target;
	unsigned int ntargets;
};

#define NAND_MEMORG(bpc, ps, os, ppe, epl, mbb, ppl, lpt, nt)	\
	{							\
		.bits_per_cell = (bpc),				\
		.pagesize = (ps),				\
		.oobsize = (os),				\
		.pages_per_eraseblock = (ppe),			\
		.eraseblocks_per_lun = (epl),			\
		.max_bad_eraseblocks_per_lun = (mbb),		\
		.planes_per_lun = (ppl),			\
		.luns_per_target = (lpt),			\
		.ntargets = (nt),				\
	}


struct nand_row_converter {
	unsigned int lun_addr_shift;
	unsigned int eraseblock_addr_shift;
};


struct nand_pos {
	unsigned int target;
	unsigned int lun;
	unsigned int plane;
	unsigned int eraseblock;
	unsigned int page;
};


enum nand_page_io_req_type {
	NAND_PAGE_READ = 0,
	NAND_PAGE_WRITE,
};


struct nand_page_io_req {
	enum nand_page_io_req_type type;
	struct nand_pos pos;
	unsigned int dataoffs;
	unsigned int datalen;
	union {
		const void *out;
		void *in;
	} databuf;
	unsigned int ooboffs;
	unsigned int ooblen;
	union {
		const void *out;
		void *in;
	} oobbuf;
	int mode;
};

const struct mtd_ooblayout_ops *nand_get_small_page_ooblayout(void);
const struct mtd_ooblayout_ops *nand_get_large_page_ooblayout(void);
const struct mtd_ooblayout_ops *nand_get_large_page_hamming_ooblayout(void);


enum nand_ecc_engine_type {
	NAND_ECC_ENGINE_TYPE_INVALID,
	NAND_ECC_ENGINE_TYPE_NONE,
	NAND_ECC_ENGINE_TYPE_SOFT,
	NAND_ECC_ENGINE_TYPE_ON_HOST,
	NAND_ECC_ENGINE_TYPE_ON_DIE,
};


enum nand_ecc_placement {
	NAND_ECC_PLACEMENT_UNKNOWN,
	NAND_ECC_PLACEMENT_OOB,
	NAND_ECC_PLACEMENT_INTERLEAVED,
};


enum nand_ecc_algo {
	NAND_ECC_ALGO_UNKNOWN,
	NAND_ECC_ALGO_HAMMING,
	NAND_ECC_ALGO_BCH,
	NAND_ECC_ALGO_RS,
};


struct nand_ecc_props {
	enum nand_ecc_engine_type engine_type;
	enum nand_ecc_placement placement;
	enum nand_ecc_algo algo;
	unsigned int strength;
	unsigned int step_size;
	unsigned int flags;
};

#define NAND_ECCREQ(str, stp) { .strength = (str), .step_size = (stp) }


#define NAND_ECC_MAXIMIZE_STRENGTH BIT(0)


struct nand_bbt {
	unsigned long *cache;
};


struct nand_ops {
	int (*erase)(struct nand_device *nand, const struct nand_pos *pos);
	int (*markbad)(struct nand_device *nand, const struct nand_pos *pos);
	bool (*isbad)(struct nand_device *nand, const struct nand_pos *pos);
};


struct nand_ecc_context {
	struct nand_ecc_props conf;
	unsigned int nsteps;
	unsigned int total;
	void *priv;
};


struct nand_ecc_engine_ops {
	int (*init_ctx)(struct nand_device *nand);
	void (*cleanup_ctx)(struct nand_device *nand);
	int (*prepare_io_req)(struct nand_device *nand,
			      struct nand_page_io_req *req);
	int (*finish_io_req)(struct nand_device *nand,
			     struct nand_page_io_req *req);
};


enum nand_ecc_engine_integration {
	NAND_ECC_ENGINE_INTEGRATION_INVALID,
	NAND_ECC_ENGINE_INTEGRATION_PIPELINED,
	NAND_ECC_ENGINE_INTEGRATION_EXTERNAL,
};


struct nand_ecc_engine {
	struct device *dev;
	struct list_head node;
	struct nand_ecc_engine_ops *ops;
	enum nand_ecc_engine_integration integration;
	void *priv;
};

void of_get_nand_ecc_user_config(struct nand_device *nand);
int nand_ecc_init_ctx(struct nand_device *nand);
void nand_ecc_cleanup_ctx(struct nand_device *nand);
int nand_ecc_prepare_io_req(struct nand_device *nand,
			    struct nand_page_io_req *req);
int nand_ecc_finish_io_req(struct nand_device *nand,
			   struct nand_page_io_req *req);
bool nand_ecc_is_strong_enough(struct nand_device *nand);

#if IS_REACHABLE(CONFIG_MTD_NAND_CORE)
int nand_ecc_register_on_host_hw_engine(struct nand_ecc_engine *engine);
int nand_ecc_unregister_on_host_hw_engine(struct nand_ecc_engine *engine);
#else
static inline int
nand_ecc_register_on_host_hw_engine(struct nand_ecc_engine *engine)
{
	return -ENOTSUPP;
}
static inline int
nand_ecc_unregister_on_host_hw_engine(struct nand_ecc_engine *engine)
{
	return -ENOTSUPP;
}
#endif

struct nand_ecc_engine *nand_ecc_get_sw_engine(struct nand_device *nand);
struct nand_ecc_engine *nand_ecc_get_on_die_hw_engine(struct nand_device *nand);
struct nand_ecc_engine *nand_ecc_get_on_host_hw_engine(struct nand_device *nand);
void nand_ecc_put_on_host_hw_engine(struct nand_device *nand);
struct device *nand_ecc_get_engine_dev(struct device *host);

#if IS_ENABLED(CONFIG_MTD_NAND_ECC_SW_HAMMING)
struct nand_ecc_engine *nand_ecc_sw_hamming_get_engine(void);
#else
static inline struct nand_ecc_engine *nand_ecc_sw_hamming_get_engine(void)
{
	return NULL;
}
#endif 

#if IS_ENABLED(CONFIG_MTD_NAND_ECC_SW_BCH)
struct nand_ecc_engine *nand_ecc_sw_bch_get_engine(void);
#else
static inline struct nand_ecc_engine *nand_ecc_sw_bch_get_engine(void)
{
	return NULL;
}
#endif 


struct nand_ecc_req_tweak_ctx {
	struct nand_page_io_req orig_req;
	struct nand_device *nand;
	unsigned int page_buffer_size;
	unsigned int oob_buffer_size;
	void *spare_databuf;
	void *spare_oobbuf;
	bool bounce_data;
	bool bounce_oob;
};

int nand_ecc_init_req_tweaking(struct nand_ecc_req_tweak_ctx *ctx,
			       struct nand_device *nand);
void nand_ecc_cleanup_req_tweaking(struct nand_ecc_req_tweak_ctx *ctx);
void nand_ecc_tweak_req(struct nand_ecc_req_tweak_ctx *ctx,
			struct nand_page_io_req *req);
void nand_ecc_restore_req(struct nand_ecc_req_tweak_ctx *ctx,
			  struct nand_page_io_req *req);


struct nand_ecc {
	struct nand_ecc_props defaults;
	struct nand_ecc_props requirements;
	struct nand_ecc_props user_conf;
	struct nand_ecc_context ctx;
	struct nand_ecc_engine *ondie_engine;
	struct nand_ecc_engine *engine;
};


struct nand_device {
	struct mtd_info mtd;
	struct nand_memory_organization memorg;
	struct nand_ecc ecc;
	struct nand_row_converter rowconv;
	struct nand_bbt bbt;
	const struct nand_ops *ops;
};


struct nand_io_iter {
	struct nand_page_io_req req;
	unsigned int oobbytes_per_page;
	unsigned int dataleft;
	unsigned int oobleft;
};


static inline struct nand_device *mtd_to_nanddev(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_device, mtd);
}


static inline struct mtd_info *nanddev_to_mtd(struct nand_device *nand)
{
	return &nand->mtd;
}


static inline unsigned int nanddev_bits_per_cell(const struct nand_device *nand)
{
	return nand->memorg.bits_per_cell;
}


static inline size_t nanddev_page_size(const struct nand_device *nand)
{
	return nand->memorg.pagesize;
}


static inline unsigned int
nanddev_per_page_oobsize(const struct nand_device *nand)
{
	return nand->memorg.oobsize;
}


static inline unsigned int
nanddev_pages_per_eraseblock(const struct nand_device *nand)
{
	return nand->memorg.pages_per_eraseblock;
}


static inline unsigned int
nanddev_pages_per_target(const struct nand_device *nand)
{
	return nand->memorg.pages_per_eraseblock *
	       nand->memorg.eraseblocks_per_lun *
	       nand->memorg.luns_per_target;
}


static inline size_t nanddev_eraseblock_size(const struct nand_device *nand)
{
	return nand->memorg.pagesize * nand->memorg.pages_per_eraseblock;
}


static inline unsigned int
nanddev_eraseblocks_per_lun(const struct nand_device *nand)
{
	return nand->memorg.eraseblocks_per_lun;
}


static inline unsigned int
nanddev_eraseblocks_per_target(const struct nand_device *nand)
{
	return nand->memorg.eraseblocks_per_lun * nand->memorg.luns_per_target;
}


static inline u64 nanddev_target_size(const struct nand_device *nand)
{
	return (u64)nand->memorg.luns_per_target *
	       nand->memorg.eraseblocks_per_lun *
	       nand->memorg.pages_per_eraseblock *
	       nand->memorg.pagesize;
}


static inline unsigned int nanddev_ntargets(const struct nand_device *nand)
{
	return nand->memorg.ntargets;
}


static inline unsigned int nanddev_neraseblocks(const struct nand_device *nand)
{
	return nand->memorg.ntargets * nand->memorg.luns_per_target *
	       nand->memorg.eraseblocks_per_lun;
}


static inline u64 nanddev_size(const struct nand_device *nand)
{
	return nanddev_target_size(nand) * nanddev_ntargets(nand);
}


static inline struct nand_memory_organization *
nanddev_get_memorg(struct nand_device *nand)
{
	return &nand->memorg;
}


static inline const struct nand_ecc_props *
nanddev_get_ecc_conf(struct nand_device *nand)
{
	return &nand->ecc.ctx.conf;
}


static inline unsigned int
nanddev_get_ecc_nsteps(struct nand_device *nand)
{
	return nand->ecc.ctx.nsteps;
}


static inline unsigned int
nanddev_get_ecc_bytes_per_step(struct nand_device *nand)
{
	return nand->ecc.ctx.total / nand->ecc.ctx.nsteps;
}


static inline const struct nand_ecc_props *
nanddev_get_ecc_requirements(struct nand_device *nand)
{
	return &nand->ecc.requirements;
}


static inline void
nanddev_set_ecc_requirements(struct nand_device *nand,
			     const struct nand_ecc_props *reqs)
{
	nand->ecc.requirements = *reqs;
}

int nanddev_init(struct nand_device *nand, const struct nand_ops *ops,
		 struct module *owner);
void nanddev_cleanup(struct nand_device *nand);


static inline int nanddev_register(struct nand_device *nand)
{
	return mtd_device_register(&nand->mtd, NULL, 0);
}


static inline int nanddev_unregister(struct nand_device *nand)
{
	return mtd_device_unregister(&nand->mtd);
}


static inline void nanddev_set_of_node(struct nand_device *nand,
				       struct device_node *np)
{
	mtd_set_of_node(&nand->mtd, np);
}


static inline struct device_node *nanddev_get_of_node(struct nand_device *nand)
{
	return mtd_get_of_node(&nand->mtd);
}


static inline unsigned int nanddev_offs_to_pos(struct nand_device *nand,
					       loff_t offs,
					       struct nand_pos *pos)
{
	unsigned int pageoffs;
	u64 tmp = offs;

	pageoffs = do_div(tmp, nand->memorg.pagesize);
	pos->page = do_div(tmp, nand->memorg.pages_per_eraseblock);
	pos->eraseblock = do_div(tmp, nand->memorg.eraseblocks_per_lun);
	pos->plane = pos->eraseblock % nand->memorg.planes_per_lun;
	pos->lun = do_div(tmp, nand->memorg.luns_per_target);
	pos->target = tmp;

	return pageoffs;
}


static inline int nanddev_pos_cmp(const struct nand_pos *a,
				  const struct nand_pos *b)
{
	if (a->target != b->target)
		return a->target < b->target ? -1 : 1;

	if (a->lun != b->lun)
		return a->lun < b->lun ? -1 : 1;

	if (a->eraseblock != b->eraseblock)
		return a->eraseblock < b->eraseblock ? -1 : 1;

	if (a->page != b->page)
		return a->page < b->page ? -1 : 1;

	return 0;
}


static inline loff_t nanddev_pos_to_offs(struct nand_device *nand,
					 const struct nand_pos *pos)
{
	unsigned int npages;

	npages = pos->page +
		 ((pos->eraseblock +
		   (pos->lun +
		    (pos->target * nand->memorg.luns_per_target)) *
		   nand->memorg.eraseblocks_per_lun) *
		  nand->memorg.pages_per_eraseblock);

	return (loff_t)npages * nand->memorg.pagesize;
}


static inline unsigned int nanddev_pos_to_row(struct nand_device *nand,
					      const struct nand_pos *pos)
{
	return (pos->lun << nand->rowconv.lun_addr_shift) |
	       (pos->eraseblock << nand->rowconv.eraseblock_addr_shift) |
	       pos->page;
}


static inline void nanddev_pos_next_target(struct nand_device *nand,
					   struct nand_pos *pos)
{
	pos->page = 0;
	pos->plane = 0;
	pos->eraseblock = 0;
	pos->lun = 0;
	pos->target++;
}


static inline void nanddev_pos_next_lun(struct nand_device *nand,
					struct nand_pos *pos)
{
	if (pos->lun >= nand->memorg.luns_per_target - 1)
		return nanddev_pos_next_target(nand, pos);

	pos->lun++;
	pos->page = 0;
	pos->plane = 0;
	pos->eraseblock = 0;
}


static inline void nanddev_pos_next_eraseblock(struct nand_device *nand,
					       struct nand_pos *pos)
{
	if (pos->eraseblock >= nand->memorg.eraseblocks_per_lun - 1)
		return nanddev_pos_next_lun(nand, pos);

	pos->eraseblock++;
	pos->page = 0;
	pos->plane = pos->eraseblock % nand->memorg.planes_per_lun;
}


static inline void nanddev_pos_next_page(struct nand_device *nand,
					 struct nand_pos *pos)
{
	if (pos->page >= nand->memorg.pages_per_eraseblock - 1)
		return nanddev_pos_next_eraseblock(nand, pos);

	pos->page++;
}


static inline void nanddev_io_iter_init(struct nand_device *nand,
					enum nand_page_io_req_type reqtype,
					loff_t offs, struct mtd_oob_ops *req,
					struct nand_io_iter *iter)
{
	struct mtd_info *mtd = nanddev_to_mtd(nand);

	iter->req.type = reqtype;
	iter->req.mode = req->mode;
	iter->req.dataoffs = nanddev_offs_to_pos(nand, offs, &iter->req.pos);
	iter->req.ooboffs = req->ooboffs;
	iter->oobbytes_per_page = mtd_oobavail(mtd, req);
	iter->dataleft = req->len;
	iter->oobleft = req->ooblen;
	iter->req.databuf.in = req->datbuf;
	iter->req.datalen = min_t(unsigned int,
				  nand->memorg.pagesize - iter->req.dataoffs,
				  iter->dataleft);
	iter->req.oobbuf.in = req->oobbuf;
	iter->req.ooblen = min_t(unsigned int,
				 iter->oobbytes_per_page - iter->req.ooboffs,
				 iter->oobleft);
}


static inline void nanddev_io_iter_next_page(struct nand_device *nand,
					     struct nand_io_iter *iter)
{
	nanddev_pos_next_page(nand, &iter->req.pos);
	iter->dataleft -= iter->req.datalen;
	iter->req.databuf.in += iter->req.datalen;
	iter->oobleft -= iter->req.ooblen;
	iter->req.oobbuf.in += iter->req.ooblen;
	iter->req.dataoffs = 0;
	iter->req.ooboffs = 0;
	iter->req.datalen = min_t(unsigned int, nand->memorg.pagesize,
				  iter->dataleft);
	iter->req.ooblen = min_t(unsigned int, iter->oobbytes_per_page,
				 iter->oobleft);
}


static inline bool nanddev_io_iter_end(struct nand_device *nand,
				       const struct nand_io_iter *iter)
{
	if (iter->dataleft || iter->oobleft)
		return false;

	return true;
}


#define nanddev_io_for_each_page(nand, type, start, req, iter)		\
	for (nanddev_io_iter_init(nand, type, start, req, iter);	\
	     !nanddev_io_iter_end(nand, iter);				\
	     nanddev_io_iter_next_page(nand, iter))

bool nanddev_isbad(struct nand_device *nand, const struct nand_pos *pos);
bool nanddev_isreserved(struct nand_device *nand, const struct nand_pos *pos);
int nanddev_markbad(struct nand_device *nand, const struct nand_pos *pos);


int nanddev_ecc_engine_init(struct nand_device *nand);
void nanddev_ecc_engine_cleanup(struct nand_device *nand);

static inline void *nand_to_ecc_ctx(struct nand_device *nand)
{
	return nand->ecc.ctx.priv;
}


enum nand_bbt_block_status {
	NAND_BBT_BLOCK_STATUS_UNKNOWN,
	NAND_BBT_BLOCK_GOOD,
	NAND_BBT_BLOCK_WORN,
	NAND_BBT_BLOCK_RESERVED,
	NAND_BBT_BLOCK_FACTORY_BAD,
	NAND_BBT_BLOCK_NUM_STATUS,
};

int nanddev_bbt_init(struct nand_device *nand);
void nanddev_bbt_cleanup(struct nand_device *nand);
int nanddev_bbt_update(struct nand_device *nand);
int nanddev_bbt_get_block_status(const struct nand_device *nand,
				 unsigned int entry);
int nanddev_bbt_set_block_status(struct nand_device *nand, unsigned int entry,
				 enum nand_bbt_block_status status);
int nanddev_bbt_markbad(struct nand_device *nand, unsigned int block);


static inline unsigned int nanddev_bbt_pos_to_entry(struct nand_device *nand,
						    const struct nand_pos *pos)
{
	return pos->eraseblock +
	       ((pos->lun + (pos->target * nand->memorg.luns_per_target)) *
		nand->memorg.eraseblocks_per_lun);
}


static inline bool nanddev_bbt_is_initialized(struct nand_device *nand)
{
	return !!nand->bbt.cache;
}


int nanddev_mtd_erase(struct mtd_info *mtd, struct erase_info *einfo);
int nanddev_mtd_max_bad_blocks(struct mtd_info *mtd, loff_t offs, size_t len);

#endif 
