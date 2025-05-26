/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef __HID_BPF_H
#define __HID_BPF_H

#include <linux/bpf.h>
#include <linux/spinlock.h>
#include <uapi/linux/hid.h>

struct hid_device;




struct hid_bpf_ctx {
	__u32 index;
	const struct hid_device *hid;
	__u32 allocated_size;
	enum hid_report_type report_type;
	union {
		__s32 retval;
		__s32 size;
	};
};


enum hid_bpf_attach_flags {
	HID_BPF_FLAG_NONE = 0,
	HID_BPF_FLAG_INSERT_HEAD = _BITUL(0),
	HID_BPF_FLAG_MAX,
};


int hid_bpf_device_event(struct hid_bpf_ctx *ctx);
int hid_bpf_rdesc_fixup(struct hid_bpf_ctx *ctx);



__u8 *hid_bpf_get_data(struct hid_bpf_ctx *ctx, unsigned int offset, const size_t __sz);


int hid_bpf_attach_prog(unsigned int hid_id, int prog_fd, __u32 flags);
int hid_bpf_hw_request(struct hid_bpf_ctx *ctx, __u8 *buf, size_t buf__sz,
		       enum hid_report_type rtype, enum hid_class_request reqtype);
struct hid_bpf_ctx *hid_bpf_allocate_context(unsigned int hid_id);
void hid_bpf_release_context(struct hid_bpf_ctx *ctx);




int __hid_bpf_tail_call(struct hid_bpf_ctx *ctx);

#define HID_BPF_MAX_PROGS_PER_DEV 64
#define HID_BPF_FLAG_MASK (((HID_BPF_FLAG_MAX - 1) << 1) - 1)


enum hid_bpf_prog_type {
	HID_BPF_PROG_TYPE_UNDEF = -1,
	HID_BPF_PROG_TYPE_DEVICE_EVENT,			
	HID_BPF_PROG_TYPE_RDESC_FIXUP,
	HID_BPF_PROG_TYPE_MAX,
};

struct hid_report_enum;

struct hid_bpf_ops {
	struct hid_report *(*hid_get_report)(struct hid_report_enum *report_enum, const u8 *data);
	int (*hid_hw_raw_request)(struct hid_device *hdev,
				  unsigned char reportnum, __u8 *buf,
				  size_t len, enum hid_report_type rtype,
				  enum hid_class_request reqtype);
	struct module *owner;
	struct bus_type *bus_type;
};

extern struct hid_bpf_ops *hid_bpf_ops;

struct hid_bpf_prog_list {
	u16 prog_idx[HID_BPF_MAX_PROGS_PER_DEV];
	u8 prog_cnt;
};


struct hid_bpf {
	u8 *device_data;		
	u32 allocated_data;

	struct hid_bpf_prog_list __rcu *progs[HID_BPF_PROG_TYPE_MAX];	
	bool destroyed;			

	spinlock_t progs_lock;		
};


struct hid_bpf_link {
	struct bpf_link link;
	int hid_table_index;
};

#ifdef CONFIG_HID_BPF
u8 *dispatch_hid_bpf_device_event(struct hid_device *hid, enum hid_report_type type, u8 *data,
				  u32 *size, int interrupt);
int hid_bpf_connect_device(struct hid_device *hdev);
void hid_bpf_disconnect_device(struct hid_device *hdev);
void hid_bpf_destroy_device(struct hid_device *hid);
void hid_bpf_device_init(struct hid_device *hid);
u8 *call_hid_bpf_rdesc_fixup(struct hid_device *hdev, u8 *rdesc, unsigned int *size);
#else 
static inline u8 *dispatch_hid_bpf_device_event(struct hid_device *hid, enum hid_report_type type,
						u8 *data, u32 *size, int interrupt) { return data; }
static inline int hid_bpf_connect_device(struct hid_device *hdev) { return 0; }
static inline void hid_bpf_disconnect_device(struct hid_device *hdev) {}
static inline void hid_bpf_destroy_device(struct hid_device *hid) {}
static inline void hid_bpf_device_init(struct hid_device *hid) {}
static inline u8 *call_hid_bpf_rdesc_fixup(struct hid_device *hdev, u8 *rdesc, unsigned int *size)
{
	return kmemdup(rdesc, *size, GFP_KERNEL);
}

#endif 

#endif 
