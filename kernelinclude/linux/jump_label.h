/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_JUMP_LABEL_H
#define _LINUX_JUMP_LABEL_H



#ifndef __ASSEMBLY__

#include <linux/types.h>
#include <linux/compiler.h>

extern bool static_key_initialized;

#define STATIC_KEY_CHECK_USE(key) WARN(!static_key_initialized,		      \
				    "%s(): static key '%pS' used before call to jump_label_init()", \
				    __func__, (key))

struct static_key {
	atomic_t enabled;
#ifdef CONFIG_JUMP_LABEL

	union {
		unsigned long type;
		struct jump_entry *entries;
		struct static_key_mod *next;
	};
#endif	
};

#endif 

#ifdef CONFIG_JUMP_LABEL
#include <asm/jump_label.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_HAVE_ARCH_JUMP_LABEL_RELATIVE

struct jump_entry {
	s32 code;
	s32 target;
	long key;	// key may be far away from the core kernel under KASLR
};

static inline unsigned long jump_entry_code(const struct jump_entry *entry)
{
	return (unsigned long)&entry->code + entry->code;
}

static inline unsigned long jump_entry_target(const struct jump_entry *entry)
{
	return (unsigned long)&entry->target + entry->target;
}

static inline struct static_key *jump_entry_key(const struct jump_entry *entry)
{
	long offset = entry->key & ~3L;

	return (struct static_key *)((unsigned long)&entry->key + offset);
}

#else

static inline unsigned long jump_entry_code(const struct jump_entry *entry)
{
	return entry->code;
}

static inline unsigned long jump_entry_target(const struct jump_entry *entry)
{
	return entry->target;
}

static inline struct static_key *jump_entry_key(const struct jump_entry *entry)
{
	return (struct static_key *)((unsigned long)entry->key & ~3UL);
}

#endif

static inline bool jump_entry_is_branch(const struct jump_entry *entry)
{
	return (unsigned long)entry->key & 1UL;
}

static inline bool jump_entry_is_init(const struct jump_entry *entry)
{
	return (unsigned long)entry->key & 2UL;
}

static inline void jump_entry_set_init(struct jump_entry *entry, bool set)
{
	if (set)
		entry->key |= 2;
	else
		entry->key &= ~2;
}

static inline int jump_entry_size(struct jump_entry *entry)
{
#ifdef JUMP_LABEL_NOP_SIZE
	return JUMP_LABEL_NOP_SIZE;
#else
	return arch_jump_entry_size(entry);
#endif
}

#endif
#endif

#ifndef __ASSEMBLY__

enum jump_label_type {
	JUMP_LABEL_NOP = 0,
	JUMP_LABEL_JMP,
};

struct module;

#ifdef CONFIG_JUMP_LABEL

#define JUMP_TYPE_FALSE		0UL
#define JUMP_TYPE_TRUE		1UL
#define JUMP_TYPE_LINKED	2UL
#define JUMP_TYPE_MASK		3UL

static __always_inline bool static_key_false(struct static_key *key)
{
	return arch_static_branch(key, false);
}

static __always_inline bool static_key_true(struct static_key *key)
{
	return !arch_static_branch(key, true);
}

extern struct jump_entry __start___jump_table[];
extern struct jump_entry __stop___jump_table[];

extern void jump_label_init(void);
extern void jump_label_lock(void);
extern void jump_label_unlock(void);
extern void arch_jump_label_transform(struct jump_entry *entry,
				      enum jump_label_type type);
extern bool arch_jump_label_transform_queue(struct jump_entry *entry,
					    enum jump_label_type type);
extern void arch_jump_label_transform_apply(void);
extern int jump_label_text_reserved(void *start, void *end);
extern bool static_key_slow_inc(struct static_key *key);
extern bool static_key_fast_inc_not_disabled(struct static_key *key);
extern void static_key_slow_dec(struct static_key *key);
extern bool static_key_slow_inc_cpuslocked(struct static_key *key);
extern void static_key_slow_dec_cpuslocked(struct static_key *key);
extern int static_key_count(struct static_key *key);
extern void static_key_enable(struct static_key *key);
extern void static_key_disable(struct static_key *key);
extern void static_key_enable_cpuslocked(struct static_key *key);
extern void static_key_disable_cpuslocked(struct static_key *key);
extern enum jump_label_type jump_label_init_type(struct jump_entry *entry);


