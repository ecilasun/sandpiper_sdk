/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_STATIC_CALL_H
#define _LINUX_STATIC_CALL_H



#include <linux/types.h>
#include <linux/cpu.h>
#include <linux/static_call_types.h>

#ifdef CONFIG_HAVE_STATIC_CALL
#include <asm/static_call.h>


extern void arch_static_call_transform(void *site, void *tramp, void *func, bool tail);

#define STATIC_CALL_TRAMP_ADDR(name) &STATIC_CALL_TRAMP(name)

#else
#define STATIC_CALL_TRAMP_ADDR(name) NULL
#endif

#define static_call_update(name, func)					\
({									\
	typeof(&STATIC_CALL_TRAMP(name)) __F = (func);			\
	__static_call_update(&STATIC_CALL_KEY(name),			\
			     STATIC_CALL_TRAMP_ADDR(name), __F);	\
})

#define static_call_query(name) (READ_ONCE(STATIC_CALL_KEY(name).func))

#ifdef CONFIG_HAVE_STATIC_CALL_INLINE

extern int __init static_call_init(void);

extern void static_call_force_reinit(void);

struct static_call_mod {
	struct static_call_mod *next;
	struct module *mod; 
	struct static_call_site *sites;
};


struct static_call_tramp_key {
	s32 tramp;
	s32 key;
};

extern void __static_call_update(struct static_call_key *key, void *tramp, void *func);
extern int static_call_mod_init(struct module *mod);
extern int static_call_text_reserved(void *start, void *end);

extern long __static_call_return0(void);

#define DEFINE_STATIC_CALL(name, _func)					\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = _func,						\
		.type = 1,						\
	};								\
	ARCH_DEFINE_STATIC_CALL_TRAMP(name, _func)

#define DEFINE_STATIC_CALL_NULL(name, _func)				\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = NULL,						\
		.type = 1,						\
	};								\
	ARCH_DEFINE_STATIC_CALL_NULL_TRAMP(name)

#define DEFINE_STATIC_CALL_RET0(name, _func)				\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = __static_call_return0,				\
		.type = 1,						\
	};								\
	ARCH_DEFINE_STATIC_CALL_RET0_TRAMP(name)

#define static_call_cond(name)	(void)__static_call(name)

#define EXPORT_STATIC_CALL(name)					\
	EXPORT_SYMBOL(STATIC_CALL_KEY(name));				\
	EXPORT_SYMBOL(STATIC_CALL_TRAMP(name))
#define EXPORT_STATIC_CALL_GPL(name)					\
	EXPORT_SYMBOL_GPL(STATIC_CALL_KEY(name));			\
	EXPORT_SYMBOL_GPL(STATIC_CALL_TRAMP(name))


#define EXPORT_STATIC_CALL_TRAMP(name)					\
	EXPORT_SYMBOL(STATIC_CALL_TRAMP(name));				\
	ARCH_ADD_TRAMP_KEY(name)
#define EXPORT_STATIC_CALL_TRAMP_GPL(name)				\
	EXPORT_SYMBOL_GPL(STATIC_CALL_TRAMP(name));			\
	ARCH_ADD_TRAMP_KEY(name)

#elif defined(CONFIG_HAVE_STATIC_CALL)

static inline int static_call_init(void) { return 0; }

#define DEFINE_STATIC_CALL(name, _func)					\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = _func,						\
	};								\
	ARCH_DEFINE_STATIC_CALL_TRAMP(name, _func)

#define DEFINE_STATIC_CALL_NULL(name, _func)				\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = NULL,						\
	};								\
	ARCH_DEFINE_STATIC_CALL_NULL_TRAMP(name)

#define DEFINE_STATIC_CALL_RET0(name, _func)				\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = __static_call_return0,				\
	};								\
	ARCH_DEFINE_STATIC_CALL_RET0_TRAMP(name)

#define static_call_cond(name)	(void)__static_call(name)

static inline
void __static_call_update(struct static_call_key *key, void *tramp, void *func)
{
	cpus_read_lock();
	WRITE_ONCE(key->func, func);
	arch_static_call_transform(NULL, tramp, func, false);
	cpus_read_unlock();
}

static inline int static_call_text_reserved(void *start, void *end)
{
	return 0;
}

extern long __static_call_return0(void);

#define EXPORT_STATIC_CALL(name)					\
	EXPORT_SYMBOL(STATIC_CALL_KEY(name));				\
	EXPORT_SYMBOL(STATIC_CALL_TRAMP(name))
#define EXPORT_STATIC_CALL_GPL(name)					\
	EXPORT_SYMBOL_GPL(STATIC_CALL_KEY(name));			\
	EXPORT_SYMBOL_GPL(STATIC_CALL_TRAMP(name))


#define EXPORT_STATIC_CALL_TRAMP(name)					\
	EXPORT_SYMBOL(STATIC_CALL_TRAMP(name))
#define EXPORT_STATIC_CALL_TRAMP_GPL(name)				\
	EXPORT_SYMBOL_GPL(STATIC_CALL_TRAMP(name))

#else 

static inline int static_call_init(void) { return 0; }

static inline long __static_call_return0(void)
{
	return 0;
}

#define __DEFINE_STATIC_CALL(name, _func, _func_init)			\
	DECLARE_STATIC_CALL(name, _func);				\
	struct static_call_key STATIC_CALL_KEY(name) = {		\
		.func = _func_init,					\
	}

#define DEFINE_STATIC_CALL(name, _func)					\
	__DEFINE_STATIC_CALL(name, _func, _func)

#define DEFINE_STATIC_CALL_NULL(name, _func)				\
	__DEFINE_STATIC_CALL(name, _func, NULL)

#define DEFINE_STATIC_CALL_RET0(name, _func)				\
	__DEFINE_STATIC_CALL(name, _func, __static_call_return0)

static inline void __static_call_nop(void) { }


#define __static_call_cond(name)					\
({									\
	void *func = READ_ONCE(STATIC_CALL_KEY(name).func);		\
	if (!func)							\
		func = &__static_call_nop;				\
	(typeof(STATIC_CALL_TRAMP(name))*)func;				\
})

#define static_call_cond(name)	(void)__static_call_cond(name)

static inline
void __static_call_update(struct static_call_key *key, void *tramp, void *func)
{
	WRITE_ONCE(key->func, func);
}

static inline int static_call_text_reserved(void *start, void *end)
{
	return 0;
}

#define EXPORT_STATIC_CALL(name)	EXPORT_SYMBOL(STATIC_CALL_KEY(name))
#define EXPORT_STATIC_CALL_GPL(name)	EXPORT_SYMBOL_GPL(STATIC_CALL_KEY(name))

#endif 

#endif 
