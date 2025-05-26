/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __TI_CPPI5_H__
#define __TI_CPPI5_H__

#include <linux/bitops.h>
#include <linux/printk.h>
#include <linux/bug.h>


struct cppi5_desc_hdr_t {
	u32 pkt_info0;
	u32 pkt_info1;
	u32 pkt_info2;
	u32 src_dst_tag;
} __packed;


struct cppi5_host_desc_t {
	struct cppi5_desc_hdr_t hdr;
	u64 next_desc;
	u64 buf_ptr;
	u32 buf_info1;
	u32 org_buf_len;
	u64 org_buf_ptr;
	u32 epib[];
} __packed;

#define CPPI5_DESC_MIN_ALIGN			(16U)

#define CPPI5_INFO0_HDESC_EPIB_SIZE		(16U)
#define CPPI5_INFO0_HDESC_PSDATA_MAX_SIZE	(128U)

#define CPPI5_INFO0_HDESC_TYPE_SHIFT		(30U)
#define CPPI5_INFO0_HDESC_TYPE_MASK		GENMASK(31, 30)
#define   CPPI5_INFO0_DESC_TYPE_VAL_HOST	(1U)
#define   CPPI5_INFO0_DESC_TYPE_VAL_MONO	(2U)
#define   CPPI5_INFO0_DESC_TYPE_VAL_TR		(3U)
#define CPPI5_INFO0_HDESC_EPIB_PRESENT		BIT(29)

#define CPPI5_INFO0_HDESC_PSINFO_LOCATION	BIT(28)
#define CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT	(22U)
#define CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK	GENMASK(27, 22)
#define CPPI5_INFO0_HDESC_PKTLEN_SHIFT		(0)
#define CPPI5_INFO0_HDESC_PKTLEN_MASK		GENMASK(21, 0)

#define CPPI5_INFO1_DESC_PKTERROR_SHIFT		(28U)
#define CPPI5_INFO1_DESC_PKTERROR_MASK		GENMASK(31, 28)
#define CPPI5_INFO1_HDESC_PSFLGS_SHIFT		(24U)
#define CPPI5_INFO1_HDESC_PSFLGS_MASK		GENMASK(27, 24)
#define CPPI5_INFO1_DESC_PKTID_SHIFT		(14U)
#define CPPI5_INFO1_DESC_PKTID_MASK		GENMASK(23, 14)
#define CPPI5_INFO1_DESC_FLOWID_SHIFT		(0)
#define CPPI5_INFO1_DESC_FLOWID_MASK		GENMASK(13, 0)
#define CPPI5_INFO1_DESC_FLOWID_DEFAULT		CPPI5_INFO1_DESC_FLOWID_MASK

#define CPPI5_INFO2_HDESC_PKTTYPE_SHIFT		(27U)
#define CPPI5_INFO2_HDESC_PKTTYPE_MASK		GENMASK(31, 27)

#define CPPI5_INFO2_HDESC_RETPOLICY		BIT(18)

#define CPPI5_INFO2_HDESC_EARLYRET		BIT(17)

#define CPPI5_INFO2_DESC_RETPUSHPOLICY		BIT(16)
#define CPPI5_INFO2_DESC_RETP_MASK		GENMASK(18, 16)

#define CPPI5_INFO2_DESC_RETQ_SHIFT		(0)
#define CPPI5_INFO2_DESC_RETQ_MASK		GENMASK(15, 0)

#define CPPI5_INFO3_DESC_SRCTAG_SHIFT		(16U)
#define CPPI5_INFO3_DESC_SRCTAG_MASK		GENMASK(31, 16)
#define CPPI5_INFO3_DESC_DSTTAG_SHIFT		(0)
#define CPPI5_INFO3_DESC_DSTTAG_MASK		GENMASK(15, 0)

#define CPPI5_BUFINFO1_HDESC_DATA_LEN_SHIFT	(0)
#define CPPI5_BUFINFO1_HDESC_DATA_LEN_MASK	GENMASK(27, 0)

#define CPPI5_OBUFINFO0_HDESC_BUF_LEN_SHIFT	(0)
#define CPPI5_OBUFINFO0_HDESC_BUF_LEN_MASK	GENMASK(27, 0)


