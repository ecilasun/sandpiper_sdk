/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CAN_LENGTH_H
#define _CAN_LENGTH_H

#include <linux/bits.h>
#include <linux/can.h>
#include <linux/can/netlink.h>
#include <linux/math.h>


#define CAN_FRAME_HEADER_SFF_BITS 19


#define CAN_FRAME_HEADER_EFF_BITS 39


#define CANFD_FRAME_HEADER_SFF_BITS 22


#define CANFD_FRAME_HEADER_EFF_BITS 41


#define CAN_FRAME_CRC_FIELD_BITS 16


#define CANFD_FRAME_CRC17_FIELD_BITS 28


#define CANFD_FRAME_CRC21_FIELD_BITS 33


#define CAN_FRAME_FOOTER_BITS 9


#define CAN_INTERMISSION_BITS 3


#define can_bitstuffing_len(destuffed_len)			\
	(destuffed_len + (destuffed_len - 1) / 4)

#define __can_bitstuffing_len(bitstuffing, destuffed_len)	\
	(bitstuffing ? can_bitstuffing_len(destuffed_len) :	\
		       destuffed_len)

#define __can_cc_frame_bits(is_eff, bitstuffing,		\
			    intermission, data_len)		\
(								\
	__can_bitstuffing_len(bitstuffing,			\
		(is_eff ? CAN_FRAME_HEADER_EFF_BITS :		\
			  CAN_FRAME_HEADER_SFF_BITS) +		\
		(data_len) * BITS_PER_BYTE +			\
		CAN_FRAME_CRC_FIELD_BITS) +			\
	CAN_FRAME_FOOTER_BITS +					\
	(intermission ? CAN_INTERMISSION_BITS : 0)		\
)

#define __can_fd_frame_bits(is_eff, bitstuffing,		\
			    intermission, data_len)		\
(								\
	__can_bitstuffing_len(bitstuffing,			\
		(is_eff ? CANFD_FRAME_HEADER_EFF_BITS :		\
			  CANFD_FRAME_HEADER_SFF_BITS) +	\
		(data_len) * BITS_PER_BYTE) +			\
	((data_len) <= 16 ?					\
		CANFD_FRAME_CRC17_FIELD_BITS :			\
		CANFD_FRAME_CRC21_FIELD_BITS) +			\
	CAN_FRAME_FOOTER_BITS +					\
	(intermission ? CAN_INTERMISSION_BITS : 0)		\
)


#define can_frame_bits(is_fd, is_eff, bitstuffing,		\
		       intermission, data_len)			\
(								\
	is_fd ? __can_fd_frame_bits(is_eff, bitstuffing,	\
				    intermission, data_len) :	\
		__can_cc_frame_bits(is_eff, bitstuffing,	\
				    intermission, data_len)	\
)


#define can_frame_bytes(is_fd, is_eff, bitstuffing, data_len)	\
	DIV_ROUND_UP(can_frame_bits(is_fd, is_eff, bitstuffing,	\
				    true, data_len),		\
		     BITS_PER_BYTE)


#define CAN_FRAME_LEN_MAX can_frame_bytes(false, true, false, CAN_MAX_DLEN)


#define CANFD_FRAME_LEN_MAX can_frame_bytes(true, true, false, CANFD_MAX_DLEN)


#define can_cc_dlc2len(dlc)	(min_t(u8, (dlc), CAN_MAX_DLEN))


static inline u8 can_get_cc_dlc(const struct can_frame *cf, const u32 ctrlmode)
{
	
	if ((ctrlmode & CAN_CTRLMODE_CC_LEN8_DLC) &&
	    (cf->len == CAN_MAX_DLEN) &&
	    (cf->len8_dlc > CAN_MAX_DLEN && cf->len8_dlc <= CAN_MAX_RAW_DLC))
		return cf->len8_dlc;

	
	return cf->len;
}


static inline void can_frame_set_cc_len(struct can_frame *cf, const u8 dlc,
					const u32 ctrlmode)
{
	
	if (ctrlmode & CAN_CTRLMODE_CC_LEN8_DLC && dlc > CAN_MAX_DLEN)
		cf->len8_dlc = dlc;

	
	cf->len = can_cc_dlc2len(dlc);
}


u8 can_fd_dlc2len(u8 dlc);


u8 can_fd_len2dlc(u8 len);


unsigned int can_skb_get_frame_len(const struct sk_buff *skb);


static inline u8 canfd_sanitize_len(u8 len)
{
	return can_fd_dlc2len(can_fd_len2dlc(len));
}

#endif 
