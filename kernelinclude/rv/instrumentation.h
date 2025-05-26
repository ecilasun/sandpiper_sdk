/* SPDX-License-Identifier: GPL-2.0 */


#include <linux/ftrace.h>


#define rv_attach_trace_probe(monitor, tp, rv_handler)					\
	do {										\
		check_trace_callback_type_##tp(rv_handler);				\
		WARN_ONCE(register_trace_##tp(rv_handler, NULL),			\
				"fail attaching " #monitor " " #tp "handler");		\
	} while (0)


#define rv_detach_trace_probe(monitor, tp, rv_handler)					\
	do {										\
		unregister_trace_##tp(rv_handler, NULL);				\
	} while (0)