struct cppi5_desc_epib_t {
	u32 timestamp;	
	u32 sw_info0;	
	u32 sw_info1;	
	u32 sw_info2;	
};


struct cppi5_monolithic_desc_t {
	struct cppi5_desc_hdr_t hdr;
	u32 epib[];
};

#define CPPI5_INFO2_MDESC_DATA_OFFSET_SHIFT	(18U)
#define CPPI5_INFO2_MDESC_DATA_OFFSET_MASK	GENMASK(26, 18)


#define CPPI5_INFO0_TRDESC_RLDCNT_SHIFT		(20U)
#define CPPI5_INFO0_TRDESC_RLDCNT_MASK		GENMASK(28, 20)
#define CPPI5_INFO0_TRDESC_RLDCNT_MAX		(0x1ff)
#define CPPI5_INFO0_TRDESC_RLDCNT_INFINITE	CPPI5_INFO0_TRDESC_RLDCNT_MAX
#define CPPI5_INFO0_TRDESC_RLDIDX_SHIFT		(14U)
#define CPPI5_INFO0_TRDESC_RLDIDX_MASK		GENMASK(19, 14)
#define CPPI5_INFO0_TRDESC_RLDIDX_MAX		(0x3f)
#define CPPI5_INFO0_TRDESC_LASTIDX_SHIFT	(0)
#define CPPI5_INFO0_TRDESC_LASTIDX_MASK		GENMASK(13, 0)

#define CPPI5_INFO1_TRDESC_RECSIZE_SHIFT	(24U)
#define CPPI5_INFO1_TRDESC_RECSIZE_MASK		GENMASK(26, 24)
#define   CPPI5_INFO1_TRDESC_RECSIZE_VAL_16B	(0)
#define   CPPI5_INFO1_TRDESC_RECSIZE_VAL_32B	(1U)
#define   CPPI5_INFO1_TRDESC_RECSIZE_VAL_64B	(2U)
#define   CPPI5_INFO1_TRDESC_RECSIZE_VAL_128B	(3U)

static inline void cppi5_desc_dump(void *desc, u32 size)
{
	print_hex_dump(KERN_ERR, "dump udmap_desc: ", DUMP_PREFIX_NONE,
		       32, 4, desc, size, false);
}

#define CPPI5_TDCM_MARKER			(0x1)

static inline bool cppi5_desc_is_tdcm(dma_addr_t paddr)
{
	return (paddr & CPPI5_TDCM_MARKER) ? true : false;
}


static inline u32 cppi5_desc_get_type(struct cppi5_desc_hdr_t *desc_hdr)
{
	return (desc_hdr->pkt_info0 & CPPI5_INFO0_HDESC_TYPE_MASK) >>
		CPPI5_INFO0_HDESC_TYPE_SHIFT;
}


static inline u32 cppi5_desc_get_errflags(struct cppi5_desc_hdr_t *desc_hdr)
{
	return (desc_hdr->pkt_info1 & CPPI5_INFO1_DESC_PKTERROR_MASK) >>
		CPPI5_INFO1_DESC_PKTERROR_SHIFT;
}


static inline void cppi5_desc_get_pktids(struct cppi5_desc_hdr_t *desc_hdr,
					 u32 *pkt_id, u32 *flow_id)
{
	*pkt_id = (desc_hdr->pkt_info1 & CPPI5_INFO1_DESC_PKTID_MASK) >>
		   CPPI5_INFO1_DESC_PKTID_SHIFT;
	*flow_id = (desc_hdr->pkt_info1 & CPPI5_INFO1_DESC_FLOWID_MASK) >>
		    CPPI5_INFO1_DESC_FLOWID_SHIFT;
}


static inline void cppi5_desc_set_pktids(struct cppi5_desc_hdr_t *desc_hdr,
					 u32 pkt_id, u32 flow_id)
{
	desc_hdr->pkt_info1 &= ~(CPPI5_INFO1_DESC_PKTID_MASK |
				 CPPI5_INFO1_DESC_FLOWID_MASK);
	desc_hdr->pkt_info1 |= (pkt_id << CPPI5_INFO1_DESC_PKTID_SHIFT) &
				CPPI5_INFO1_DESC_PKTID_MASK;
	desc_hdr->pkt_info1 |= (flow_id << CPPI5_INFO1_DESC_FLOWID_SHIFT) &
				CPPI5_INFO1_DESC_FLOWID_MASK;
}


