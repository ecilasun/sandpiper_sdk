/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _VIVALDI_FMAP_H
#define _VIVALDI_FMAP_H

#include <linux/types.h>

#define VIVALDI_MAX_FUNCTION_ROW_KEYS	24


struct vivaldi_data {
	u32 function_row_physmap[VIVALDI_MAX_FUNCTION_ROW_KEYS];
	unsigned int num_function_row_keys;
};

ssize_t vivaldi_function_row_physmap_show(const struct vivaldi_data *data,
					  char *buf);

#endif 
