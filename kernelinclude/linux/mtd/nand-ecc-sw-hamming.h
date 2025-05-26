/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MTD_NAND_ECC_SW_HAMMING_H__
#define __MTD_NAND_ECC_SW_HAMMING_H__

#include <linux/mtd/nand.h>


struct nand_ecc_sw_hamming_conf {
	struct nand_ecc_req_tweak_ctx req_ctx;
	unsigned int code_size;
	u8 *calc_buf;
	u8 *code_buf;
	unsigned int sm_order;
};

#if IS_ENABLED(CONFIG_MTD_NAND_ECC_SW_HAMMING)

int nand_ecc_sw_hamming_init_ctx(struct nand_device *nand);
void nand_ecc_sw_hamming_cleanup_ctx(struct nand_device *nand);
int ecc_sw_hamming_calculate(const unsigned char *buf, unsigned int step_size,
			     unsigned char *code, bool sm_order);
int nand_ecc_sw_hamming_calculate(struct nand_device *nand,
				  const unsigned char *buf,
				  unsigned char *code);
int ecc_sw_hamming_correct(unsigned char *buf, unsigned char *read_ecc,
			   unsigned char *calc_ecc, unsigned int step_size,
			   bool sm_order);
int nand_ecc_sw_hamming_correct(struct nand_device *nand, unsigned char *buf,
				unsigned char *read_ecc,
				unsigned char *calc_ecc);

#else 

static inline int nand_ecc_sw_hamming_init_ctx(struct nand_device *nand)
{
	return -ENOTSUPP;
}

static inline void nand_ecc_sw_hamming_cleanup_ctx(struct nand_device *nand) {}

static inline int ecc_sw_hamming_calculate(const unsigned char *buf,
					   unsigned int step_size,
					   unsigned char *code, bool sm_order)
{
	return -ENOTSUPP;
}

static inline int nand_ecc_sw_hamming_calculate(struct nand_device *nand,
						const unsigned char *buf,
						unsigned char *code)
{
	return -ENOTSUPP;
}

static inline int ecc_sw_hamming_correct(unsigned char *buf,
					 unsigned char *read_ecc,
					 unsigned char *calc_ecc,
					 unsigned int step_size, bool sm_order)
{
	return -ENOTSUPP;
}

static inline int nand_ecc_sw_hamming_correct(struct nand_device *nand,
					      unsigned char *buf,
					      unsigned char *read_ecc,
					      unsigned char *calc_ecc)
{
	return -ENOTSUPP;
}

#endif 

#endif 