static inline void cppi5_desc_set_retpolicy(struct cppi5_desc_hdr_t *desc_hdr,
					    u32 flags, u32 return_ring_id)
{
	desc_hdr->pkt_info2 &= ~(CPPI5_INFO2_DESC_RETP_MASK |
				 CPPI5_INFO2_DESC_RETQ_MASK);
	desc_hdr->pkt_info2 |= flags & CPPI5_INFO2_DESC_RETP_MASK;
	desc_hdr->pkt_info2 |= return_ring_id & CPPI5_INFO2_DESC_RETQ_MASK;
}


static inline void cppi5_desc_get_tags_ids(struct cppi5_desc_hdr_t *desc_hdr,
					   u32 *src_tag_id, u32 *dst_tag_id)
{
	if (src_tag_id)
		*src_tag_id = (desc_hdr->src_dst_tag &
			      CPPI5_INFO3_DESC_SRCTAG_MASK) >>
			      CPPI5_INFO3_DESC_SRCTAG_SHIFT;
	if (dst_tag_id)
		*dst_tag_id = desc_hdr->src_dst_tag &
			      CPPI5_INFO3_DESC_DSTTAG_MASK;
}


static inline void cppi5_desc_set_tags_ids(struct cppi5_desc_hdr_t *desc_hdr,
					   u32 src_tag_id, u32 dst_tag_id)
{
	desc_hdr->src_dst_tag = (src_tag_id << CPPI5_INFO3_DESC_SRCTAG_SHIFT) &
				CPPI5_INFO3_DESC_SRCTAG_MASK;
	desc_hdr->src_dst_tag |= dst_tag_id & CPPI5_INFO3_DESC_DSTTAG_MASK;
}


static inline u32 cppi5_hdesc_calc_size(bool epib, u32 psdata_size,
					u32 sw_data_size)
{
	u32 desc_size;

	if (psdata_size > CPPI5_INFO0_HDESC_PSDATA_MAX_SIZE)
		return 0;

	desc_size = sizeof(struct cppi5_host_desc_t) + psdata_size +
		    sw_data_size;

	if (epib)
		desc_size += CPPI5_INFO0_HDESC_EPIB_SIZE;

	return ALIGN(desc_size, CPPI5_DESC_MIN_ALIGN);
}


static inline void cppi5_hdesc_init(struct cppi5_host_desc_t *desc, u32 flags,
				    u32 psdata_size)
{
	desc->hdr.pkt_info0 = (CPPI5_INFO0_DESC_TYPE_VAL_HOST <<
			       CPPI5_INFO0_HDESC_TYPE_SHIFT) | (flags);
	desc->hdr.pkt_info0 |= ((psdata_size >> 2) <<
				CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT) &
				CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK;
	desc->next_desc = 0;
}


static inline void cppi5_hdesc_update_flags(struct cppi5_host_desc_t *desc,
					    u32 flags)
{
	desc->hdr.pkt_info0 &= ~(CPPI5_INFO0_HDESC_EPIB_PRESENT |
				 CPPI5_INFO0_HDESC_PSINFO_LOCATION);
	desc->hdr.pkt_info0 |= flags;
}


static inline void
cppi5_hdesc_update_psdata_size(struct cppi5_host_desc_t *desc, u32 psdata_size)
{
	desc->hdr.pkt_info0 &= ~CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK;
	desc->hdr.pkt_info0 |= ((psdata_size >> 2) <<
				CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT) &
				CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK;
}


static inline u32 cppi5_hdesc_get_psdata_size(struct cppi5_host_desc_t *desc)
{
	u32 psdata_size = 0;

	if (!(desc->hdr.pkt_info0 & CPPI5_INFO0_HDESC_PSINFO_LOCATION))
		psdata_size = (desc->hdr.pkt_info0 &
			       CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK) >>
			       CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT;

	return (psdata_size << 2);
}


static inline u32 cppi5_hdesc_get_pktlen(struct cppi5_host_desc_t *desc)
{
	return (desc->hdr.pkt_info0 & CPPI5_INFO0_HDESC_PKTLEN_MASK);
}


