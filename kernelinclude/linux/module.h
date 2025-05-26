/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_MODULE_H
#define _LINUX_MODULE_H

#include <linux/list.h>
#include <linux/stat.h>
#include <linux/buildid.h>
#include <linux/compiler.h>
#include <linux/cache.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/elf.h>
#include <linux/stringify.h>
#include <linux/kobject.h>
#include <linux/moduleparam.h>
#include <linux/jump_label.h>
#include <linux/export.h>
#include <linux/rbtree_latch.h>
#include <linux/error-injection.h>
#include <linux/tracepoint-defs.h>
#include <linux/srcu.h>
#include <linux/static_call_types.h>
#include <linux/dynamic_debug.h>

#include <linux/percpu.h>
#include <asm/module.h>

#define MODULE_NAME_LEN MAX_PARAM_PREFIX_LEN

struct modversion_info {
	unsigned long crc;
	char name[MODULE_NAME_LEN];
};

struct module;
struct exception_table_entry;

struct module_kobject {
	struct kobject kobj;
	struct module *mod;
	struct kobject *drivers_dir;
	struct module_param_attrs *mp;
	struct completion *kobj_completion;
} __randomize_layout;

struct module_attribute {
	struct attribute attr;
	ssize_t (*show)(struct module_attribute *, struct module_kobject *,
			char *);
	ssize_t (*store)(struct module_attribute *, struct module_kobject *,
			 const char *, size_t count);
	void (*setup)(struct module *, const char *);
	int (*test)(struct module *);
	void (*free)(struct module *);
};

struct module_version_attribute {
	struct module_attribute mattr;
	const char *module_name;
	const char *version;
};

extern ssize_t __modver_version_show(struct module_attribute *,
				     struct module_kobject *, char *);

extern struct module_attribute module_uevent;


extern int init_module(void);
extern void cleanup_module(void);

#ifndef MODULE

#define module_init(x)	__initcall(x);


#define module_exit(x)	__exitcall(x);

#else 


#define early_initcall(fn)		module_init(fn)
#define core_initcall(fn)		module_init(fn)
#define core_initcall_sync(fn)		module_init(fn)
#define postcore_initcall(fn)		module_init(fn)
#define postcore_initcall_sync(fn)	module_init(fn)
#define arch_initcall(fn)		module_init(fn)
#define subsys_initcall(fn)		module_init(fn)
#define subsys_initcall_sync(fn)	module_init(fn)
#define fs_initcall(fn)			module_init(fn)
#define fs_initcall_sync(fn)		module_init(fn)
#define rootfs_initcall(fn)		module_init(fn)
#define device_initcall(fn)		module_init(fn)
#define device_initcall_sync(fn)	module_init(fn)
#define late_initcall(fn)		module_init(fn)
#define late_initcall_sync(fn)		module_init(fn)

#define console_initcall(fn)		module_init(fn)


