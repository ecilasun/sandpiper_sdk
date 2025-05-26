/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __CAAM_BLOB_GEN
#define __CAAM_BLOB_GEN

#include <linux/types.h>
#include <linux/errno.h>

#define CAAM_BLOB_KEYMOD_LENGTH		16
#define CAAM_BLOB_OVERHEAD		(32 + 16)
#define CAAM_BLOB_MAX_LEN		4096

struct caam_blob_priv;


struct caam_blob_info {
	void *input;
	size_t input_len;

	void *output;
	size_t output_len;

	const void *key_mod;
	size_t key_mod_len;
};


struct caam_blob_priv *caam_blob_gen_init(void);


void caam_blob_gen_exit(struct caam_blob_priv *priv);


int caam_process_blob(struct caam_blob_priv *priv,
		      struct caam_blob_info *info, bool encap);


static inline int caam_encap_blob(struct caam_blob_priv *priv,
				  struct caam_blob_info *info)
{
	if (info->output_len < info->input_len + CAAM_BLOB_OVERHEAD)
		return -EINVAL;

	return caam_process_blob(priv, info, true);
}


static inline int caam_decap_blob(struct caam_blob_priv *priv,
				  struct caam_blob_info *info)
{
	if (info->input_len < CAAM_BLOB_OVERHEAD ||
	    info->output_len < info->input_len - CAAM_BLOB_OVERHEAD)
		return -EINVAL;

	return caam_process_blob(priv, info, false);
}

#endif