static inline void cppi5_hdesc_set_pktlen(struct cppi5_host_desc_t *desc,
					  u32 pkt_len)
{
	desc->hdr.pkt_info0 &= ~CPPI5_INFO0_HDESC_PKTLEN_MASK;
	desc->hdr.pkt_info0 |= (pkt_len & CPPI5_INFO0_HDESC_PKTLEN_MASK);
}


static inline u32 cppi5_hdesc_get_psflags(struct cppi5_host_desc_t *desc)
{
	return (desc->hdr.pkt_info1 & CPPI5_INFO1_HDESC_PSFLGS_MASK) >>
		CPPI5_INFO1_HDESC_PSFLGS_SHIFT;
}


static inline void cppi5_hdesc_set_psflags(struct cppi5_host_desc_t *desc,
					   u32 ps_flags)
{
	desc->hdr.pkt_info1 &= ~CPPI5_INFO1_HDESC_PSFLGS_MASK;
	desc->hdr.pkt_info1 |= (ps_flags <<
				CPPI5_INFO1_HDESC_PSFLGS_SHIFT) &
				CPPI5_INFO1_HDESC_PSFLGS_MASK;
}


static inline u32 cppi5_hdesc_get_pkttype(struct cppi5_host_desc_t *desc)
{
	return (desc->hdr.pkt_info2 & CPPI5_INFO2_HDESC_PKTTYPE_MASK) >>
		CPPI5_INFO2_HDESC_PKTTYPE_SHIFT;
}


static inline void cppi5_hdesc_set_pkttype(struct cppi5_host_desc_t *desc,
					   u32 pkt_type)
{
	desc->hdr.pkt_info2 &= ~CPPI5_INFO2_HDESC_PKTTYPE_MASK;
	desc->hdr.pkt_info2 |=
			(pkt_type << CPPI5_INFO2_HDESC_PKTTYPE_SHIFT) &
			 CPPI5_INFO2_HDESC_PKTTYPE_MASK;
}


static inline void cppi5_hdesc_attach_buf(struct cppi5_host_desc_t *desc,
					  dma_addr_t buf, u32 buf_data_len,
					  dma_addr_t obuf, u32 obuf_len)
{
	desc->buf_ptr = buf;
	desc->buf_info1 = buf_data_len & CPPI5_BUFINFO1_HDESC_DATA_LEN_MASK;
	desc->org_buf_ptr = obuf;
	desc->org_buf_len = obuf_len & CPPI5_OBUFINFO0_HDESC_BUF_LEN_MASK;
}

static inline void cppi5_hdesc_get_obuf(struct cppi5_host_desc_t *desc,
					dma_addr_t *obuf, u32 *obuf_len)
{
	*obuf = desc->org_buf_ptr;
	*obuf_len = desc->org_buf_len & CPPI5_OBUFINFO0_HDESC_BUF_LEN_MASK;
}

static inline void cppi5_hdesc_reset_to_original(struct cppi5_host_desc_t *desc)
{
	desc->buf_ptr = desc->org_buf_ptr;
	desc->buf_info1 = desc->org_buf_len;
}


static inline void cppi5_hdesc_link_hbdesc(struct cppi5_host_desc_t *desc,
					   dma_addr_t hbuf_desc)
{
	desc->next_desc = hbuf_desc;
}

static inline dma_addr_t
cppi5_hdesc_get_next_hbdesc(struct cppi5_host_desc_t *desc)
{
	return (dma_addr_t)desc->next_desc;
}

static inline void cppi5_hdesc_reset_hbdesc(struct cppi5_host_desc_t *desc)
{
	desc->hdr = (struct cppi5_desc_hdr_t) { 0 };
	desc->next_desc = 0;
}


static inline bool cppi5_hdesc_epib_present(struct cppi5_desc_hdr_t *desc_hdr)
{
	return !!(desc_hdr->pkt_info0 & CPPI5_INFO0_HDESC_EPIB_PRESENT);
}


static inline void *cppi5_hdesc_get_psdata(struct cppi5_host_desc_t *desc)
{
	u32 psdata_size;
	void *psdata;

	if (desc->hdr.pkt_info0 & CPPI5_INFO0_HDESC_PSINFO_LOCATION)
		return NULL;

	psdata_size = (desc->hdr.pkt_info0 &
		       CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK) >>
		       CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT;

	if (!psdata_size)
		return NULL;

	psdata = &desc->epib;

	if (cppi5_hdesc_epib_present(&desc->hdr))
		psdata += CPPI5_INFO0_HDESC_EPIB_SIZE;

	return psdata;
}


