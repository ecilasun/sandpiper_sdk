/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <uapi/linux/counter.h>

struct counter_device;
struct counter_count;
struct counter_synapse;
struct counter_signal;

enum counter_comp_type {
	COUNTER_COMP_U8,
	COUNTER_COMP_U64,
	COUNTER_COMP_BOOL,
	COUNTER_COMP_SIGNAL_LEVEL,
	COUNTER_COMP_FUNCTION,
	COUNTER_COMP_SYNAPSE_ACTION,
	COUNTER_COMP_ENUM,
	COUNTER_COMP_COUNT_DIRECTION,
	COUNTER_COMP_COUNT_MODE,
	COUNTER_COMP_SIGNAL_POLARITY,
	COUNTER_COMP_ARRAY,
};


struct counter_comp {
	enum counter_comp_type type;
	const char *name;
	void *priv;
	union {
		int (*action_read)(struct counter_device *counter,
				   struct counter_count *count,
				   struct counter_synapse *synapse,
				   enum counter_synapse_action *action);
		int (*device_u8_read)(struct counter_device *counter, u8 *val);
		int (*count_u8_read)(struct counter_device *counter,
				     struct counter_count *count, u8 *val);
		int (*signal_u8_read)(struct counter_device *counter,
				      struct counter_signal *signal, u8 *val);
		int (*device_u32_read)(struct counter_device *counter,
				       u32 *val);
		int (*count_u32_read)(struct counter_device *counter,
				      struct counter_count *count, u32 *val);
		int (*signal_u32_read)(struct counter_device *counter,
				       struct counter_signal *signal, u32 *val);
		int (*device_u64_read)(struct counter_device *counter,
				       u64 *val);
		int (*count_u64_read)(struct counter_device *counter,
				      struct counter_count *count, u64 *val);
		int (*signal_u64_read)(struct counter_device *counter,
				       struct counter_signal *signal, u64 *val);
		int (*signal_array_u32_read)(struct counter_device *counter,
					     struct counter_signal *signal,
					     size_t idx, u32 *val);
		int (*device_array_u64_read)(struct counter_device *counter,
					     size_t idx, u64 *val);
		int (*count_array_u64_read)(struct counter_device *counter,
					    struct counter_count *count,
					    size_t idx, u64 *val);
		int (*signal_array_u64_read)(struct counter_device *counter,
					     struct counter_signal *signal,
					     size_t idx, u64 *val);
	};
	union {
		int (*action_write)(struct counter_device *counter,
				    struct counter_count *count,
				    struct counter_synapse *synapse,
				    enum counter_synapse_action action);
		int (*device_u8_write)(struct counter_device *counter, u8 val);
		int (*count_u8_write)(struct counter_device *counter,
				      struct counter_count *count, u8 val);
		int (*signal_u8_write)(struct counter_device *counter,
				       struct counter_signal *signal, u8 val);
		int (*device_u32_write)(struct counter_device *counter,
					u32 val);
		int (*count_u32_write)(struct counter_device *counter,
				       struct counter_count *count, u32 val);
		int (*signal_u32_write)(struct counter_device *counter,
					struct counter_signal *signal, u32 val);
		int (*device_u64_write)(struct counter_device *counter,
					u64 val);
		int (*count_u64_write)(struct counter_device *counter,
				       struct counter_count *count, u64 val);
		int (*signal_u64_write)(struct counter_device *counter,
					struct counter_signal *signal, u64 val);
		int (*signal_array_u32_write)(struct counter_device *counter,
					      struct counter_signal *signal,
					      size_t idx, u32 val);
		int (*device_array_u64_write)(struct counter_device *counter,
					      size_t idx, u64 val);
		int (*count_array_u64_write)(struct counter_device *counter,
					     struct counter_count *count,
					     size_t idx, u64 val);
		int (*signal_array_u64_write)(struct counter_device *counter,
					      struct counter_signal *signal,
					      size_t idx, u64 val);
	};
};


struct counter_signal {
	int id;
	const char *name;

	struct counter_comp *ext;
	size_t num_ext;
};


struct counter_synapse {
	const enum counter_synapse_action *actions_list;
	size_t num_actions;

	struct counter_signal *signal;
};


struct counter_count {
	int id;
	const char *name;

	const enum counter_function *functions_list;
	size_t num_functions;

	struct counter_synapse *synapses;
	size_t num_synapses;

	struct counter_comp *ext;
	size_t num_ext;
};


struct counter_event_node {
	struct list_head l;
	u8 event;
	u8 channel;
	struct list_head comp_list;
};