#define STATIC_KEY_INIT_TRUE					\
	{ .enabled = { 1 },					\
	  { .type = JUMP_TYPE_TRUE } }
#define STATIC_KEY_INIT_FALSE					\
	{ .enabled = { 0 },					\
	  { .type = JUMP_TYPE_FALSE } }

#else  

#include <linux/atomic.h>
#include <linux/bug.h>

static __always_inline int static_key_count(struct static_key *key)
{
	return raw_atomic_read(&key->enabled);
}

static __always_inline void jump_label_init(void)
{
	static_key_initialized = true;
}

static __always_inline bool static_key_false(struct static_key *key)
{
	if (unlikely_notrace(static_key_count(key) > 0))
		return true;
	return false;
}

static __always_inline bool static_key_true(struct static_key *key)
{
	if (likely_notrace(static_key_count(key) > 0))
		return true;
	return false;
}

static inline bool static_key_fast_inc_not_disabled(struct static_key *key)
{
	int v;

	STATIC_KEY_CHECK_USE(key);
	
	v = atomic_read(&key->enabled);
	do {
		if (v < 0 || (v + 1) < 0)
			return false;
	} while (!likely(atomic_try_cmpxchg(&key->enabled, &v, v + 1)));
	return true;
}
#define static_key_slow_inc(key)	static_key_fast_inc_not_disabled(key)

static inline void static_key_slow_dec(struct static_key *key)
{
	STATIC_KEY_CHECK_USE(key);
	atomic_dec(&key->enabled);
}

#define static_key_slow_inc_cpuslocked(key) static_key_slow_inc(key)
#define static_key_slow_dec_cpuslocked(key) static_key_slow_dec(key)

static inline int jump_label_text_reserved(void *start, void *end)
{
	return 0;
}

static inline void jump_label_lock(void) {}
static inline void jump_label_unlock(void) {}

static inline void static_key_enable(struct static_key *key)
{
	STATIC_KEY_CHECK_USE(key);

	if (atomic_read(&key->enabled) != 0) {
		WARN_ON_ONCE(atomic_read(&key->enabled) != 1);
		return;
	}
	atomic_set(&key->enabled, 1);
}

static inline void static_key_disable(struct static_key *key)
{
	STATIC_KEY_CHECK_USE(key);

	if (atomic_read(&key->enabled) != 1) {
		WARN_ON_ONCE(atomic_read(&key->enabled) != 0);
		return;
	}
	atomic_set(&key->enabled, 0);
}

#define static_key_enable_cpuslocked(k)		static_key_enable((k))
#define static_key_disable_cpuslocked(k)	static_key_disable((k))

#define STATIC_KEY_INIT_TRUE	{ .enabled = ATOMIC_INIT(1) }
#define STATIC_KEY_INIT_FALSE	{ .enabled = ATOMIC_INIT(0) }

#endif	

#define STATIC_KEY_INIT STATIC_KEY_INIT_FALSE
#define jump_label_enabled static_key_enabled





struct static_key_true {
	struct static_key key;
};

struct static_key_false {
	struct static_key key;
};

#define STATIC_KEY_TRUE_INIT  (struct static_key_true) { .key = STATIC_KEY_INIT_TRUE,  }
#define STATIC_KEY_FALSE_INIT (struct static_key_false){ .key = STATIC_KEY_INIT_FALSE, }

#define DEFINE_STATIC_KEY_TRUE(name)	\
	struct static_key_true name = STATIC_KEY_TRUE_INIT

#define DEFINE_STATIC_KEY_TRUE_RO(name)	\
	struct static_key_true name __ro_after_init = STATIC_KEY_TRUE_INIT

#define DECLARE_STATIC_KEY_TRUE(name)	\
	extern struct static_key_true name

#define DEFINE_STATIC_KEY_FALSE(name)	\
	struct static_key_false name = STATIC_KEY_FALSE_INIT

#define DEFINE_STATIC_KEY_FALSE_RO(name)	\
	struct static_key_false name __ro_after_init = STATIC_KEY_FALSE_INIT

#define DECLARE_STATIC_KEY_FALSE(name)	\
	extern struct static_key_false name

