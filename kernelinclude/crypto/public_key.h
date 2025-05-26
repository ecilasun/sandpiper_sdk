/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_PUBLIC_KEY_H
#define _LINUX_PUBLIC_KEY_H

#include <linux/keyctl.h>
#include <linux/oid_registry.h>


struct public_key {
	void *key;
	u32 keylen;
	enum OID algo;
	void *params;
	u32 paramlen;
	bool key_is_private;
	const char *id_type;
	const char *pkey_algo;
	unsigned long key_eflags;	
#define KEY_EFLAG_CA		0	
#define KEY_EFLAG_DIGITALSIG	1	
#define KEY_EFLAG_KEYCERTSIGN	2	
};

extern void public_key_free(struct public_key *key);


struct public_key_signature {
	struct asymmetric_key_id *auth_ids[3];
	u8 *s;			
	u8 *digest;
	u32 s_size;		
	u32 digest_size;	
	const char *pkey_algo;
	const char *hash_algo;
	const char *encoding;
};

extern void public_key_signature_free(struct public_key_signature *sig);

extern struct asymmetric_key_subtype public_key_subtype;

struct key;
struct key_type;
union key_payload;

extern int restrict_link_by_signature(struct key *dest_keyring,
				      const struct key_type *type,
				      const union key_payload *payload,
				      struct key *trust_keyring);

extern int restrict_link_by_key_or_keyring(struct key *dest_keyring,
					   const struct key_type *type,
					   const union key_payload *payload,
					   struct key *trusted);

extern int restrict_link_by_key_or_keyring_chain(struct key *trust_keyring,
						 const struct key_type *type,
						 const union key_payload *payload,
						 struct key *trusted);

#if IS_REACHABLE(CONFIG_ASYMMETRIC_KEY_TYPE)
extern int restrict_link_by_ca(struct key *dest_keyring,
			       const struct key_type *type,
			       const union key_payload *payload,
			       struct key *trust_keyring);
int restrict_link_by_digsig(struct key *dest_keyring,
			    const struct key_type *type,
			    const union key_payload *payload,
			    struct key *trust_keyring);
#else
static inline int restrict_link_by_ca(struct key *dest_keyring,
				      const struct key_type *type,
				      const union key_payload *payload,
				      struct key *trust_keyring)
{
	return 0;
}

static inline int restrict_link_by_digsig(struct key *dest_keyring,
					  const struct key_type *type,
					  const union key_payload *payload,
					  struct key *trust_keyring)
{
	return 0;
}
#endif

extern int query_asymmetric_key(const struct kernel_pkey_params *,
				struct kernel_pkey_query *);

extern int encrypt_blob(struct kernel_pkey_params *, const void *, void *);
extern int decrypt_blob(struct kernel_pkey_params *, const void *, void *);
extern int create_signature(struct kernel_pkey_params *, const void *, void *);
extern int verify_signature(const struct key *,
			    const struct public_key_signature *);

#if IS_REACHABLE(CONFIG_ASYMMETRIC_PUBLIC_KEY_SUBTYPE)
int public_key_verify_signature(const struct public_key *pkey,
				const struct public_key_signature *sig);
#else
static inline
int public_key_verify_signature(const struct public_key *pkey,
				const struct public_key_signature *sig)
{
	return -EINVAL;
}
#endif

#endif 