static inline void *cppi5_hdesc_get_swdata(struct cppi5_host_desc_t *desc)
{
	u32 psdata_size = 0;
	void *swdata;

	if (!(desc->hdr.pkt_info0 & CPPI5_INFO0_HDESC_PSINFO_LOCATION))
		psdata_size = (desc->hdr.pkt_info0 &
			       CPPI5_INFO0_HDESC_PSINFO_SIZE_MASK) >>
			       CPPI5_INFO0_HDESC_PSINFO_SIZE_SHIFT;

	swdata = &desc->epib;

	if (cppi5_hdesc_epib_present(&desc->hdr))
		swdata += CPPI5_INFO0_HDESC_EPIB_SIZE;

	swdata += (psdata_size << 2);

	return swdata;
}



#define CPPI5_TR_TYPE_SHIFT			(0U)
#define CPPI5_TR_TYPE_MASK			GENMASK(3, 0)
#define CPPI5_TR_STATIC				BIT(4)
#define CPPI5_TR_WAIT				BIT(5)
#define CPPI5_TR_EVENT_SIZE_SHIFT		(6U)
#define CPPI5_TR_EVENT_SIZE_MASK		GENMASK(7, 6)
#define CPPI5_TR_TRIGGER0_SHIFT			(8U)
#define CPPI5_TR_TRIGGER0_MASK			GENMASK(9, 8)
#define CPPI5_TR_TRIGGER0_TYPE_SHIFT		(10U)
#define CPPI5_TR_TRIGGER0_TYPE_MASK		GENMASK(11, 10)
#define CPPI5_TR_TRIGGER1_SHIFT			(12U)
#define CPPI5_TR_TRIGGER1_MASK			GENMASK(13, 12)
#define CPPI5_TR_TRIGGER1_TYPE_SHIFT		(14U)
#define CPPI5_TR_TRIGGER1_TYPE_MASK		GENMASK(15, 14)
#define CPPI5_TR_CMD_ID_SHIFT			(16U)
#define CPPI5_TR_CMD_ID_MASK			GENMASK(23, 16)
#define CPPI5_TR_CSF_FLAGS_SHIFT		(24U)
#define CPPI5_TR_CSF_FLAGS_MASK			GENMASK(31, 24)
#define   CPPI5_TR_CSF_SA_INDIRECT		BIT(0)
#define   CPPI5_TR_CSF_DA_INDIRECT		BIT(1)
#define   CPPI5_TR_CSF_SUPR_EVT			BIT(2)
#define   CPPI5_TR_CSF_EOL_ADV_SHIFT		(4U)
#define   CPPI5_TR_CSF_EOL_ADV_MASK		GENMASK(6, 4)
#define   CPPI5_TR_CSF_EOL_ICNT0		BIT(4)
#define   CPPI5_TR_CSF_EOP			BIT(7)


enum cppi5_tr_types {
	CPPI5_TR_TYPE0 = 0,
	CPPI5_TR_TYPE1,
	CPPI5_TR_TYPE2,
	CPPI5_TR_TYPE3,
	CPPI5_TR_TYPE4,
	CPPI5_TR_TYPE5,
	
	CPPI5_TR_TYPE8 = 8,
	CPPI5_TR_TYPE9,
	CPPI5_TR_TYPE10,
	CPPI5_TR_TYPE11,
	
	CPPI5_TR_TYPE15 = 15,
	CPPI5_TR_TYPE_MAX
};


enum cppi5_tr_event_size {
	CPPI5_TR_EVENT_SIZE_COMPLETION,
	CPPI5_TR_EVENT_SIZE_ICNT1_DEC,
	CPPI5_TR_EVENT_SIZE_ICNT2_DEC,
	CPPI5_TR_EVENT_SIZE_ICNT3_DEC,
	CPPI5_TR_EVENT_SIZE_MAX
};