struct counter_ops {
	int (*signal_read)(struct counter_device *counter,
			   struct counter_signal *signal,
			   enum counter_signal_level *level);
	int (*count_read)(struct counter_device *counter,
			  struct counter_count *count, u64 *value);
	int (*count_write)(struct counter_device *counter,
			   struct counter_count *count, u64 value);
	int (*function_read)(struct counter_device *counter,
			     struct counter_count *count,
			     enum counter_function *function);
	int (*function_write)(struct counter_device *counter,
			      struct counter_count *count,
			      enum counter_function function);
	int (*action_read)(struct counter_device *counter,
			   struct counter_count *count,
			   struct counter_synapse *synapse,
			   enum counter_synapse_action *action);
	int (*action_write)(struct counter_device *counter,
			    struct counter_count *count,
			    struct counter_synapse *synapse,
			    enum counter_synapse_action action);
	int (*events_configure)(struct counter_device *counter);
	int (*watch_validate)(struct counter_device *counter,
			      const struct counter_watch *watch);
};


struct counter_device {
	const char *name;
	struct device *parent;

	const struct counter_ops *ops;

	struct counter_signal *signals;
	size_t num_signals;
	struct counter_count *counts;
	size_t num_counts;

	struct counter_comp *ext;
	size_t num_ext;

	struct device dev;
	struct cdev chrdev;
	struct list_head events_list;
	spinlock_t events_list_lock;
	struct list_head next_events_list;
	struct mutex n_events_list_lock;
	DECLARE_KFIFO_PTR(events, struct counter_event);
	wait_queue_head_t events_wait;
	spinlock_t events_in_lock;
	struct mutex events_out_lock;
	struct mutex ops_exist_lock;
};

void *counter_priv(const struct counter_device *const counter) __attribute_const__;

struct counter_device *counter_alloc(size_t sizeof_priv);
void counter_put(struct counter_device *const counter);
int counter_add(struct counter_device *const counter);

void counter_unregister(struct counter_device *const counter);
struct counter_device *devm_counter_alloc(struct device *dev,
					  size_t sizeof_priv);
int devm_counter_add(struct device *dev,
		     struct counter_device *const counter);
void counter_push_event(struct counter_device *const counter, const u8 event,
			const u8 channel);

#define COUNTER_COMP_DEVICE_U8(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U8, \
	.name = (_name), \
	.device_u8_read = (_read), \
	.device_u8_write = (_write), \
}
#define COUNTER_COMP_COUNT_U8(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U8, \
	.name = (_name), \
	.count_u8_read = (_read), \
	.count_u8_write = (_write), \
}
#define COUNTER_COMP_SIGNAL_U8(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U8, \
	.name = (_name), \
	.signal_u8_read = (_read), \
	.signal_u8_write = (_write), \
}

#define COUNTER_COMP_DEVICE_U64(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U64, \
	.name = (_name), \
	.device_u64_read = (_read), \
	.device_u64_write = (_write), \
}
#define COUNTER_COMP_COUNT_U64(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U64, \
	.name = (_name), \
	.count_u64_read = (_read), \
	.count_u64_write = (_write), \
}
#define COUNTER_COMP_SIGNAL_U64(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_U64, \
	.name = (_name), \
	.signal_u64_read = (_read), \
	.signal_u64_write = (_write), \
}

#define COUNTER_COMP_DEVICE_BOOL(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_BOOL, \
	.name = (_name), \
	.device_u8_read = (_read), \
	.device_u8_write = (_write), \
}
#define COUNTER_COMP_COUNT_BOOL(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_BOOL, \
	.name = (_name), \
	.count_u8_read = (_read), \
	.count_u8_write = (_write), \
}
#define COUNTER_COMP_SIGNAL_BOOL(_name, _read, _write) \
{ \
	.type = COUNTER_COMP_BOOL, \
	.name = (_name), \
	.signal_u8_read = (_read), \
	.signal_u8_write = (_write), \
}

struct counter_available {
	union {
		const u32 *enums;
		const char *const *strs;
	};
	size_t num_items;
};

#define DEFINE_COUNTER_AVAILABLE(_name, _enums) \
	struct counter_available _name = { \
		.enums = (_enums), \
		.num_items = ARRAY_SIZE(_enums), \
	}

#define DEFINE_COUNTER_ENUM(_name, _strs) \
	struct counter_available _name = { \
		.strs = (_strs), \
		.num_items = ARRAY_SIZE(_strs), \
	}

