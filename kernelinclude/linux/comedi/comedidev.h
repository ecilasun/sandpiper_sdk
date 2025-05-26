/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _COMEDIDEV_H
#define _COMEDIDEV_H

#include <linux/dma-mapping.h>
#include <linux/mutex.h>
#include <linux/spinlock_types.h>
#include <linux/rwsem.h>
#include <linux/kref.h>
#include <linux/comedi.h>

#define COMEDI_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#define COMEDI_VERSION_CODE COMEDI_VERSION(COMEDI_MAJORVERSION, \
	COMEDI_MINORVERSION, COMEDI_MICROVERSION)
#define COMEDI_RELEASE VERSION

#define COMEDI_NUM_BOARD_MINORS 0x30


struct comedi_subdevice {
	struct comedi_device *device;
	int index;
	int type;
	int n_chan;
	int subdev_flags;
	int len_chanlist;	

	void *private;

	struct comedi_async *async;

	void *lock;
	void *busy;
	unsigned int runflags;
	spinlock_t spin_lock;	

	unsigned int io_bits;

	unsigned int maxdata;	
	const unsigned int *maxdata_list;	

	const struct comedi_lrange *range_table;
	const struct comedi_lrange *const *range_table_list;

	unsigned int *chanlist;	

	int (*insn_read)(struct comedi_device *dev, struct comedi_subdevice *s,
			 struct comedi_insn *insn, unsigned int *data);
	int (*insn_write)(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data);
	int (*insn_bits)(struct comedi_device *dev, struct comedi_subdevice *s,
			 struct comedi_insn *insn, unsigned int *data);
	int (*insn_config)(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_insn *insn,
			   unsigned int *data);

	int (*do_cmd)(struct comedi_device *dev, struct comedi_subdevice *s);
	int (*do_cmdtest)(struct comedi_device *dev,
			  struct comedi_subdevice *s,
			  struct comedi_cmd *cmd);
	int (*poll)(struct comedi_device *dev, struct comedi_subdevice *s);
	int (*cancel)(struct comedi_device *dev, struct comedi_subdevice *s);

	
	int (*buf_change)(struct comedi_device *dev,
			  struct comedi_subdevice *s);

	void (*munge)(struct comedi_device *dev, struct comedi_subdevice *s,
		      void *data, unsigned int num_bytes,
		      unsigned int start_chan_index);
	enum dma_data_direction async_dma_dir;

	unsigned int state;

	struct device *class_dev;
	int minor;

	unsigned int *readback;
};


struct comedi_buf_page {
	void *virt_addr;
	dma_addr_t dma_addr;
};


struct comedi_buf_map {
	struct device *dma_hw_dev;
	struct comedi_buf_page *page_list;
	unsigned int n_pages;
	enum dma_data_direction dma_dir;
	struct kref refcount;
};


struct comedi_async {
	void *prealloc_buf;
	unsigned int prealloc_bufsz;
	struct comedi_buf_map *buf_map;
	unsigned int max_bufsize;
	unsigned int buf_write_count;
	unsigned int buf_write_alloc_count;
	unsigned int buf_read_count;
	unsigned int buf_read_alloc_count;
	unsigned int buf_write_ptr;
	unsigned int buf_read_ptr;
	unsigned int cur_chan;
	unsigned int scans_done;
	unsigned int scan_progress;
	unsigned int munge_chan;
	unsigned int munge_count;
	unsigned int munge_ptr;
	unsigned int events;
	struct comedi_cmd cmd;
	wait_queue_head_t wait_head;
	unsigned int cb_mask;
	int (*inttrig)(struct comedi_device *dev, struct comedi_subdevice *s,
		       unsigned int x);
};


enum comedi_cb {
	COMEDI_CB_EOS		= BIT(0),
	COMEDI_CB_EOA		= BIT(1),
	COMEDI_CB_BLOCK		= BIT(2),
	COMEDI_CB_EOBUF		= BIT(3),
	COMEDI_CB_ERROR		= BIT(4),
	COMEDI_CB_OVERFLOW	= BIT(5),
	
	COMEDI_CB_ERROR_MASK	= (COMEDI_CB_ERROR | COMEDI_CB_OVERFLOW),
	COMEDI_CB_CANCEL_MASK	= (COMEDI_CB_EOA | COMEDI_CB_ERROR_MASK)
};


struct comedi_driver {
	
	struct comedi_driver *next;	
	
	const char *driver_name;
	struct module *module;
	int (*attach)(struct comedi_device *dev, struct comedi_devconfig *it);
	void (*detach)(struct comedi_device *dev);
	int (*auto_attach)(struct comedi_device *dev, unsigned long context);
	unsigned int num_names;
	const char *const *board_name;
	int offset;
};


