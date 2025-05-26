/* SPDX-License-Identifier: GPL-2.0 */


#include <rv/automata.h>
#include <linux/rv.h>
#include <linux/bug.h>

#ifdef CONFIG_RV_REACTORS

#define DECLARE_RV_REACTING_HELPERS(name, type)							\
static char REACT_MSG_##name[1024];								\
												\
static inline char *format_react_msg_##name(type curr_state, type event)			\
{												\
	snprintf(REACT_MSG_##name, 1024,							\
		 "rv: monitor %s does not allow event %s on state %s\n",			\
		 #name,										\
		 model_get_event_name_##name(event),						\
		 model_get_state_name_##name(curr_state));					\
	return REACT_MSG_##name;								\
}												\
												\
static void cond_react_##name(char *msg)							\
{												\
	if (rv_##name.react)									\
		rv_##name.react(msg);								\
}												\
												\
static bool rv_reacting_on_##name(void)								\
{												\
	return rv_reacting_on();								\
}

#else 

#define DECLARE_RV_REACTING_HELPERS(name, type)							\
static inline char *format_react_msg_##name(type curr_state, type event)			\
{												\
	return NULL;										\
}												\
												\
static void cond_react_##name(char *msg)							\
{												\
	return;											\
}												\
												\
static bool rv_reacting_on_##name(void)								\
{												\
	return 0;										\
}
#endif


#define DECLARE_DA_MON_GENERIC_HELPERS(name, type)						\
												\
DECLARE_RV_REACTING_HELPERS(name, type)								\
												\
												\
static inline void da_monitor_reset_##name(struct da_monitor *da_mon)				\
{												\
	da_mon->monitoring = 0;									\
	da_mon->curr_state = model_get_initial_state_##name();					\
}												\
												\
												\
static inline type da_monitor_curr_state_##name(struct da_monitor *da_mon)			\
{												\
	return da_mon->curr_state;								\
}												\
												\
												\
static inline void										\
da_monitor_set_state_##name(struct da_monitor *da_mon, enum states_##name state)		\
{												\
	da_mon->curr_state = state;								\
}												\
												\
												\
static inline void da_monitor_start_##name(struct da_monitor *da_mon)				\
{												\
	da_mon->curr_state = model_get_initial_state_##name();					\
	da_mon->monitoring = 1;									\
}												\
												\
												\
static inline bool da_monitoring_##name(struct da_monitor *da_mon)				\
{												\
	return da_mon->monitoring;								\
}												\
												\
												\
static inline bool da_monitor_enabled_##name(void)						\
{												\
										\
	if (unlikely(!rv_monitoring_on()))							\
		return 0;									\
												\
										\
	if (unlikely(!rv_##name.enabled))							\
		return 0;									\
												\
	return 1;										\
}												\
												\
												\
static inline bool da_monitor_handling_event_##name(struct da_monitor *da_mon)			\
{												\
												\
	if (!da_monitor_enabled_##name())							\
		return 0;									\
												\
								\
	if (unlikely(!da_monitoring_##name(da_mon)))						\
		return 0;									\
												\
	return 1;										\
}


#define DECLARE_DA_MON_MODEL_HANDLER_IMPLICIT(name, type)					\
												\
static inline bool										\
da_event_##name(struct da_monitor *da_mon, enum events_##name event)				\
{												\
	type curr_state = da_monitor_curr_state_##name(da_mon);					\
	type next_state = model_get_next_state_##name(curr_state, event);			\
												\
	if (next_state != INVALID_STATE) {							\
		da_monitor_set_state_##name(da_mon, next_state);				\
												\
		trace_event_##name(model_get_state_name_##name(curr_state),			\
				   model_get_event_name_##name(event),				\
				   model_get_state_name_##name(next_state),			\
				   model_is_final_state_##name(next_state));			\
												\
		return true;									\
	}											\
												\
	if (rv_reacting_on_##name())								\
		cond_react_##name(format_react_msg_##name(curr_state, event));			\
												\
	trace_error_##name(model_get_state_name_##name(curr_state),				\
			   model_get_event_name_##name(event));					\
												\
	return false;										\
}												\


#define DECLARE_DA_MON_MODEL_HANDLER_PER_TASK(name, type)					\
												\
static inline bool da_event_##name(struct da_monitor *da_mon, struct task_struct *tsk,		\
				   enum events_##name event)					\
{												\
	type curr_state = da_monitor_curr_state_##name(da_mon);					\
	type next_state = model_get_next_state_##name(curr_state, event);			\
												\
	if (next_state != INVALID_STATE) {							\
		da_monitor_set_state_##name(da_mon, next_state);				\
												\
		trace_event_##name(tsk->pid,							\
				   model_get_state_name_##name(curr_state),			\
				   model_get_event_name_##name(event),				\
				   model_get_state_name_##name(next_state),			\
				   model_is_final_state_##name(next_state));			\
												\
		return true;									\
	}											\
												\
	if (rv_reacting_on_##name())								\
		cond_react_##name(format_react_msg_##name(curr_state, event));			\
												\
	trace_error_##name(tsk->pid,								\
			   model_get_state_name_##name(curr_state),				\
			   model_get_event_name_##name(event));					\
												\
	return false;										\
}


#define DECLARE_DA_MON_INIT_GLOBAL(name, type)							\
												\
												\
static struct da_monitor da_mon_##name;								\
												\
												\
static struct da_monitor *da_get_monitor_##name(void)						\
{												\
	return &da_mon_##name;									\
}												\
												\
												\
static void da_monitor_reset_all_##name(void)							\
{												\
	da_monitor_reset_##name(da_get_monitor_##name());					\
}												\
												\
												\
static inline int da_monitor_init_##name(void)							\
{												\
	da_monitor_reset_all_##name();								\
	return 0;										\
}												\
												\
												\
static inline void da_monitor_destroy_##name(void)						\
{												\
	return;											\
}


#define DECLARE_DA_MON_INIT_PER_CPU(name, type)							\
												\
												\
static DEFINE_PER_CPU(struct da_monitor, da_mon_##name);					\
												\
												\
static struct da_monitor *da_get_monitor_##name(void)						\
{												\
	return this_cpu_ptr(&da_mon_##name);							\
}												\
												\
												\
static void da_monitor_reset_all_##name(void)							\
{												\
	struct da_monitor *da_mon;								\
	int cpu;										\
	for_each_cpu(cpu, cpu_online_mask) {							\
		da_mon = per_cpu_ptr(&da_mon_##name, cpu);					\
		da_monitor_reset_##name(da_mon);						\
	}											\
}												\
												\
												\
static inline int da_monitor_init_##name(void)							\
{												\
	da_monitor_reset_all_##name();								\
	return 0;										\
}												\
												\
												\
static inline void da_monitor_destroy_##name(void)						\
{												\
	return;											\
}


#define DECLARE_DA_MON_INIT_PER_TASK(name, type)						\
												\
												\
static int task_mon_slot_##name = RV_PER_TASK_MONITOR_INIT;					\
												\
												\
static inline struct da_monitor *da_get_monitor_##name(struct task_struct *tsk)			\
{												\
	return &tsk->rv[task_mon_slot_##name].da_mon;						\
}												\
												\
static void da_monitor_reset_all_##name(void)							\
{												\
	struct task_struct *g, *p;								\
												\
	read_lock(&tasklist_lock);								\
	for_each_process_thread(g, p)								\
		da_monitor_reset_##name(da_get_monitor_##name(p));				\
	read_unlock(&tasklist_lock);								\
}												\
												\
												\
static int da_monitor_init_##name(void)								\
{												\
	int slot;										\
												\
	slot = rv_get_task_monitor_slot();							\
	if (slot < 0 || slot >= RV_PER_TASK_MONITOR_INIT)					\
		return slot;									\
												\
	task_mon_slot_##name = slot;								\
												\
	da_monitor_reset_all_##name();								\
	return 0;										\
}												\
												\
												\
static inline void da_monitor_destroy_##name(void)						\
{												\
	if (task_mon_slot_##name == RV_PER_TASK_MONITOR_INIT) {					\
		WARN_ONCE(1, "Disabling a disabled monitor: " #name);				\
		return;										\
	}											\
	rv_put_task_monitor_slot(task_mon_slot_##name);						\
	task_mon_slot_##name = RV_PER_TASK_MONITOR_INIT;					\
	return;											\
}


#define DECLARE_DA_MON_MONITOR_HANDLER_IMPLICIT(name, type)					\
												\
static inline void __da_handle_event_##name(struct da_monitor *da_mon,				\
					    enum events_##name event)				\
{												\
	bool retval;										\
												\
	retval = da_event_##name(da_mon, event);						\
	if (!retval)										\
		da_monitor_reset_##name(da_mon);						\
}												\
												\
												\
static inline void da_handle_event_##name(enum events_##name event)				\
{												\
	struct da_monitor *da_mon = da_get_monitor_##name();					\
	bool retval;										\
												\
	retval = da_monitor_handling_event_##name(da_mon);					\
	if (!retval)										\
		return;										\
												\
	__da_handle_event_##name(da_mon, event);						\
}												\
												\
												\
static inline bool da_handle_start_event_##name(enum events_##name event)			\
{												\
	struct da_monitor *da_mon;								\
												\
	if (!da_monitor_enabled_##name())							\
		return 0;									\
												\
	da_mon = da_get_monitor_##name();							\
												\
	if (unlikely(!da_monitoring_##name(da_mon))) {						\
		da_monitor_start_##name(da_mon);						\
		return 0;									\
	}											\
												\
	__da_handle_event_##name(da_mon, event);						\
												\
	return 1;										\
}												\
												\
												\
static inline bool da_handle_start_run_event_##name(enum events_##name event)			\
{												\
	struct da_monitor *da_mon;								\
												\
	if (!da_monitor_enabled_##name())							\
		return 0;									\
												\
	da_mon = da_get_monitor_##name();							\
												\
	if (unlikely(!da_monitoring_##name(da_mon)))						\
		da_monitor_start_##name(da_mon);						\
												\
	__da_handle_event_##name(da_mon, event);						\
												\
	return 1;										\
}


#define DECLARE_DA_MON_MONITOR_HANDLER_PER_TASK(name, type)					\
												\
static inline void										\
__da_handle_event_##name(struct da_monitor *da_mon, struct task_struct *tsk,			\
			 enum events_##name event)						\
{												\
	bool retval;										\
												\
	retval = da_event_##name(da_mon, tsk, event);						\
	if (!retval)										\
		da_monitor_reset_##name(da_mon);						\
}												\
												\
												\
static inline void										\
da_handle_event_##name(struct task_struct *tsk, enum events_##name event)			\
{												\
	struct da_monitor *da_mon = da_get_monitor_##name(tsk);					\
	bool retval;										\
												\
	retval = da_monitor_handling_event_##name(da_mon);					\
	if (!retval)										\
		return;										\
												\
	__da_handle_event_##name(da_mon, tsk, event);						\
}												\
												\
												\
static inline bool										\
da_handle_start_event_##name(struct task_struct *tsk, enum events_##name event)			\
{												\
	struct da_monitor *da_mon;								\
												\
	if (!da_monitor_enabled_##name())							\
		return 0;									\
												\
	da_mon = da_get_monitor_##name(tsk);							\
												\
	if (unlikely(!da_monitoring_##name(da_mon))) {						\
		da_monitor_start_##name(da_mon);						\
		return 0;									\
	}											\
												\
	__da_handle_event_##name(da_mon, tsk, event);						\
												\
	return 1;										\
}


#define DECLARE_DA_MON_GLOBAL(name, type)							\
												\
DECLARE_AUTOMATA_HELPERS(name, type)								\
DECLARE_DA_MON_GENERIC_HELPERS(name, type)							\
DECLARE_DA_MON_MODEL_HANDLER_IMPLICIT(name, type)						\
DECLARE_DA_MON_INIT_GLOBAL(name, type)								\
DECLARE_DA_MON_MONITOR_HANDLER_IMPLICIT(name, type)


#define DECLARE_DA_MON_PER_CPU(name, type)							\
												\
DECLARE_AUTOMATA_HELPERS(name, type)								\
DECLARE_DA_MON_GENERIC_HELPERS(name, type)							\
DECLARE_DA_MON_MODEL_HANDLER_IMPLICIT(name, type)						\
DECLARE_DA_MON_INIT_PER_CPU(name, type)								\
DECLARE_DA_MON_MONITOR_HANDLER_IMPLICIT(name, type)


#define DECLARE_DA_MON_PER_TASK(name, type)							\
												\
DECLARE_AUTOMATA_HELPERS(name, type)								\
DECLARE_DA_MON_GENERIC_HELPERS(name, type)							\
DECLARE_DA_MON_MODEL_HANDLER_PER_TASK(name, type)						\
DECLARE_DA_MON_INIT_PER_TASK(name, type)							\
DECLARE_DA_MON_MONITOR_HANDLER_PER_TASK(name, type)
