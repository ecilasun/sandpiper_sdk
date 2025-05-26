/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_LIVEPATCH_H_
#define _LINUX_LIVEPATCH_H_

#include <linux/module.h>
#include <linux/ftrace.h>
#include <linux/completion.h>
#include <linux/list.h>
#include <linux/livepatch_sched.h>

#if IS_ENABLED(CONFIG_LIVEPATCH)


#define KLP_UNDEFINED	-1
#define KLP_UNPATCHED	 0
#define KLP_PATCHED	 1


struct klp_func {
	
	const char *old_name;
	void *new_func;
	
	unsigned long old_sympos;

	
	void *old_func;
	struct kobject kobj;
	struct list_head node;
	struct list_head stack_node;
	unsigned long old_size, new_size;
	bool nop;
	bool patched;
	bool transition;
};

struct klp_object;


struct klp_callbacks {
	int (*pre_patch)(struct klp_object *obj);
	void (*post_patch)(struct klp_object *obj);
	void (*pre_unpatch)(struct klp_object *obj);
	void (*post_unpatch)(struct klp_object *obj);
	bool post_unpatch_enabled;
};


struct klp_object {
	
	const char *name;
	struct klp_func *funcs;
	struct klp_callbacks callbacks;

	
	struct kobject kobj;
	struct list_head func_list;
	struct list_head node;
	struct module *mod;
	bool dynamic;
	bool patched;
};


struct klp_state {
	unsigned long id;
	unsigned int version;
	void *data;
};


struct klp_patch {
	
	struct module *mod;
	struct klp_object *objs;
	struct klp_state *states;
	bool replace;

	
	struct list_head list;
	struct kobject kobj;
	struct list_head obj_list;
	bool enabled;
	bool forced;
	struct work_struct free_work;
	struct completion finish;
};

#define klp_for_each_object_static(patch, obj) \
	for (obj = patch->objs; obj->funcs || obj->name; obj++)

#define klp_for_each_object_safe(patch, obj, tmp_obj)		\
	list_for_each_entry_safe(obj, tmp_obj, &patch->obj_list, node)

#define klp_for_each_object(patch, obj)	\
	list_for_each_entry(obj, &patch->obj_list, node)

#define klp_for_each_func_static(obj, func) \
	for (func = obj->funcs; \
	     func->old_name || func->new_func || func->old_sympos; \
	     func++)

#define klp_for_each_func_safe(obj, func, tmp_func)			\
	list_for_each_entry_safe(func, tmp_func, &obj->func_list, node)

#define klp_for_each_func(obj, func)	\
	list_for_each_entry(func, &obj->func_list, node)

int klp_enable_patch(struct klp_patch *);


int klp_module_coming(struct module *mod);
void klp_module_going(struct module *mod);

void klp_copy_process(struct task_struct *child);
void klp_update_patch_state(struct task_struct *task);

static inline bool klp_patch_pending(struct task_struct *task)
{
	return test_tsk_thread_flag(task, TIF_PATCH_PENDING);
}

static inline bool klp_have_reliable_stack(void)
{
	return IS_ENABLED(CONFIG_STACKTRACE) &&
	       IS_ENABLED(CONFIG_HAVE_RELIABLE_STACKTRACE);
}

typedef int (*klp_shadow_ctor_t)(void *obj,
				 void *shadow_data,
				 void *ctor_data);
typedef void (*klp_shadow_dtor_t)(void *obj, void *shadow_data);

void *klp_shadow_get(void *obj, unsigned long id);
void *klp_shadow_alloc(void *obj, unsigned long id,
		       size_t size, gfp_t gfp_flags,
		       klp_shadow_ctor_t ctor, void *ctor_data);
void *klp_shadow_get_or_alloc(void *obj, unsigned long id,
			      size_t size, gfp_t gfp_flags,
			      klp_shadow_ctor_t ctor, void *ctor_data);
void klp_shadow_free(void *obj, unsigned long id, klp_shadow_dtor_t dtor);
void klp_shadow_free_all(unsigned long id, klp_shadow_dtor_t dtor);

struct klp_state *klp_get_state(struct klp_patch *patch, unsigned long id);
struct klp_state *klp_get_prev_state(unsigned long id);

int klp_apply_section_relocs(struct module *pmod, Elf_Shdr *sechdrs,
			     const char *shstrtab, const char *strtab,
			     unsigned int symindex, unsigned int secindex,
			     const char *objname);

#else 

static inline int klp_module_coming(struct module *mod) { return 0; }
static inline void klp_module_going(struct module *mod) {}
static inline bool klp_patch_pending(struct task_struct *task) { return false; }
static inline void klp_update_patch_state(struct task_struct *task) {}
static inline void klp_copy_process(struct task_struct *child) {}

static inline
int klp_apply_section_relocs(struct module *pmod, Elf_Shdr *sechdrs,
			     const char *shstrtab, const char *strtab,
			     unsigned int symindex, unsigned int secindex,
			     const char *objname)
{
	return 0;
}

#endif 

#endif 
