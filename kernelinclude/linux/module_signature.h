/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_MODULE_SIGNATURE_H
#define _LINUX_MODULE_SIGNATURE_H

#include <linux/types.h>


#define MODULE_SIG_STRING "~Module signature appended~\n"

enum pkey_id_type {
	PKEY_ID_PGP,		
	PKEY_ID_X509,		
	PKEY_ID_PKCS7,		
};


struct module_signature {
	u8	algo;		
	u8	hash;		
	u8	id_type;	
	u8	signer_len;	
	u8	key_id_len;	
	u8	__pad[3];
	__be32	sig_len;	
};

int mod_check_sig(const struct module_signature *ms, size_t file_len,
		  const char *name);

#endif 