#define DEFINE_STATIC_KEY_ARRAY_TRUE(name, count)		\
	struct static_key_true name[count] = {			\
		[0 ... (count) - 1] = STATIC_KEY_TRUE_INIT,	\
	}

#define DEFINE_STATIC_KEY_ARRAY_FALSE(name, count)		\
	struct static_key_false name[count] = {			\
		[0 ... (count) - 1] = STATIC_KEY_FALSE_INIT,	\
	}

#define _DEFINE_STATIC_KEY_1(name)	DEFINE_STATIC_KEY_TRUE(name)
#define _DEFINE_STATIC_KEY_0(name)	DEFINE_STATIC_KEY_FALSE(name)
#define DEFINE_STATIC_KEY_MAYBE(cfg, name)			\
	__PASTE(_DEFINE_STATIC_KEY_, IS_ENABLED(cfg))(name)

#define _DEFINE_STATIC_KEY_RO_1(name)	DEFINE_STATIC_KEY_TRUE_RO(name)
#define _DEFINE_STATIC_KEY_RO_0(name)	DEFINE_STATIC_KEY_FALSE_RO(name)
#define DEFINE_STATIC_KEY_MAYBE_RO(cfg, name)			\
	__PASTE(_DEFINE_STATIC_KEY_RO_, IS_ENABLED(cfg))(name)

#define _DECLARE_STATIC_KEY_1(name)	DECLARE_STATIC_KEY_TRUE(name)
#define _DECLARE_STATIC_KEY_0(name)	DECLARE_STATIC_KEY_FALSE(name)
#define DECLARE_STATIC_KEY_MAYBE(cfg, name)			\
	__PASTE(_DECLARE_STATIC_KEY_, IS_ENABLED(cfg))(name)

extern bool ____wrong_branch_error(void);

#define static_key_enabled(x)							\
({										\
	if (!__builtin_types_compatible_p(typeof(*x), struct static_key) &&	\
	    !__builtin_types_compatible_p(typeof(*x), struct static_key_true) &&\
	    !__builtin_types_compatible_p(typeof(*x), struct static_key_false))	\
		____wrong_branch_error();					\
	static_key_count((struct static_key *)x) > 0;				\
})

#ifdef CONFIG_JUMP_LABEL



#define static_branch_likely(x)							\
({										\
	bool branch;								\
	if (__builtin_types_compatible_p(typeof(*x), struct static_key_true))	\
		branch = !arch_static_branch(&(x)->key, true);			\
	else if (__builtin_types_compatible_p(typeof(*x), struct static_key_false)) \
		branch = !arch_static_branch_jump(&(x)->key, true);		\
	else									\
		branch = ____wrong_branch_error();				\
	likely_notrace(branch);								\
})

#define static_branch_unlikely(x)						\
({										\
	bool branch;								\
	if (__builtin_types_compatible_p(typeof(*x), struct static_key_true))	\
		branch = arch_static_branch_jump(&(x)->key, false);		\
	else if (__builtin_types_compatible_p(typeof(*x), struct static_key_false)) \
		branch = arch_static_branch(&(x)->key, false);			\
	else									\
		branch = ____wrong_branch_error();				\
	unlikely_notrace(branch);							\
})

#else 

#define static_branch_likely(x)		likely_notrace(static_key_enabled(&(x)->key))
#define static_branch_unlikely(x)	unlikely_notrace(static_key_enabled(&(x)->key))

#endif 

#define static_branch_maybe(config, x)					\
	(IS_ENABLED(config) ? static_branch_likely(x)			\
			    : static_branch_unlikely(x))



#define static_branch_inc(x)		static_key_slow_inc(&(x)->key)
#define static_branch_dec(x)		static_key_slow_dec(&(x)->key)
#define static_branch_inc_cpuslocked(x)	static_key_slow_inc_cpuslocked(&(x)->key)
#define static_branch_dec_cpuslocked(x)	static_key_slow_dec_cpuslocked(&(x)->key)



#define static_branch_enable(x)			static_key_enable(&(x)->key)
#define static_branch_disable(x)		static_key_disable(&(x)->key)
#define static_branch_enable_cpuslocked(x)	static_key_enable_cpuslocked(&(x)->key)
#define static_branch_disable_cpuslocked(x)	static_key_disable_cpuslocked(&(x)->key)

#endif 

#endif	
