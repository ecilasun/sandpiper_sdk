/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MTD_NAND_ECC_SW_BCH_H__
#define __MTD_NAND_ECC_SW_BCH_H__

#include <linux/mtd/nand.h>
#include <linux/bch.h>


struct nand_ecc_sw_bch_conf {
	struct nand_ecc_req_tweak_ctx req_ctx;
	unsigned int code_size;
	u8 *calc_buf;
	u8 *code_buf;
	struct bch_control *bch;
	unsigned int *errloc;
	unsigned char *eccmask;
};

#if IS_ENABLED(CONFIG_MTD_NAND_ECC_SW_BCH)

int nand_ecc_sw_bch_calculate(struct nand_device *nand,
			      const unsigned char *buf, unsigned char *code);
int nand_ecc_sw_bch_correct(struct nand_device *nand, unsigned char *buf,
			    unsigned char *read_ecc, unsigned char *calc_ecc);
int nand_ecc_sw_bch_init_ctx(struct nand_device *nand);
void nand_ecc_sw_bch_cleanup_ctx(struct nand_device *nand);
struct nand_ecc_engine *nand_ecc_sw_bch_get_engine(void);

#else 

static inline int nand_ecc_sw_bch_calculate(struct nand_device *nand,
					    const unsigned char *buf,
					    unsigned char *code)
{
	return -ENOTSUPP;
}

static inline int nand_ecc_sw_bch_correct(struct nand_device *nand,
					  unsigned char *buf,
					  unsigned char *read_ecc,
					  unsigned char *calc_ecc)
{
	return -ENOTSUPP;
}

static inline int nand_ecc_sw_bch_init_ctx(struct nand_device *nand)
{
	return -ENOTSUPP;
}

static inline void nand_ecc_sw_bch_cleanup_ctx(struct nand_device *nand) {}

#endif 

#endif 
