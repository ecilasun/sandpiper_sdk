/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _STATIC_CALL_TYPES_H
#define _STATIC_CALL_TYPES_H

#include <linux/types.h>
#include <linux/stringify.h>
#include <linux/compiler.h>

#define STATIC_CALL_KEY_PREFIX		__SCK__
#define STATIC_CALL_KEY_PREFIX_STR	__stringify(STATIC_CALL_KEY_PREFIX)
#define STATIC_CALL_KEY_PREFIX_LEN	(sizeof(STATIC_CALL_KEY_PREFIX_STR) - 1)
#define STATIC_CALL_KEY(name)		__PASTE(STATIC_CALL_KEY_PREFIX, name)
#define STATIC_CALL_KEY_STR(name)	__stringify(STATIC_CALL_KEY(name))

#define STATIC_CALL_TRAMP_PREFIX	__SCT__
#define STATIC_CALL_TRAMP_PREFIX_STR	__stringify(STATIC_CALL_TRAMP_PREFIX)
#define STATIC_CALL_TRAMP_PREFIX_LEN	(sizeof(STATIC_CALL_TRAMP_PREFIX_STR) - 1)
#define STATIC_CALL_TRAMP(name)		__PASTE(STATIC_CALL_TRAMP_PREFIX, name)
#define STATIC_CALL_TRAMP_STR(name)	__stringify(STATIC_CALL_TRAMP(name))


#define STATIC_CALL_SITE_TAIL 1UL	
#define STATIC_CALL_SITE_INIT 2UL	
#define STATIC_CALL_SITE_FLAGS 3UL


struct static_call_site {
	s32 addr;
	s32 key;
};

#define DECLARE_STATIC_CALL(name, func)					\
	extern struct static_call_key STATIC_CALL_KEY(name);		\
	extern typeof(func) STATIC_CALL_TRAMP(name);

#ifdef CONFIG_HAVE_STATIC_CALL

#define __raw_static_call(name)	(&STATIC_CALL_TRAMP(name))

#ifdef CONFIG_HAVE_STATIC_CALL_INLINE


#define __STATIC_CALL_ADDRESSABLE(name) \
	__ADDRESSABLE(STATIC_CALL_KEY(name))

#define __static_call(name)						\
({									\
	__STATIC_CALL_ADDRESSABLE(name);				\
	__raw_static_call(name);					\
})

struct static_call_key {
	void *func;
	union {
		
		unsigned long type;
		struct static_call_mod *mods;
		struct static_call_site *sites;
	};
};

#else 

#define __STATIC_CALL_ADDRESSABLE(name)
#define __static_call(name)	__raw_static_call(name)

struct static_call_key {
	void *func;
};

#endif 

#ifdef MODULE
#define __STATIC_CALL_MOD_ADDRESSABLE(name)
#define static_call_mod(name)	__raw_static_call(name)
#else
#define __STATIC_CALL_MOD_ADDRESSABLE(name) __STATIC_CALL_ADDRESSABLE(name)
#define static_call_mod(name)	__static_call(name)
#endif

#define static_call(name)	__static_call(name)

#else

struct static_call_key {
	void *func;
};

#define static_call(name)						\
	((typeof(STATIC_CALL_TRAMP(name))*)(STATIC_CALL_KEY(name).func))

#endif 

#endif 
