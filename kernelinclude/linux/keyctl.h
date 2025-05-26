/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __LINUX_KEYCTL_H
#define __LINUX_KEYCTL_H

#include <uapi/linux/keyctl.h>

struct kernel_pkey_query {
	__u32		supported_ops;	
	__u32		key_size;	
	__u16		max_data_size;	
	__u16		max_sig_size;	
	__u16		max_enc_size;	
	__u16		max_dec_size;	
};

enum kernel_pkey_operation {
	kernel_pkey_encrypt,
	kernel_pkey_decrypt,
	kernel_pkey_sign,
	kernel_pkey_verify,
};

struct kernel_pkey_params {
	struct key	*key;
	const char	*encoding;	
	const char	*hash_algo;	
	char		*info;		
	__u32		in_len;		
	union {
		__u32	out_len;	
		__u32	in2_len;	
	};
	enum kernel_pkey_operation op : 8;
};

#endif 