enum cppi5_tr_trigger {
	CPPI5_TR_TRIGGER_NONE,
	CPPI5_TR_TRIGGER_GLOBAL0,
	CPPI5_TR_TRIGGER_GLOBAL1,
	CPPI5_TR_TRIGGER_LOCAL_EVENT,
	CPPI5_TR_TRIGGER_MAX
};


enum cppi5_tr_trigger_type {
	CPPI5_TR_TRIGGER_TYPE_ICNT1_DEC,
	CPPI5_TR_TRIGGER_TYPE_ICNT2_DEC,
	CPPI5_TR_TRIGGER_TYPE_ICNT3_DEC,
	CPPI5_TR_TRIGGER_TYPE_ALL,
	CPPI5_TR_TRIGGER_TYPE_MAX
};

typedef u32 cppi5_tr_flags_t;


struct cppi5_tr_type0_t {
	cppi5_tr_flags_t flags;
	u16 icnt0;
	u16 _reserved;
	u64 addr;
} __aligned(16) __packed;


struct cppi5_tr_type1_t {
	cppi5_tr_flags_t flags;
	u16 icnt0;
	u16 icnt1;
	u64 addr;
	s32 dim1;
} __aligned(32) __packed;


struct cppi5_tr_type2_t {
	cppi5_tr_flags_t flags;
	u16 icnt0;
	u16 icnt1;
	u64 addr;
	s32 dim1;
	u16 icnt2;
	u16 _reserved;
	s32 dim2;
} __aligned(32) __packed;


struct cppi5_tr_type3_t {
	cppi5_tr_flags_t flags;
	u16 icnt0;
	u16 icnt1;
	u64 addr;
	s32 dim1;
	u16 icnt2;
	u16 icnt3;
	s32 dim2;
	s32 dim3;
} __aligned(32) __packed;


struct cppi5_tr_type15_t {
	cppi5_tr_flags_t flags;
	u16 icnt0;
	u16 icnt1;
	u64 addr;
	s32 dim1;
	u16 icnt2;
	u16 icnt3;
	s32 dim2;
	s32 dim3;
	u32 _reserved;
	s32 ddim1;
	u64 daddr;
	s32 ddim2;
	s32 ddim3;
	u16 dicnt0;
	u16 dicnt1;
	u16 dicnt2;
	u16 dicnt3;
} __aligned(64) __packed;


struct cppi5_tr_resp_t {
	u8 status;
	u8 _reserved;
	u8 cmd_id;
	u8 flags;
} __packed;

#define CPPI5_TR_RESPONSE_STATUS_TYPE_SHIFT	(0U)
#define CPPI5_TR_RESPONSE_STATUS_TYPE_MASK	GENMASK(3, 0)
#define CPPI5_TR_RESPONSE_STATUS_INFO_SHIFT	(4U)
#define CPPI5_TR_RESPONSE_STATUS_INFO_MASK	GENMASK(7, 4)
#define CPPI5_TR_RESPONSE_CMDID_SHIFT		(16U)
#define CPPI5_TR_RESPONSE_CMDID_MASK		GENMASK(23, 16)
#define CPPI5_TR_RESPONSE_CFG_SPECIFIC_SHIFT	(24U)
#define CPPI5_TR_RESPONSE_CFG_SPECIFIC_MASK	GENMASK(31, 24)


enum cppi5_tr_resp_status_type {
	CPPI5_TR_RESPONSE_STATUS_NONE,
	CPPI5_TR_RESPONSE_STATUS_TRANSFER_ERR,
	CPPI5_TR_RESPONSE_STATUS_ABORTED_ERR,
	CPPI5_TR_RESPONSE_STATUS_SUBMISSION_ERR,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_ERR,
	CPPI5_TR_RESPONSE_STATUS_TRANSFER_EXCEPTION,
	CPPI5_TR_RESPONSE_STATUS__TEARDOWN_FLUSH,
	CPPI5_TR_RESPONSE_STATUS_MAX
};


enum cppi5_tr_resp_status_submission {
	CPPI5_TR_RESPONSE_STATUS_SUBMISSION_ICNT0,
	CPPI5_TR_RESPONSE_STATUS_SUBMISSION_FIFO_FULL,
	CPPI5_TR_RESPONSE_STATUS_SUBMISSION_OWN,
	CPPI5_TR_RESPONSE_STATUS_SUBMISSION_MAX
};


