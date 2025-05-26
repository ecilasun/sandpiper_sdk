/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _POSIX_ACL_XATTR_H
#define _POSIX_ACL_XATTR_H

#include <uapi/linux/xattr.h>
#include <uapi/linux/posix_acl_xattr.h>
#include <linux/posix_acl.h>

static inline size_t
posix_acl_xattr_size(int count)
{
	return (sizeof(struct posix_acl_xattr_header) +
		(count * sizeof(struct posix_acl_xattr_entry)));
}

static inline int
posix_acl_xattr_count(size_t size)
{
	if (size < sizeof(struct posix_acl_xattr_header))
		return -1;
	size -= sizeof(struct posix_acl_xattr_header);
	if (size % sizeof(struct posix_acl_xattr_entry))
		return -1;
	return size / sizeof(struct posix_acl_xattr_entry);
}

#ifdef CONFIG_FS_POSIX_ACL
struct posix_acl *posix_acl_from_xattr(struct user_namespace *user_ns,
				       const void *value, size_t size);
#else
static inline struct posix_acl *
posix_acl_from_xattr(struct user_namespace *user_ns, const void *value,
		     size_t size)
{
	return ERR_PTR(-EOPNOTSUPP);
}
#endif

int posix_acl_to_xattr(struct user_namespace *user_ns,
		       const struct posix_acl *acl, void *buffer, size_t size);
static inline const char *posix_acl_xattr_name(int type)
{
	switch (type) {
	case ACL_TYPE_ACCESS:
		return XATTR_NAME_POSIX_ACL_ACCESS;
	case ACL_TYPE_DEFAULT:
		return XATTR_NAME_POSIX_ACL_DEFAULT;
	}

	return "";
}

static inline int posix_acl_type(const char *name)
{
	if (strcmp(name, XATTR_NAME_POSIX_ACL_ACCESS) == 0)
		return ACL_TYPE_ACCESS;
	else if (strcmp(name, XATTR_NAME_POSIX_ACL_DEFAULT) == 0)
		return ACL_TYPE_DEFAULT;

	return -1;
}


extern const struct xattr_handler nop_posix_acl_access;
extern const struct xattr_handler nop_posix_acl_default;

#endif	
