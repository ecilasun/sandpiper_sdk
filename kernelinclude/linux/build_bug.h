/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_BUILD_BUG_H
#define _LINUX_BUILD_BUG_H

#include <linux/compiler.h>

#ifdef __CHECKER__
#define BUILD_BUG_ON_ZERO(e) (0)
#else 

#define BUILD_BUG_ON_ZERO(e) ((int)(sizeof(struct { int:(-!!(e)); })))
#endif 


#define __BUILD_BUG_ON_NOT_POWER_OF_2(n)	\
	BUILD_BUG_ON(((n) & ((n) - 1)) != 0)
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)			\
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))


#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))


#define BUILD_BUG_ON_MSG(cond, msg) compiletime_assert(!(cond), msg)


#define BUILD_BUG_ON(condition) \
	BUILD_BUG_ON_MSG(condition, "BUILD_BUG_ON failed: " #condition)


#define BUILD_BUG() BUILD_BUG_ON_MSG(1, "BUILD_BUG failed")


#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)



#define ASSERT_STRUCT_OFFSET(type, field, expected_offset)	\
	BUILD_BUG_ON_MSG(offsetof(type, field) != (expected_offset),	\
		"Offset of " #field " in " #type " has changed.")


#endif	