struct comedi_device {
	int use_count;
	struct comedi_driver *driver;
	struct comedi_8254 *pacer;
	void *private;

	struct device *class_dev;
	int minor;
	unsigned int detach_count;
	struct device *hw_dev;

	const char *board_name;
	const void *board_ptr;
	unsigned int attached:1;
	unsigned int ioenabled:1;
	spinlock_t spinlock;	
	struct mutex mutex;	
	struct rw_semaphore attach_lock;
	struct kref refcount;

	int n_subdevices;
	struct comedi_subdevice *subdevices;

	
	void __iomem *mmio;
	unsigned long iobase;
	unsigned long iolen;
	unsigned int irq;

	struct comedi_subdevice *read_subdev;
	struct comedi_subdevice *write_subdev;

	struct fasync_struct *async_queue;

	int (*open)(struct comedi_device *dev);
	void (*close)(struct comedi_device *dev);
	int (*insn_device_config)(struct comedi_device *dev,
				  struct comedi_insn *insn, unsigned int *data);
	unsigned int (*get_valid_routes)(struct comedi_device *dev,
					 unsigned int n_pairs,
					 unsigned int *pair_data);
};



void comedi_event(struct comedi_device *dev, struct comedi_subdevice *s);

struct comedi_device *comedi_dev_get_from_minor(unsigned int minor);
int comedi_dev_put(struct comedi_device *dev);

bool comedi_is_subdevice_running(struct comedi_subdevice *s);

void *comedi_alloc_spriv(struct comedi_subdevice *s, size_t size);
void comedi_set_spriv_auto_free(struct comedi_subdevice *s);

int comedi_check_chanlist(struct comedi_subdevice *s,
			  int n,
			  unsigned int *chanlist);



#define RANGE(a, b)		{(a) * 1e6, (b) * 1e6, 0}
#define RANGE_ext(a, b)		{(a) * 1e6, (b) * 1e6, RF_EXTERNAL}
#define RANGE_mA(a, b)		{(a) * 1e6, (b) * 1e6, UNIT_mA}
#define RANGE_unitless(a, b)	{(a) * 1e6, (b) * 1e6, 0}
#define BIP_RANGE(a)		{-(a) * 1e6, (a) * 1e6, 0}
#define UNI_RANGE(a)		{0, (a) * 1e6, 0}

extern const struct comedi_lrange range_bipolar10;
extern const struct comedi_lrange range_bipolar5;
extern const struct comedi_lrange range_bipolar2_5;
extern const struct comedi_lrange range_unipolar10;
extern const struct comedi_lrange range_unipolar5;
extern const struct comedi_lrange range_unipolar2_5;
extern const struct comedi_lrange range_0_20mA;
extern const struct comedi_lrange range_4_20mA;
extern const struct comedi_lrange range_0_32mA;
extern const struct comedi_lrange range_unknown;

#define range_digital		range_unipolar5


struct comedi_lrange {
	int length;
	struct comedi_krange range[];
};


static inline bool comedi_range_is_bipolar(struct comedi_subdevice *s,
					   unsigned int range)
{
	return s->range_table->range[range].min < 0;
}


static inline bool comedi_range_is_unipolar(struct comedi_subdevice *s,
					    unsigned int range)
{
	return s->range_table->range[range].min >= 0;
}


static inline bool comedi_range_is_external(struct comedi_subdevice *s,
					    unsigned int range)
{
	return !!(s->range_table->range[range].flags & RF_EXTERNAL);
}


static inline bool comedi_chan_range_is_bipolar(struct comedi_subdevice *s,
						unsigned int chan,
						unsigned int range)
{
	return s->range_table_list[chan]->range[range].min < 0;
}


static inline bool comedi_chan_range_is_unipolar(struct comedi_subdevice *s,
						 unsigned int chan,
						 unsigned int range)
{
	return s->range_table_list[chan]->range[range].min >= 0;
}


static inline bool comedi_chan_range_is_external(struct comedi_subdevice *s,
						 unsigned int chan,
						 unsigned int range)
{
	return !!(s->range_table_list[chan]->range[range].flags & RF_EXTERNAL);
}


static inline unsigned int comedi_offset_munge(struct comedi_subdevice *s,
					       unsigned int val)
{
	return val ^ s->maxdata ^ (s->maxdata >> 1);
}


static inline unsigned int comedi_bytes_per_sample(struct comedi_subdevice *s)
{
	return s->subdev_flags & SDF_LSAMPL ? sizeof(int) : sizeof(short);
}


static inline unsigned int comedi_sample_shift(struct comedi_subdevice *s)
{
	return s->subdev_flags & SDF_LSAMPL ? 2 : 1;
}


static inline unsigned int comedi_bytes_to_samples(struct comedi_subdevice *s,
						   unsigned int nbytes)
{
	return nbytes >> comedi_sample_shift(s);
}


