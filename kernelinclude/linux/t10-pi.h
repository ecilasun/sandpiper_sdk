/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_T10_PI_H
#define _LINUX_T10_PI_H

#include <linux/types.h>
#include <linux/blk-mq.h>


enum t10_dif_type {
	T10_PI_TYPE0_PROTECTION = 0x0,
	T10_PI_TYPE1_PROTECTION = 0x1,
	T10_PI_TYPE2_PROTECTION = 0x2,
	T10_PI_TYPE3_PROTECTION = 0x3,
};


struct t10_pi_tuple {
	__be16 guard_tag;	
	__be16 app_tag;		
	__be32 ref_tag;		
};

#define T10_PI_APP_ESCAPE cpu_to_be16(0xffff)
#define T10_PI_REF_ESCAPE cpu_to_be32(0xffffffff)

static inline u32 t10_pi_ref_tag(struct request *rq)
{
	unsigned int shift = ilog2(queue_logical_block_size(rq->q));

#ifdef CONFIG_BLK_DEV_INTEGRITY
	if (rq->q->integrity.interval_exp)
		shift = rq->q->integrity.interval_exp;
#endif
	return blk_rq_pos(rq) >> (shift - SECTOR_SHIFT) & 0xffffffff;
}

extern const struct blk_integrity_profile t10_pi_type1_crc;
extern const struct blk_integrity_profile t10_pi_type1_ip;
extern const struct blk_integrity_profile t10_pi_type3_crc;
extern const struct blk_integrity_profile t10_pi_type3_ip;

struct crc64_pi_tuple {
	__be64 guard_tag;
	__be16 app_tag;
	__u8   ref_tag[6];
};


static inline u64 lower_48_bits(u64 n)
{
	return n & ((1ull << 48) - 1);
}

static inline u64 ext_pi_ref_tag(struct request *rq)
{
	unsigned int shift = ilog2(queue_logical_block_size(rq->q));

#ifdef CONFIG_BLK_DEV_INTEGRITY
	if (rq->q->integrity.interval_exp)
		shift = rq->q->integrity.interval_exp;
#endif
	return lower_48_bits(blk_rq_pos(rq) >> (shift - SECTOR_SHIFT));
}

extern const struct blk_integrity_profile ext_pi_type1_crc64;
extern const struct blk_integrity_profile ext_pi_type3_crc64;

#endif
