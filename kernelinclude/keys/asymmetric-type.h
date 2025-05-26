/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _KEYS_ASYMMETRIC_TYPE_H
#define _KEYS_ASYMMETRIC_TYPE_H

#include <linux/key-type.h>
#include <linux/verification.h>

extern struct key_type key_type_asymmetric;


enum asymmetric_payload_bits {
	asym_crypto,		
	asym_subtype,		
	asym_key_ids,		
	asym_auth		
};


struct asymmetric_key_id {
	unsigned short	len;
	unsigned char	data[];
};

struct asymmetric_key_ids {
	void		*id[3];
};

extern bool asymmetric_key_id_same(const struct asymmetric_key_id *kid1,
				   const struct asymmetric_key_id *kid2);

extern bool asymmetric_key_id_partial(const struct asymmetric_key_id *kid1,
				      const struct asymmetric_key_id *kid2);

extern struct asymmetric_key_id *asymmetric_key_generate_id(const void *val_1,
							    size_t len_1,
							    const void *val_2,
							    size_t len_2);
static inline
const struct asymmetric_key_ids *asymmetric_key_ids(const struct key *key)
{
	return key->payload.data[asym_key_ids];
}

static inline
const struct public_key *asymmetric_key_public_key(const struct key *key)
{
	return key->payload.data[asym_crypto];
}

extern struct key *find_asymmetric_key(struct key *keyring,
				       const struct asymmetric_key_id *id_0,
				       const struct asymmetric_key_id *id_1,
				       const struct asymmetric_key_id *id_2,
				       bool partial);

int x509_load_certificate_list(const u8 cert_list[], const unsigned long list_size,
			       const struct key *keyring);



#endif 
