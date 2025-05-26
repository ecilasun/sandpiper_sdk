/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_FS_PARSER_H
#define _LINUX_FS_PARSER_H

#include <linux/fs_context.h>

struct path;

struct constant_table {
	const char	*name;
	int		value;
};

struct fs_parameter_spec;
struct fs_parse_result;
typedef int fs_param_type(struct p_log *,
			  const struct fs_parameter_spec *,
			  struct fs_parameter *,
			  struct fs_parse_result *);

fs_param_type fs_param_is_bool, fs_param_is_u32, fs_param_is_s32, fs_param_is_u64,
	fs_param_is_enum, fs_param_is_string, fs_param_is_blob, fs_param_is_blockdev,
	fs_param_is_path, fs_param_is_fd;


struct fs_parameter_spec {
	const char		*name;
	fs_param_type		*type;	
	u8			opt;	
	unsigned short		flags;
#define fs_param_neg_with_no	0x0002	
#define fs_param_can_be_empty	0x0004	
#define fs_param_deprecated	0x0008	
	const void		*data;
};


struct fs_parse_result {
	bool			negated;	
	union {
		bool		boolean;	
		int		int_32;		
		unsigned int	uint_32;	
		u64		uint_64;	
	};
};

extern int __fs_parse(struct p_log *log,
		    const struct fs_parameter_spec *desc,
		    struct fs_parameter *value,
		    struct fs_parse_result *result);

static inline int fs_parse(struct fs_context *fc,
	     const struct fs_parameter_spec *desc,
	     struct fs_parameter *param,
	     struct fs_parse_result *result)
{
	return __fs_parse(&fc->log, desc, param, result);
}

extern int fs_lookup_param(struct fs_context *fc,
			   struct fs_parameter *param,
			   bool want_bdev,
			   unsigned int flags,
			   struct path *_path);

extern int lookup_constant(const struct constant_table tbl[], const char *name, int not_found);

#ifdef CONFIG_VALIDATE_FS_PARSER
extern bool validate_constant_table(const struct constant_table *tbl, size_t tbl_size,
				    int low, int high, int special);
extern bool fs_validate_description(const char *name,
				    const struct fs_parameter_spec *desc);
#else
static inline bool validate_constant_table(const struct constant_table *tbl, size_t tbl_size,
					   int low, int high, int special)
{ return true; }
static inline bool fs_validate_description(const char *name,
					   const struct fs_parameter_spec *desc)
{ return true; }
#endif


#define __fsparam(TYPE, NAME, OPT, FLAGS, DATA) \
	{ \
		.name = NAME, \
		.opt = OPT, \
		.type = TYPE, \
		.flags = FLAGS, \
		.data = DATA \
	}

#define fsparam_flag(NAME, OPT)	__fsparam(NULL, NAME, OPT, 0, NULL)
#define fsparam_flag_no(NAME, OPT) \
			__fsparam(NULL, NAME, OPT, fs_param_neg_with_no, NULL)
#define fsparam_bool(NAME, OPT)	__fsparam(fs_param_is_bool, NAME, OPT, 0, NULL)
#define fsparam_u32(NAME, OPT)	__fsparam(fs_param_is_u32, NAME, OPT, 0, NULL)
#define fsparam_u32oct(NAME, OPT) \
			__fsparam(fs_param_is_u32, NAME, OPT, 0, (void *)8)
#define fsparam_u32hex(NAME, OPT) \
			__fsparam(fs_param_is_u32_hex, NAME, OPT, 0, (void *)16)
#define fsparam_s32(NAME, OPT)	__fsparam(fs_param_is_s32, NAME, OPT, 0, NULL)
#define fsparam_u64(NAME, OPT)	__fsparam(fs_param_is_u64, NAME, OPT, 0, NULL)
#define fsparam_enum(NAME, OPT, array)	__fsparam(fs_param_is_enum, NAME, OPT, 0, array)
#define fsparam_string(NAME, OPT) \
				__fsparam(fs_param_is_string, NAME, OPT, 0, NULL)
#define fsparam_blob(NAME, OPT)	__fsparam(fs_param_is_blob, NAME, OPT, 0, NULL)
#define fsparam_bdev(NAME, OPT)	__fsparam(fs_param_is_blockdev, NAME, OPT, 0, NULL)
#define fsparam_path(NAME, OPT)	__fsparam(fs_param_is_path, NAME, OPT, 0, NULL)
#define fsparam_fd(NAME, OPT)	__fsparam(fs_param_is_fd, NAME, OPT, 0, NULL)

#endif 