enum cppi5_tr_resp_status_unsupported {
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_TR_TYPE,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_STATIC,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_EOL,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_CFG_SPECIFIC,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_AMODE,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_ELTYPE,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_DFMT,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_SECTR,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_AMODE_SPECIFIC,
	CPPI5_TR_RESPONSE_STATUS_UNSUPPORTED_MAX
};


static inline size_t cppi5_trdesc_calc_size(u32 tr_count, u32 tr_size)
{
	
	return tr_size * (tr_count + 1) +
		sizeof(struct cppi5_tr_resp_t) * tr_count;
}


static inline void cppi5_trdesc_init(struct cppi5_desc_hdr_t *desc_hdr,
				     u32 tr_count, u32 tr_size, u32 reload_idx,
				     u32 reload_count)
{
	desc_hdr->pkt_info0 = CPPI5_INFO0_DESC_TYPE_VAL_TR <<
			      CPPI5_INFO0_HDESC_TYPE_SHIFT;
	desc_hdr->pkt_info0 |=
			(reload_count << CPPI5_INFO0_TRDESC_RLDCNT_SHIFT) &
			CPPI5_INFO0_TRDESC_RLDCNT_MASK;
	desc_hdr->pkt_info0 |=
			(reload_idx << CPPI5_INFO0_TRDESC_RLDIDX_SHIFT) &
			CPPI5_INFO0_TRDESC_RLDIDX_MASK;
	desc_hdr->pkt_info0 |= (tr_count - 1) & CPPI5_INFO0_TRDESC_LASTIDX_MASK;

	desc_hdr->pkt_info1 |= ((ffs(tr_size >> 4) - 1) <<
				CPPI5_INFO1_TRDESC_RECSIZE_SHIFT) &
				CPPI5_INFO1_TRDESC_RECSIZE_MASK;
}


static inline void cppi5_tr_init(cppi5_tr_flags_t *flags,
				 enum cppi5_tr_types type, bool static_tr,
				 bool wait, enum cppi5_tr_event_size event_size,
				 u32 cmd_id)
{
	*flags = type;
	*flags |= (event_size << CPPI5_TR_EVENT_SIZE_SHIFT) &
		  CPPI5_TR_EVENT_SIZE_MASK;

	*flags |= (cmd_id << CPPI5_TR_CMD_ID_SHIFT) &
		  CPPI5_TR_CMD_ID_MASK;

	if (static_tr && (type == CPPI5_TR_TYPE8 || type == CPPI5_TR_TYPE9))
		*flags |= CPPI5_TR_STATIC;

	if (wait)
		*flags |= CPPI5_TR_WAIT;
}


static inline void cppi5_tr_set_trigger(cppi5_tr_flags_t *flags,
		enum cppi5_tr_trigger trigger0,
		enum cppi5_tr_trigger_type trigger0_type,
		enum cppi5_tr_trigger trigger1,
		enum cppi5_tr_trigger_type trigger1_type)
{
	*flags &= ~(CPPI5_TR_TRIGGER0_MASK | CPPI5_TR_TRIGGER0_TYPE_MASK |
		    CPPI5_TR_TRIGGER1_MASK | CPPI5_TR_TRIGGER1_TYPE_MASK);
	*flags |= (trigger0 << CPPI5_TR_TRIGGER0_SHIFT) &
		  CPPI5_TR_TRIGGER0_MASK;
	*flags |= (trigger0_type << CPPI5_TR_TRIGGER0_TYPE_SHIFT) &
		  CPPI5_TR_TRIGGER0_TYPE_MASK;

	*flags |= (trigger1 << CPPI5_TR_TRIGGER1_SHIFT) &
		  CPPI5_TR_TRIGGER1_MASK;
	*flags |= (trigger1_type << CPPI5_TR_TRIGGER1_TYPE_SHIFT) &
		  CPPI5_TR_TRIGGER1_TYPE_MASK;
}


static inline void cppi5_tr_csf_set(cppi5_tr_flags_t *flags, u32 csf)
{
	*flags &= ~CPPI5_TR_CSF_FLAGS_MASK;
	*flags |= (csf << CPPI5_TR_CSF_FLAGS_SHIFT) &
		  CPPI5_TR_CSF_FLAGS_MASK;
}

#endif 