#define module_init(initfn)					\
	static inline initcall_t __maybe_unused __inittest(void)		\
	{ return initfn; }					\
	int init_module(void) __copy(initfn)			\
		__attribute__((alias(#initfn)));		\
	___ADDRESSABLE(init_module, __initdata);


#define module_exit(exitfn)					\
	static inline exitcall_t __maybe_unused __exittest(void)		\
	{ return exitfn; }					\
	void cleanup_module(void) __copy(exitfn)		\
		__attribute__((alias(#exitfn)));		\
	___ADDRESSABLE(cleanup_module, __exitdata);

#endif


#ifdef CONFIG_MODULES
#define __init_or_module
#define __initdata_or_module
#define __initconst_or_module
#define __INIT_OR_MODULE	.text
#define __INITDATA_OR_MODULE	.data
#define __INITRODATA_OR_MODULE	.section ".rodata","a",%progbits
#else
#define __init_or_module __init
#define __initdata_or_module __initdata
#define __initconst_or_module __initconst
#define __INIT_OR_MODULE __INIT
#define __INITDATA_OR_MODULE __INITDATA
#define __INITRODATA_OR_MODULE __INITRODATA
#endif 


#define MODULE_INFO(tag, info) __MODULE_INFO(tag, tag, info)


#define MODULE_ALIAS(_alias) MODULE_INFO(alias, _alias)


#define MODULE_SOFTDEP(_softdep) MODULE_INFO(softdep, _softdep)


#ifdef MODULE
#define MODULE_FILE
#else
#define MODULE_FILE	MODULE_INFO(file, KBUILD_MODFILE);
#endif


#define MODULE_LICENSE(_license) MODULE_FILE MODULE_INFO(license, _license)


#define MODULE_AUTHOR(_author) MODULE_INFO(author, _author)


#define MODULE_DESCRIPTION(_description) MODULE_INFO(description, _description)

#ifdef MODULE

#define MODULE_DEVICE_TABLE(type, name)					\
extern typeof(name) __mod_##type##__##name##_device_table		\
  __attribute__ ((unused, alias(__stringify(name))))
#else  
#define MODULE_DEVICE_TABLE(type, name)
#endif



#if defined(MODULE) || !defined(CONFIG_SYSFS)
#define MODULE_VERSION(_version) MODULE_INFO(version, _version)
#else
#define MODULE_VERSION(_version)					\
	MODULE_INFO(version, _version);					\
	static struct module_version_attribute __modver_attr		\
		__used __section("__modver")				\
		__aligned(__alignof__(struct module_version_attribute)) \
		= {							\
			.mattr	= {					\
				.attr	= {				\
					.name	= "version",		\
					.mode	= S_IRUGO,		\
				},					\
				.show	= __modver_version_show,	\
			},						\
			.module_name	= KBUILD_MODNAME,		\
			.version	= _version,			\
		}
#endif


#define MODULE_FIRMWARE(_firmware) MODULE_INFO(firmware, _firmware)

#define MODULE_IMPORT_NS(ns)	MODULE_INFO(import_ns, __stringify(ns))

struct notifier_block;

#ifdef CONFIG_MODULES

extern int modules_disabled; 

void *__symbol_get(const char *symbol);
void *__symbol_get_gpl(const char *symbol);
#define symbol_get(x) ((typeof(&x))(__symbol_get(__stringify(x))))


struct module_use {
	struct list_head source_list;
	struct list_head target_list;
	struct module *source, *target;
};

enum module_state {
	MODULE_STATE_LIVE,	
	MODULE_STATE_COMING,	
	MODULE_STATE_GOING,	
	MODULE_STATE_UNFORMED,	
};

struct mod_tree_node {
	struct module *mod;
	struct latch_tree_node node;
};

enum mod_mem_type {
	MOD_TEXT = 0,
	MOD_DATA,
	MOD_RODATA,
	MOD_RO_AFTER_INIT,
	MOD_INIT_TEXT,
	MOD_INIT_DATA,
	MOD_INIT_RODATA,

	MOD_MEM_NUM_TYPES,
	MOD_INVALID = -1,
};

#define mod_mem_type_is_init(type)	\
	((type) == MOD_INIT_TEXT ||	\
	 (type) == MOD_INIT_DATA ||	\
	 (type) == MOD_INIT_RODATA)

#define mod_mem_type_is_core(type) (!mod_mem_type_is_init(type))

#define mod_mem_type_is_text(type)	\
	 ((type) == MOD_TEXT ||		\
	  (type) == MOD_INIT_TEXT)

#define mod_mem_type_is_data(type) (!mod_mem_type_is_text(type))

#define mod_mem_type_is_core_data(type)	\
	(mod_mem_type_is_core(type) &&	\
	 mod_mem_type_is_data(type))

#define for_each_mod_mem_type(type)			\
	for (enum mod_mem_type (type) = 0;		\
	     (type) < MOD_MEM_NUM_TYPES; (type)++)

#define for_class_mod_mem_type(type, class)		\
	for_each_mod_mem_type(type)			\
		if (mod_mem_type_is_##class(type))

struct module_memory {
	void *base;
	unsigned int size;

#ifdef CONFIG_MODULES_TREE_LOOKUP
	struct mod_tree_node mtn;
#endif
};

#ifdef CONFIG_MODULES_TREE_LOOKUP

#define __module_memory_align ____cacheline_aligned
#else
#define __module_memory_align
#endif

struct mod_kallsyms {
	Elf_Sym *symtab;
	unsigned int num_symtab;
	char *strtab;
	char *typetab;
};

#ifdef CONFIG_LIVEPATCH

struct klp_modinfo {
	Elf_Ehdr hdr;
	Elf_Shdr *sechdrs;
	char *secstrings;
	unsigned int symndx;
};
#endif

struct module {
	enum module_state state;

	
	struct list_head list;

	
	char name[MODULE_NAME_LEN];

#ifdef CONFIG_STACKTRACE_BUILD_ID
	
	unsigned char build_id[BUILD_ID_SIZE_MAX];
#endif

	
	struct module_kobject mkobj;
	struct module_attribute *modinfo_attrs;
	const char *version;
	const char *srcversion;
	struct kobject *holders_dir;

	
	const struct kernel_symbol *syms;
	const s32 *crcs;
	unsigned int num_syms;

#ifdef CONFIG_ARCH_USES_CFI_TRAPS
	s32 *kcfi_traps;
	s32 *kcfi_traps_end;
#endif

	
#ifdef CONFIG_SYSFS
	struct mutex param_lock;
#endif
	struct kernel_param *kp;
	unsigned int num_kp;

	
	unsigned int num_gpl_syms;
	const struct kernel_symbol *gpl_syms;
	const s32 *gpl_crcs;
	bool using_gplonly_symbols;

#ifdef CONFIG_MODULE_SIG
	
	bool sig_ok;
#endif

	bool async_probe_requested;

	
	unsigned int num_exentries;
	struct exception_table_entry *extable;

	
	int (*init)(void);

	struct module_memory mem[MOD_MEM_NUM_TYPES] __module_memory_align;

	
	struct mod_arch_specific arch;

	unsigned long taints;	

#ifdef CONFIG_GENERIC_BUG
	
	unsigned num_bugs;
	struct list_head bug_list;
	struct bug_entry *bug_table;
#endif

#ifdef CONFIG_KALLSYMS
	
	struct mod_kallsyms __rcu *kallsyms;
	struct mod_kallsyms core_kallsyms;

	
	struct module_sect_attrs *sect_attrs;

	
	struct module_notes_attrs *notes_attrs;
#endif

	
	char *args;

#ifdef CONFIG_SMP
	
	void __percpu *percpu;
	unsigned int percpu_size;
#endif
	void *noinstr_text_start;
	unsigned int noinstr_text_size;

#ifdef CONFIG_TRACEPOINTS
	unsigned int num_tracepoints;
	tracepoint_ptr_t *tracepoints_ptrs;
#endif
#ifdef CONFIG_TREE_SRCU
	unsigned int num_srcu_structs;
	struct srcu_struct **srcu_struct_ptrs;
#endif
#ifdef CONFIG_BPF_EVENTS
	unsigned int num_bpf_raw_events;
	struct bpf_raw_event_map *bpf_raw_events;
#endif
#ifdef CONFIG_DEBUG_INFO_BTF_MODULES
	unsigned int btf_data_size;
	void *btf_data;
#endif
#ifdef CONFIG_JUMP_LABEL
	struct jump_entry *jump_entries;
	unsigned int num_jump_entries;
#endif
#ifdef CONFIG_TRACING
	unsigned int num_trace_bprintk_fmt;
	const char **trace_bprintk_fmt_start;
#endif
#ifdef CONFIG_EVENT_TRACING
	struct trace_event_call **trace_events;
	unsigned int num_trace_events;
	struct trace_eval_map **trace_evals;
	unsigned int num_trace_evals;
#endif
#ifdef CONFIG_FTRACE_MCOUNT_RECORD
	unsigned int num_ftrace_callsites;
	unsigned long *ftrace_callsites;
#endif
#ifdef CONFIG_KPROBES
	void *kprobes_text_start;
	unsigned int kprobes_text_size;
	unsigned long *kprobe_blacklist;
	unsigned int num_kprobe_blacklist;
#endif
#ifdef CONFIG_HAVE_STATIC_CALL_INLINE
	int num_static_call_sites;
	struct static_call_site *static_call_sites;
#endif
#if IS_ENABLED(CONFIG_KUNIT)
	int num_kunit_suites;
	struct kunit_suite **kunit_suites;
#endif


#ifdef CONFIG_LIVEPATCH
	bool klp; 
	bool klp_alive;

	
	struct klp_modinfo *klp_info;
#endif

#ifdef CONFIG_PRINTK_INDEX
	unsigned int printk_index_size;
	struct pi_entry **printk_index_start;
#endif

#ifdef CONFIG_MODULE_UNLOAD
	
	struct list_head source_list;
	
	struct list_head target_list;

	
	void (*exit)(void);

	atomic_t refcnt;
#endif

#ifdef CONFIG_CONSTRUCTORS
	
	ctor_fn_t *ctors;
	unsigned int num_ctors;
#endif

#ifdef CONFIG_FUNCTION_ERROR_INJECTION
	struct error_injection_entry *ei_funcs;
	unsigned int num_ei_funcs;
#endif
#ifdef CONFIG_DYNAMIC_DEBUG_CORE
	struct _ddebug_info dyndbg_info;
#endif
} ____cacheline_aligned __randomize_layout;
#ifndef MODULE_ARCH_INIT
#define MODULE_ARCH_INIT {}
#endif

#ifndef HAVE_ARCH_KALLSYMS_SYMBOL_VALUE
static inline unsigned long kallsyms_symbol_value(const Elf_Sym *sym)
{
	return sym->st_value;
}
#endif


static inline bool module_is_live(struct module *mod)
{
	return mod->state != MODULE_STATE_GOING;
}

struct module *__module_text_address(unsigned long addr);
struct module *__module_address(unsigned long addr);
bool is_module_address(unsigned long addr);
bool __is_module_percpu_address(unsigned long addr, unsigned long *can_addr);
bool is_module_percpu_address(unsigned long addr);
bool is_module_text_address(unsigned long addr);

static inline bool within_module_mem_type(unsigned long addr,
					  const struct module *mod,
					  enum mod_mem_type type)
{
	unsigned long base, size;

	base = (unsigned long)mod->mem[type].base;
	size = mod->mem[type].size;
	return addr - base < size;
}

static inline bool within_module_core(unsigned long addr,
				      const struct module *mod)
{
	for_class_mod_mem_type(type, core) {
		if (within_module_mem_type(addr, mod, type))
			return true;
	}
	return false;
}

static inline bool within_module_init(unsigned long addr,
				      const struct module *mod)
{
	for_class_mod_mem_type(type, init) {
		if (within_module_mem_type(addr, mod, type))
			return true;
	}
	return false;
}

static inline bool within_module(unsigned long addr, const struct module *mod)
{
	return within_module_init(addr, mod) || within_module_core(addr, mod);
}


struct module *find_module(const char *name);

extern void __noreturn __module_put_and_kthread_exit(struct module *mod,
			long code);
#define module_put_and_kthread_exit(code) __module_put_and_kthread_exit(THIS_MODULE, code)

#ifdef CONFIG_MODULE_UNLOAD
int module_refcount(struct module *mod);
void __symbol_put(const char *symbol);
#define symbol_put(x) __symbol_put(__stringify(x))
void symbol_put_addr(void *addr);


extern void __module_get(struct module *module);


extern bool try_module_get(struct module *module);


extern void module_put(struct module *module);

#else 
static inline bool try_module_get(struct module *module)
{
	return !module || module_is_live(module);
}
static inline void module_put(struct module *module)
{
}
static inline void __module_get(struct module *module)
{
}
#define symbol_put(x) do { } while (0)
#define symbol_put_addr(p) do { } while (0)

#endif 


#define module_name(mod)			\
({						\
	struct module *__mod = (mod);		\
	__mod ? __mod->name : "kernel";		\
})


void *dereference_module_function_descriptor(struct module *mod, void *ptr);

int register_module_notifier(struct notifier_block *nb);
int unregister_module_notifier(struct notifier_block *nb);

extern void print_modules(void);

static inline bool module_requested_async_probing(struct module *module)
{
	return module && module->async_probe_requested;
}

static inline bool is_livepatch_module(struct module *mod)
{
#ifdef CONFIG_LIVEPATCH
	return mod->klp;
#else
	return false;
#endif
}

void set_module_sig_enforced(void);

#else 

static inline struct module *__module_address(unsigned long addr)
{
	return NULL;
}

static inline struct module *__module_text_address(unsigned long addr)
{
	return NULL;
}

static inline bool is_module_address(unsigned long addr)
{
	return false;
}

static inline bool is_module_percpu_address(unsigned long addr)
{
	return false;
}

static inline bool __is_module_percpu_address(unsigned long addr, unsigned long *can_addr)
{
	return false;
}

static inline bool is_module_text_address(unsigned long addr)
{
	return false;
}

static inline bool within_module_core(unsigned long addr,
				      const struct module *mod)
{
	return false;
}

static inline bool within_module_init(unsigned long addr,
				      const struct module *mod)
{
	return false;
}

static inline bool within_module(unsigned long addr, const struct module *mod)
{
	return false;
}


#define symbol_get(x) ({ extern typeof(x) x __attribute__((weak,visibility("hidden"))); &(x); })
#define symbol_put(x) do { } while (0)
#define symbol_put_addr(x) do { } while (0)

static inline void __module_get(struct module *module)
{
}

static inline bool try_module_get(struct module *module)
{
	return true;
}

static inline void module_put(struct module *module)
{
}

#define module_name(mod) "kernel"

static inline int register_module_notifier(struct notifier_block *nb)
{
	
	return 0;
}

static inline int unregister_module_notifier(struct notifier_block *nb)
{
	return 0;
}

#define module_put_and_kthread_exit(code) kthread_exit(code)

static inline void print_modules(void)
{
}

static inline bool module_requested_async_probing(struct module *module)
{
	return false;
}


static inline void set_module_sig_enforced(void)
{
}


static inline
void *dereference_module_function_descriptor(struct module *mod, void *ptr)
{
	return ptr;
}

#endif 

#ifdef CONFIG_SYSFS
extern struct kset *module_kset;
extern const struct kobj_type module_ktype;
#endif 

#define symbol_request(x) try_then_request_module(symbol_get(x), "symbol:" #x)



#define __MODULE_STRING(x) __stringify(x)

#ifdef CONFIG_GENERIC_BUG
void module_bug_finalize(const Elf_Ehdr *, const Elf_Shdr *,
			 struct module *);
void module_bug_cleanup(struct module *);

#else	

static inline void module_bug_finalize(const Elf_Ehdr *hdr,
					const Elf_Shdr *sechdrs,
					struct module *mod)
{
}
static inline void module_bug_cleanup(struct module *mod) {}
#endif	

#ifdef CONFIG_RETPOLINE
extern bool retpoline_module_ok(bool has_retpoline);
#else
static inline bool retpoline_module_ok(bool has_retpoline)
{
	return true;
}
#endif

#ifdef CONFIG_MODULE_SIG
bool is_module_sig_enforced(void);

static inline bool module_sig_ok(struct module *module)
{
	return module->sig_ok;
}
#else	
static inline bool is_module_sig_enforced(void)
{
	return false;
}

static inline bool module_sig_ok(struct module *module)
{
	return true;
}
#endif	

#if defined(CONFIG_MODULES) && defined(CONFIG_KALLSYMS)
int module_kallsyms_on_each_symbol(const char *modname,
				   int (*fn)(void *, const char *, unsigned long),
				   void *data);


const char *module_address_lookup(unsigned long addr,
				  unsigned long *symbolsize,
				  unsigned long *offset,
				  char **modname, const unsigned char **modbuildid,
				  char *namebuf);
int lookup_module_symbol_name(unsigned long addr, char *symname);
int lookup_module_symbol_attrs(unsigned long addr,
			       unsigned long *size,
			       unsigned long *offset,
			       char *modname,
			       char *name);


int module_get_kallsym(unsigned int symnum, unsigned long *value, char *type,
		       char *name, char *module_name, int *exported);


unsigned long module_kallsyms_lookup_name(const char *name);

unsigned long find_kallsyms_symbol_value(struct module *mod, const char *name);

#else	

static inline int module_kallsyms_on_each_symbol(const char *modname,
						 int (*fn)(void *, const char *, unsigned long),
						 void *data)
{
	return -EOPNOTSUPP;
}


static inline const char *module_address_lookup(unsigned long addr,
						unsigned long *symbolsize,
						unsigned long *offset,
						char **modname,
						const unsigned char **modbuildid,
						char *namebuf)
{
	return NULL;
}

static inline int lookup_module_symbol_name(unsigned long addr, char *symname)
{
	return -ERANGE;
}

static inline int module_get_kallsym(unsigned int symnum, unsigned long *value,
				     char *type, char *name,
				     char *module_name, int *exported)
{
	return -ERANGE;
}

static inline unsigned long module_kallsyms_lookup_name(const char *name)
{
	return 0;
}

static inline unsigned long find_kallsyms_symbol_value(struct module *mod,
						       const char *name)
{
	return 0;
}

#endif  

#endif 
