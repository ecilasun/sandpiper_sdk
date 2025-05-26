/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_KEY_TYPE_H
#define _LINUX_KEY_TYPE_H

#include <linux/key.h>
#include <linux/errno.h>

#ifdef CONFIG_KEYS

struct kernel_pkey_query;
struct kernel_pkey_params;


struct key_preparsed_payload {
	const char	*orig_description; 
	char		*description;	
	union key_payload payload;	
	const void	*data;		
	size_t		datalen;	
	size_t		quotalen;	
	time64_t	expiry;		
} __randomize_layout;

typedef int (*request_key_actor_t)(struct key *auth_key, void *aux);


struct key_match_data {
	
	bool (*cmp)(const struct key *key,
		    const struct key_match_data *match_data);

	const void	*raw_data;	
	void		*preparsed;	
	unsigned	lookup_type;	
#define KEYRING_SEARCH_LOOKUP_DIRECT	0x0000	
#define KEYRING_SEARCH_LOOKUP_ITERATE	0x0001	
};


struct key_type {
	
	const char *name;

	
	size_t def_datalen;

	unsigned int flags;
#define KEY_TYPE_NET_DOMAIN	0x00000001 
#define KEY_TYPE_INSTANT_REAP	0x00000002 

	
	int (*vet_description)(const char *description);

	
	int (*preparse)(struct key_preparsed_payload *prep);

	
	void (*free_preparse)(struct key_preparsed_payload *prep);

	
	int (*instantiate)(struct key *key, struct key_preparsed_payload *prep);

	
	int (*update)(struct key *key, struct key_preparsed_payload *prep);

	
	int (*match_preparse)(struct key_match_data *match_data);

	
	void (*match_free)(struct key_match_data *match_data);

	
	void (*revoke)(struct key *key);

	
	void (*destroy)(struct key *key);

	
	void (*describe)(const struct key *key, struct seq_file *p);

	
	long (*read)(const struct key *key, char *buffer, size_t buflen);

	
	request_key_actor_t request_key;

	
	struct key_restriction *(*lookup_restriction)(const char *params);

	
	int (*asym_query)(const struct kernel_pkey_params *params,
			  struct kernel_pkey_query *info);
	int (*asym_eds_op)(struct kernel_pkey_params *params,
			   const void *in, void *out);
	int (*asym_verify_signature)(struct kernel_pkey_params *params,
				     const void *in, const void *in2);

	
	struct list_head	link;		
	struct lock_class_key	lock_class;	
} __randomize_layout;

extern struct key_type key_type_keyring;

extern int register_key_type(struct key_type *ktype);
extern void unregister_key_type(struct key_type *ktype);

extern int key_payload_reserve(struct key *key, size_t datalen);
extern int key_instantiate_and_link(struct key *key,
				    const void *data,
				    size_t datalen,
				    struct key *keyring,
				    struct key *authkey);
extern int key_reject_and_link(struct key *key,
			       unsigned timeout,
			       unsigned error,
			       struct key *keyring,
			       struct key *authkey);
extern void complete_request_key(struct key *authkey, int error);

static inline int key_negate_and_link(struct key *key,
				      unsigned timeout,
				      struct key *keyring,
				      struct key *authkey)
{
	return key_reject_and_link(key, timeout, ENOKEY, keyring, authkey);
}

extern int generic_key_instantiate(struct key *key, struct key_preparsed_payload *prep);

#endif 
#endif 