static inline unsigned int comedi_samples_to_bytes(struct comedi_subdevice *s,
						   unsigned int nsamples)
{
	return nsamples << comedi_sample_shift(s);
}


static inline int comedi_check_trigger_src(unsigned int *src,
					   unsigned int flags)
{
	unsigned int orig_src = *src;

	*src = orig_src & flags;
	if (*src == TRIG_INVALID || *src != orig_src)
		return -EINVAL;
	return 0;
}


static inline int comedi_check_trigger_is_unique(unsigned int src)
{
	
	if ((src & (src - 1)) != 0)
		return -EINVAL;
	return 0;
}


static inline int comedi_check_trigger_arg_is(unsigned int *arg,
					      unsigned int val)
{
	if (*arg != val) {
		*arg = val;
		return -EINVAL;
	}
	return 0;
}


static inline int comedi_check_trigger_arg_min(unsigned int *arg,
					       unsigned int val)
{
	if (*arg < val) {
		*arg = val;
		return -EINVAL;
	}
	return 0;
}


static inline int comedi_check_trigger_arg_max(unsigned int *arg,
					       unsigned int val)
{
	if (*arg > val) {
		*arg = val;
		return -EINVAL;
	}
	return 0;
}


int comedi_set_hw_dev(struct comedi_device *dev, struct device *hw_dev);


static inline unsigned int comedi_buf_n_bytes_ready(struct comedi_subdevice *s)
{
	return s->async->buf_write_count - s->async->buf_read_count;
}

unsigned int comedi_buf_write_alloc(struct comedi_subdevice *s, unsigned int n);
unsigned int comedi_buf_write_free(struct comedi_subdevice *s, unsigned int n);

unsigned int comedi_buf_read_n_available(struct comedi_subdevice *s);
unsigned int comedi_buf_read_alloc(struct comedi_subdevice *s, unsigned int n);
unsigned int comedi_buf_read_free(struct comedi_subdevice *s, unsigned int n);

unsigned int comedi_buf_write_samples(struct comedi_subdevice *s,
				      const void *data, unsigned int nsamples);
unsigned int comedi_buf_read_samples(struct comedi_subdevice *s,
				     void *data, unsigned int nsamples);



#define COMEDI_TIMEOUT_MS	1000

int comedi_timeout(struct comedi_device *dev, struct comedi_subdevice *s,
		   struct comedi_insn *insn,
		   int (*cb)(struct comedi_device *dev,
			     struct comedi_subdevice *s,
			     struct comedi_insn *insn, unsigned long context),
		   unsigned long context);

unsigned int comedi_handle_events(struct comedi_device *dev,
				  struct comedi_subdevice *s);

int comedi_dio_insn_config(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_insn *insn, unsigned int *data,
			   unsigned int mask);
unsigned int comedi_dio_update_state(struct comedi_subdevice *s,
				     unsigned int *data);
unsigned int comedi_bytes_per_scan_cmd(struct comedi_subdevice *s,
				       struct comedi_cmd *cmd);
unsigned int comedi_bytes_per_scan(struct comedi_subdevice *s);
unsigned int comedi_nscans_left(struct comedi_subdevice *s,
				unsigned int nscans);
unsigned int comedi_nsamples_left(struct comedi_subdevice *s,
				  unsigned int nsamples);
void comedi_inc_scan_progress(struct comedi_subdevice *s,
			      unsigned int num_bytes);

void *comedi_alloc_devpriv(struct comedi_device *dev, size_t size);
int comedi_alloc_subdevices(struct comedi_device *dev, int num_subdevices);
int comedi_alloc_subdev_readback(struct comedi_subdevice *s);

int comedi_readback_insn_read(struct comedi_device *dev,
			      struct comedi_subdevice *s,
			      struct comedi_insn *insn, unsigned int *data);

int comedi_load_firmware(struct comedi_device *dev, struct device *hw_dev,
			 const char *name,
			 int (*cb)(struct comedi_device *dev,
				   const u8 *data, size_t size,
				   unsigned long context),
			 unsigned long context);

int __comedi_request_region(struct comedi_device *dev,
			    unsigned long start, unsigned long len);
int comedi_request_region(struct comedi_device *dev,
			  unsigned long start, unsigned long len);
void comedi_legacy_detach(struct comedi_device *dev);

int comedi_auto_config(struct device *hardware_device,
		       struct comedi_driver *driver, unsigned long context);
void comedi_auto_unconfig(struct device *hardware_device);

int comedi_driver_register(struct comedi_driver *driver);
void comedi_driver_unregister(struct comedi_driver *driver);


#define module_comedi_driver(__comedi_driver) \
	module_driver(__comedi_driver, comedi_driver_register, \
			comedi_driver_unregister)

#endif 
