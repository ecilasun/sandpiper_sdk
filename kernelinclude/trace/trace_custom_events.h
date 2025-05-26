/* SPDX-License-Identifier: GPL-2.0 */


#include <linux/trace_events.h>


#undef TRACE_SYSTEM
#define TRACE_SYSTEM custom

#ifndef TRACE_SYSTEM_VAR
#define TRACE_SYSTEM_VAR TRACE_SYSTEM
#endif



#include "stages/init.h"

#undef TRACE_CUSTOM_EVENT
#define TRACE_CUSTOM_EVENT(name, proto, args, tstruct, assign, print) \
	DECLARE_CUSTOM_EVENT_CLASS(name,			      \
			     PARAMS(proto),		       \
			     PARAMS(args),		       \
			     PARAMS(tstruct),		       \
			     PARAMS(assign),		       \
			     PARAMS(print));		       \
	DEFINE_CUSTOM_EVENT(name, name, PARAMS(proto), PARAMS(args));



#include "stages/stage1_struct_define.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(name, proto, args, tstruct, assign, print) \
	struct trace_custom_event_raw_##name {				\
		struct trace_entry	ent;				\
		tstruct							\
		char			__data[];			\
	};								\
									\
	static struct trace_event_class custom_event_class_##name;

#undef DEFINE_CUSTOM_EVENT
#define DEFINE_CUSTOM_EVENT(template, name, proto, args)	\
	static struct trace_event_call	__used			\
	__attribute__((__aligned__(4))) custom_event_##name

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage2_data_offsets.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, assign, print)	\
	struct trace_custom_event_data_offsets_##call {			\
		tstruct;						\
	};

#undef DEFINE_CUSTOM_EVENT
#define DEFINE_CUSTOM_EVENT(template, name, proto, args)

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage3_trace_output.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, assign, print) \
static notrace enum print_line_t					\
trace_custom_raw_output_##call(struct trace_iterator *iter, int flags,	\
			struct trace_event *trace_event)		\
{									\
	struct trace_seq *s = &iter->seq;				\
	struct trace_seq __maybe_unused *p = &iter->tmp_seq;		\
	struct trace_custom_event_raw_##call *field;			\
	int ret;							\
									\
	field = (typeof(field))iter->ent;				\
									\
	ret = trace_raw_output_prep(iter, trace_event);			\
	if (ret != TRACE_TYPE_HANDLED)					\
		return ret;						\
									\
	trace_event_printf(iter, print);				\
									\
	return trace_handle_return(s);					\
}									\
static struct trace_event_functions trace_custom_event_type_funcs_##call = { \
	.trace			= trace_custom_raw_output_##call,	\
};

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage4_event_fields.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, func, print)	\
static struct trace_event_fields trace_custom_event_fields_##call[] = {	\
	tstruct								\
	{} };

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage5_get_offsets.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, assign, print) \
static inline notrace int trace_custom_event_get_offsets_##call(	\
	struct trace_custom_event_data_offsets_##call *__data_offsets, proto) \
{									\
	int __data_size = 0;						\
	int __maybe_unused __item_length;				\
	struct trace_custom_event_raw_##call __maybe_unused *entry;	\
									\
	tstruct;							\
									\
	return __data_size;						\
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage6_event_callback.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, assign, print) \
									\
static notrace void							\
trace_custom_event_raw_event_##call(void *__data, proto)		\
{									\
	struct trace_event_file *trace_file = __data;			\
	struct trace_custom_event_data_offsets_##call __maybe_unused __data_offsets; \
	struct trace_event_buffer fbuffer;				\
	struct trace_custom_event_raw_##call *entry;			\
	int __data_size;						\
									\
	if (trace_trigger_soft_disabled(trace_file))			\
		return;							\
									\
	__data_size = trace_custom_event_get_offsets_##call(&__data_offsets, args); \
									\
	entry = trace_event_buffer_reserve(&fbuffer, trace_file,	\
				 sizeof(*entry) + __data_size);		\
									\
	if (!entry)							\
		return;							\
									\
	tstruct								\
									\
	{ assign; }							\
									\
	trace_event_buffer_commit(&fbuffer);				\
}


#undef DEFINE_CUSTOM_EVENT
#define DEFINE_CUSTOM_EVENT(template, call, proto, args)		\
static inline void ftrace_test_custom_probe_##call(void)		\
{									\
	check_trace_callback_type_##call(trace_custom_event_raw_event_##template); \
}

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)



#include "stages/stage7_class_define.h"

#undef DECLARE_CUSTOM_EVENT_CLASS
#define DECLARE_CUSTOM_EVENT_CLASS(call, proto, args, tstruct, assign, print) \
static char custom_print_fmt_##call[] = print;					\
static struct trace_event_class __used __refdata custom_event_class_##call = { \
	.system			= TRACE_SYSTEM_STRING,			\
	.fields_array		= trace_custom_event_fields_##call,		\
	.fields			= LIST_HEAD_INIT(custom_event_class_##call.fields),\
	.raw_init		= trace_event_raw_init,			\
	.probe			= trace_custom_event_raw_event_##call,	\
	.reg			= trace_event_reg,			\
};

#undef DEFINE_CUSTOM_EVENT
#define DEFINE_CUSTOM_EVENT(template, call, proto, args)		\
									\
static struct trace_event_call __used custom_event_##call = {		\
	.name			= #call,				\
	.class			= &custom_event_class_##template,	\
	.event.funcs		= &trace_custom_event_type_funcs_##template, \
	.print_fmt		= custom_print_fmt_##template,		\
	.flags			= TRACE_EVENT_FL_CUSTOM,		\
};									\
static inline int trace_custom_event_##call##_update(struct tracepoint *tp) \
{									\
	if (tp->name && strcmp(tp->name, #call) == 0) {			\
		custom_event_##call.tp = tp;				\
		custom_event_##call.flags = TRACE_EVENT_FL_TRACEPOINT;	\
		return 1;						\
	}								\
	return 0;							\
}									\
static struct trace_event_call __used					\
__section("_ftrace_events") *__custom_event_##call = &custom_event_##call

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)