#define COUNTER_COMP_DEVICE_ENUM(_name, _get, _set, _available) \
{ \
	.type = COUNTER_COMP_ENUM, \
	.name = (_name), \
	.device_u32_read = (_get), \
	.device_u32_write = (_set), \
	.priv = &(_available), \
}
#define COUNTER_COMP_COUNT_ENUM(_name, _get, _set, _available) \
{ \
	.type = COUNTER_COMP_ENUM, \
	.name = (_name), \
	.count_u32_read = (_get), \
	.count_u32_write = (_set), \
	.priv = &(_available), \
}
#define COUNTER_COMP_SIGNAL_ENUM(_name, _get, _set, _available) \
{ \
	.type = COUNTER_COMP_ENUM, \
	.name = (_name), \
	.signal_u32_read = (_get), \
	.signal_u32_write = (_set), \
	.priv = &(_available), \
}

struct counter_array {
	enum counter_comp_type type;
	const struct counter_available *avail;
	union {
		size_t length;
		size_t idx;
	};
};

#define DEFINE_COUNTER_ARRAY_U64(_name, _length) \
	struct counter_array _name = { \
		.type = COUNTER_COMP_U64, \
		.length = (_length), \
	}

#define DEFINE_COUNTER_ARRAY_CAPTURE(_name, _length) \
	DEFINE_COUNTER_ARRAY_U64(_name, _length)

#define DEFINE_COUNTER_ARRAY_POLARITY(_name, _available, _length) \
	struct counter_array _name = { \
		.type = COUNTER_COMP_SIGNAL_POLARITY, \
		.avail = &(_available), \
		.length = (_length), \
	}

#define COUNTER_COMP_DEVICE_ARRAY_U64(_name, _read, _write, _array) \
{ \
	.type = COUNTER_COMP_ARRAY, \
	.name = (_name), \
	.device_array_u64_read = (_read), \
	.device_array_u64_write = (_write), \
	.priv = &(_array), \
}
#define COUNTER_COMP_COUNT_ARRAY_U64(_name, _read, _write, _array) \
{ \
	.type = COUNTER_COMP_ARRAY, \
	.name = (_name), \
	.count_array_u64_read = (_read), \
	.count_array_u64_write = (_write), \
	.priv = &(_array), \
}
#define COUNTER_COMP_SIGNAL_ARRAY_U64(_name, _read, _write, _array) \
{ \
	.type = COUNTER_COMP_ARRAY, \
	.name = (_name), \
	.signal_array_u64_read = (_read), \
	.signal_array_u64_write = (_write), \
	.priv = &(_array), \
}

#define COUNTER_COMP_CAPTURE(_read, _write) \
	COUNTER_COMP_COUNT_U64("capture", _read, _write)

#define COUNTER_COMP_CEILING(_read, _write) \
	COUNTER_COMP_COUNT_U64("ceiling", _read, _write)

#define COUNTER_COMP_COUNT_MODE(_read, _write, _available) \
{ \
	.type = COUNTER_COMP_COUNT_MODE, \
	.name = "count_mode", \
	.count_u32_read = (_read), \
	.count_u32_write = (_write), \
	.priv = &(_available), \
}

#define COUNTER_COMP_DIRECTION(_read) \
{ \
	.type = COUNTER_COMP_COUNT_DIRECTION, \
	.name = "direction", \
	.count_u32_read = (_read), \
}

#define COUNTER_COMP_ENABLE(_read, _write) \
	COUNTER_COMP_COUNT_BOOL("enable", _read, _write)

#define COUNTER_COMP_FLOOR(_read, _write) \
	COUNTER_COMP_COUNT_U64("floor", _read, _write)

#define COUNTER_COMP_POLARITY(_read, _write, _available) \
{ \
	.type = COUNTER_COMP_SIGNAL_POLARITY, \
	.name = "polarity", \
	.signal_u32_read = (_read), \
	.signal_u32_write = (_write), \
	.priv = &(_available), \
}

#define COUNTER_COMP_PRESET(_read, _write) \
	COUNTER_COMP_COUNT_U64("preset", _read, _write)

#define COUNTER_COMP_PRESET_ENABLE(_read, _write) \
	COUNTER_COMP_COUNT_BOOL("preset_enable", _read, _write)

#define COUNTER_COMP_ARRAY_CAPTURE(_read, _write, _array) \
	COUNTER_COMP_COUNT_ARRAY_U64("capture", _read, _write, _array)

#define COUNTER_COMP_ARRAY_POLARITY(_read, _write, _array) \
{ \
	.type = COUNTER_COMP_ARRAY, \
	.name = "polarity", \
	.signal_array_u32_read = (_read), \
	.signal_array_u32_write = (_write), \
	.priv = &(_array), \
}

#endif 
