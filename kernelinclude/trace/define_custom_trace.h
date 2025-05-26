/* SPDX-License-Identifier: GPL-2.0 */


#ifdef CREATE_CUSTOM_TRACE_EVENTS


#undef CREATE_CUSTOM_TRACE_EVENTS

#include <linux/stringify.h>

#undef TRACE_CUSTOM_EVENT
#define TRACE_CUSTOM_EVENT(name, proto, args, tstruct, assign, print)

#undef DEFINE_CUSTOM_EVENT
#define DEFINE_CUSTOM_EVENT(template, name, proto, args)

#undef TRACE_INCLUDE
#undef __TRACE_INCLUDE

#ifndef TRACE_INCLUDE_FILE
# define TRACE_INCLUDE_FILE TRACE_SYSTEM
# define UNDEF_TRACE_INCLUDE_FILE
#endif

#ifndef TRACE_INCLUDE_PATH
# define __TRACE_INCLUDE(system) <trace/events/system.h>
# define UNDEF_TRACE_INCLUDE_PATH
#else
# define __TRACE_INCLUDE(system) __stringify(TRACE_INCLUDE_PATH/system.h)
#endif

# define TRACE_INCLUDE(system) __TRACE_INCLUDE(system)


#define TRACE_CUSTOM_MULTI_READ

#include TRACE_INCLUDE(TRACE_INCLUDE_FILE)

#ifdef TRACEPOINTS_ENABLED
#include <trace/trace_custom_events.h>
#endif

#undef TRACE_CUSTOM_EVENT
#undef DECLARE_CUSTOM_EVENT_CLASS
#undef DEFINE_CUSTOM_EVENT
#undef TRACE_CUSTOM_MULTI_READ


#ifdef UNDEF_TRACE_INCLUDE_FILE
# undef TRACE_INCLUDE_FILE
# undef UNDEF_TRACE_INCLUDE_FILE
#endif

#ifdef UNDEF_TRACE_INCLUDE_PATH
# undef TRACE_INCLUDE_PATH
# undef UNDEF_TRACE_INCLUDE_PATH
#endif


#define CREATE_CUSTOM_TRACE_POINTS

#endif 
