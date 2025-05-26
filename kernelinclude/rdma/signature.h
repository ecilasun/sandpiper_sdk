/* SPDX-License-Identifier: (GPL-2.0 OR Linux-OpenIB) */


#ifndef _RDMA_SIGNATURE_H_
#define _RDMA_SIGNATURE_H_

#include <linux/types.h>

enum ib_signature_prot_cap {
	IB_PROT_T10DIF_TYPE_1 = 1,
	IB_PROT_T10DIF_TYPE_2 = 1 << 1,
	IB_PROT_T10DIF_TYPE_3 = 1 << 2,
};

enum ib_signature_guard_cap {
	IB_GUARD_T10DIF_CRC	= 1,
	IB_GUARD_T10DIF_CSUM	= 1 << 1,
};


enum ib_signature_type {
	IB_SIG_TYPE_NONE,
	IB_SIG_TYPE_T10_DIF,
};


enum ib_t10_dif_bg_type {
	IB_T10DIF_CRC,
	IB_T10DIF_CSUM,
};


struct ib_t10_dif_domain {
	enum ib_t10_dif_bg_type bg_type;
	u16			pi_interval;
	u16			bg;
	u16			app_tag;
	u32			ref_tag;
	bool			ref_remap;
	bool			app_escape;
	bool			ref_escape;
	u16			apptag_check_mask;
};


struct ib_sig_domain {
	enum ib_signature_type sig_type;
	union {
		struct ib_t10_dif_domain dif;
	} sig;
};


struct ib_sig_attrs {
	u8			check_mask;
	struct ib_sig_domain	mem;
	struct ib_sig_domain	wire;
	int			meta_length;
};

enum ib_sig_err_type {
	IB_SIG_BAD_GUARD,
	IB_SIG_BAD_REFTAG,
	IB_SIG_BAD_APPTAG,
};


enum {
	IB_SIG_CHECK_GUARD = 0xc0,
	IB_SIG_CHECK_APPTAG = 0x30,
	IB_SIG_CHECK_REFTAG = 0x0f,
};


struct ib_sig_err {
	enum ib_sig_err_type	err_type;
	u32			expected;
	u32			actual;
	u64			sig_err_offset;
	u32			key;
};

#endif 
