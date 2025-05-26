/* SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2005 David Brownell
 */

#ifndef __LINUX_SPI_H
#define __LINUX_SPI_H

#include <linux/acpi.h>
#include <linux/bits.h>
#include <linux/completion.h>
#include <linux/device.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/mod_devicetable.h>
#include <linux/overflow.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/u64_stats_sync.h>

#include <uapi/linux/spi/spi.h>


#define SPI_CS_CNT_MAX 16

struct dma_chan;
struct software_node;
struct ptp_system_timestamp;
struct spi_controller;
struct spi_transfer;
struct spi_controller_mem_ops;
struct spi_controller_mem_caps;
struct spi_message;


extern struct bus_type spi_bus_type;


struct spi_statistics {
	struct u64_stats_sync	syncp;

	u64_stats_t		messages;
	u64_stats_t		transfers;
	u64_stats_t		errors;
	u64_stats_t		timedout;

	u64_stats_t		spi_sync;
	u64_stats_t		spi_sync_immediate;
	u64_stats_t		spi_async;

	u64_stats_t		bytes;
	u64_stats_t		bytes_rx;
	u64_stats_t		bytes_tx;

#define SPI_STATISTICS_HISTO_SIZE 17
	u64_stats_t	transfer_bytes_histo[SPI_STATISTICS_HISTO_SIZE];

	u64_stats_t	transfers_split_maxsize;
};

#define SPI_STATISTICS_ADD_TO_FIELD(pcpu_stats, field, count)		\
	do {								\
		struct spi_statistics *__lstats;			\
		get_cpu();						\
		__lstats = this_cpu_ptr(pcpu_stats);			\
		u64_stats_update_begin(&__lstats->syncp);		\
		u64_stats_add(&__lstats->field, count);			\
		u64_stats_update_end(&__lstats->syncp);			\
		put_cpu();						\
	} while (0)

#define SPI_STATISTICS_INCREMENT_FIELD(pcpu_stats, field)		\
	do {								\
		struct spi_statistics *__lstats;			\
		get_cpu();						\
		__lstats = this_cpu_ptr(pcpu_stats);			\
		u64_stats_update_begin(&__lstats->syncp);		\
		u64_stats_inc(&__lstats->field);			\
		u64_stats_update_end(&__lstats->syncp);			\
		put_cpu();						\
	} while (0)


struct spi_delay {
#define SPI_DELAY_UNIT_USECS	0
#define SPI_DELAY_UNIT_NSECS	1
#define SPI_DELAY_UNIT_SCK	2
	u16	value;
	u8	unit;
};

extern int spi_delay_to_ns(struct spi_delay *_delay, struct spi_transfer *xfer);
extern int spi_delay_exec(struct spi_delay *_delay, struct spi_transfer *xfer);
extern void spi_transfer_cs_change_delay_exec(struct spi_message *msg,
						  struct spi_transfer *xfer);


struct spi_device {
	struct device		dev;
	struct spi_controller	*controller;
	struct spi_controller	*master;	
	u32			max_speed_hz;
	u8			chip_select[SPI_CS_CNT_MAX];
	u8			bits_per_word;
	bool			rt;
#define SPI_NO_TX		BIT(31)		
#define SPI_NO_RX		BIT(30)		
	
#define SPI_TPM_HW_FLOW		BIT(29)		
	
#define SPI_MODE_KERNEL_MASK	(~(BIT(29) - 1))
	u32			mode;
	int			irq;
	void			*controller_state;
	void			*controller_data;
	char			modalias[SPI_NAME_SIZE];
	const char		*driver_override;
	struct gpio_desc	*cs_gpiod[SPI_CS_CNT_MAX];	
	struct spi_delay	word_delay; 
	bool			multi_die;	
	
	struct spi_delay	cs_setup;
	struct spi_delay	cs_hold;
	struct spi_delay	cs_inactive;

	
	struct spi_statistics __percpu	*pcpu_statistics;

	
	u32			cs_index_mask : SPI_CS_CNT_MAX;

	
};


static_assert((SPI_MODE_KERNEL_MASK & SPI_MODE_USER_MASK) == 0,
	      "SPI_MODE_USER_MASK & SPI_MODE_KERNEL_MASK must not overlap");

static inline struct spi_device *to_spi_device(const struct device *dev)
{
	return dev ? container_of(dev, struct spi_device, dev) : NULL;
}


static inline struct spi_device *spi_dev_get(struct spi_device *spi)
{
	return (spi && get_device(&spi->dev)) ? spi : NULL;
}

static inline void spi_dev_put(struct spi_device *spi)
{
	if (spi)
		put_device(&spi->dev);
}


static inline void *spi_get_ctldata(const struct spi_device *spi)
{
	return spi->controller_state;
}

static inline void spi_set_ctldata(struct spi_device *spi, void *state)
{
	spi->controller_state = state;
}



static inline void spi_set_drvdata(struct spi_device *spi, void *data)
{
	dev_set_drvdata(&spi->dev, data);
}

static inline void *spi_get_drvdata(const struct spi_device *spi)
{
	return dev_get_drvdata(&spi->dev);
}

static inline u8 spi_get_chipselect(const struct spi_device *spi, u8 idx)
{
	return spi->chip_select[idx];
}

static inline void spi_set_chipselect(struct spi_device *spi, u8 idx, u8 chipselect)
{
	spi->chip_select[idx] = chipselect;
}

static inline struct gpio_desc *spi_get_csgpiod(const struct spi_device *spi, u8 idx)
{
	return spi->cs_gpiod[idx];
}

static inline void spi_set_csgpiod(struct spi_device *spi, u8 idx, struct gpio_desc *csgpiod)
{
	spi->cs_gpiod[idx] = csgpiod;
}

static inline bool spi_is_csgpiod(struct spi_device *spi)
{
	u8 idx;

	for (idx = 0; idx < SPI_CS_CNT_MAX; idx++) {
		if (spi_get_csgpiod(spi, idx))
			return true;
	}
	return false;
}


struct spi_driver {
	const struct spi_device_id *id_table;
	int			(*probe)(struct spi_device *spi);
	void			(*remove)(struct spi_device *spi);
	void			(*shutdown)(struct spi_device *spi);
	struct device_driver	driver;
};

static inline struct spi_driver *to_spi_driver(struct device_driver *drv)
{
	return drv ? container_of(drv, struct spi_driver, driver) : NULL;
}

extern int __spi_register_driver(struct module *owner, struct spi_driver *sdrv);


static inline void spi_unregister_driver(struct spi_driver *sdrv)
{
	if (sdrv)
		driver_unregister(&sdrv->driver);
}

extern struct spi_device *spi_new_ancillary_device(struct spi_device *spi, u8 chip_select);


#define spi_register_driver(driver) \
	__spi_register_driver(THIS_MODULE, driver)


#define module_spi_driver(__spi_driver) \
	module_driver(__spi_driver, spi_register_driver, \
			spi_unregister_driver)


struct spi_controller {
	struct device	dev;

	struct list_head list;

	
	s16			bus_num;

	
	u16			num_chipselect;

	
	u16			dma_alignment;

	
	u32			mode_bits;

	
	u32			buswidth_override_bits;

	
	u32			bits_per_word_mask;
#define SPI_BPW_MASK(bits) BIT((bits) - 1)
#define SPI_BPW_RANGE_MASK(min, max) GENMASK((max) - 1, (min) - 1)

	
	u32			min_speed_hz;
	u32			max_speed_hz;

	
	u16			flags;
#define SPI_CONTROLLER_HALF_DUPLEX	BIT(0)	
#define SPI_CONTROLLER_NO_RX		BIT(1)	
#define SPI_CONTROLLER_NO_TX		BIT(2)	
#define SPI_CONTROLLER_MUST_RX		BIT(3)	
#define SPI_CONTROLLER_MUST_TX		BIT(4)	
#define SPI_CONTROLLER_GPIO_SS		BIT(5)	
#define SPI_CONTROLLER_SUSPENDED	BIT(6)	
	
#define SPI_CONTROLLER_MULTI_CS		BIT(7)
#define	SPI_CONTROLLER_NO_4B		BIT(8)	

	
	bool			devm_allocated;

	union {
		
		bool			slave;
		
		bool			target;
	};

	
	size_t (*max_transfer_size)(struct spi_device *spi);
	size_t (*max_message_size)(struct spi_device *spi);

	
	struct mutex		io_mutex;

	
	struct mutex		add_lock;

	
	spinlock_t		bus_lock_spinlock;
	struct mutex		bus_lock_mutex;

	
	bool			bus_lock_flag;

	
	int			(*setup)(struct spi_device *spi);

	
	int (*set_cs_timing)(struct spi_device *spi);

	
	int			(*transfer)(struct spi_device *spi,
						struct spi_message *mesg);

	
	void			(*cleanup)(struct spi_device *spi);

	
	bool			(*can_dma)(struct spi_controller *ctlr,
					   struct spi_device *spi,
					   struct spi_transfer *xfer);
	struct device *dma_map_dev;
	struct device *cur_rx_dma_dev;
	struct device *cur_tx_dma_dev;

	
	bool				queued;
	struct kthread_worker		*kworker;
	struct kthread_work		pump_messages;
	spinlock_t			queue_lock;
	struct list_head		queue;
	struct spi_message		*cur_msg;
	struct completion               cur_msg_completion;
	bool				cur_msg_incomplete;
	bool				cur_msg_need_completion;
	bool				busy;
	bool				running;
	bool				rt;
	bool				auto_runtime_pm;
	bool				cur_msg_mapped;
	char				last_cs[SPI_CS_CNT_MAX];
	char				last_cs_index_mask;
	bool				last_cs_mode_high;
	bool                            fallback;
	struct completion               xfer_completion;
	size_t				max_dma_len;

	int (*prepare_transfer_hardware)(struct spi_controller *ctlr);
	int (*transfer_one_message)(struct spi_controller *ctlr,
				    struct spi_message *mesg);
	int (*unprepare_transfer_hardware)(struct spi_controller *ctlr);
	int (*prepare_message)(struct spi_controller *ctlr,
			       struct spi_message *message);
	int (*unprepare_message)(struct spi_controller *ctlr,
				 struct spi_message *message);
	union {
		int (*slave_abort)(struct spi_controller *ctlr);
		int (*target_abort)(struct spi_controller *ctlr);
	};

	
	void (*set_cs)(struct spi_device *spi, bool enable);
	int (*transfer_one)(struct spi_controller *ctlr, struct spi_device *spi,
			    struct spi_transfer *transfer);
	void (*handle_err)(struct spi_controller *ctlr,
			   struct spi_message *message);

	
	const struct spi_controller_mem_ops *mem_ops;
	const struct spi_controller_mem_caps *mem_caps;

	
	struct gpio_desc	**cs_gpiods;
	bool			use_gpio_descriptors;
	s8			unused_native_cs;
	s8			max_native_cs;

	
	struct spi_statistics __percpu	*pcpu_statistics;

	
	struct dma_chan		*dma_tx;
	struct dma_chan		*dma_rx;

	
	void			*dummy_rx;
	void			*dummy_tx;

	int (*fw_translate_cs)(struct spi_controller *ctlr, unsigned cs);

	
	bool			ptp_sts_supported;

	
	unsigned long		irq_flags;

	
	bool			queue_empty;
	bool			must_async;
};

static inline void *spi_controller_get_devdata(struct spi_controller *ctlr)
{
	return dev_get_drvdata(&ctlr->dev);
}

static inline void spi_controller_set_devdata(struct spi_controller *ctlr,
					      void *data)
{
	dev_set_drvdata(&ctlr->dev, data);
}

static inline struct spi_controller *spi_controller_get(struct spi_controller *ctlr)
{
	if (!ctlr || !get_device(&ctlr->dev))
		return NULL;
	return ctlr;
}

static inline void spi_controller_put(struct spi_controller *ctlr)
{
	if (ctlr)
		put_device(&ctlr->dev);
}

static inline bool spi_controller_is_slave(struct spi_controller *ctlr)
{
	return IS_ENABLED(CONFIG_SPI_SLAVE) && ctlr->slave;
}

static inline bool spi_controller_is_target(struct spi_controller *ctlr)
{
	return IS_ENABLED(CONFIG_SPI_SLAVE) && ctlr->target;
}


extern int spi_controller_suspend(struct spi_controller *ctlr);
extern int spi_controller_resume(struct spi_controller *ctlr);


extern struct spi_message *spi_get_next_queued_message(struct spi_controller *ctlr);
extern void spi_finalize_current_message(struct spi_controller *ctlr);
extern void spi_finalize_current_transfer(struct spi_controller *ctlr);


void spi_take_timestamp_pre(struct spi_controller *ctlr,
			    struct spi_transfer *xfer,
			    size_t progress, bool irqs_off);
void spi_take_timestamp_post(struct spi_controller *ctlr,
			     struct spi_transfer *xfer,
			     size_t progress, bool irqs_off);


extern struct spi_controller *__spi_alloc_controller(struct device *host,
						unsigned int size, bool slave);

static inline struct spi_controller *spi_alloc_master(struct device *host,
						      unsigned int size)
{
	return __spi_alloc_controller(host, size, false);
}

static inline struct spi_controller *spi_alloc_slave(struct device *host,
						     unsigned int size)
{
	if (!IS_ENABLED(CONFIG_SPI_SLAVE))
		return NULL;

	return __spi_alloc_controller(host, size, true);
}

static inline struct spi_controller *spi_alloc_host(struct device *dev,
						    unsigned int size)
{
	return __spi_alloc_controller(dev, size, false);
}

static inline struct spi_controller *spi_alloc_target(struct device *dev,
						      unsigned int size)
{
	if (!IS_ENABLED(CONFIG_SPI_SLAVE))
		return NULL;

	return __spi_alloc_controller(dev, size, true);
}

struct spi_controller *__devm_spi_alloc_controller(struct device *dev,
						   unsigned int size,
						   bool slave);

static inline struct spi_controller *devm_spi_alloc_master(struct device *dev,
							   unsigned int size)
{
	return __devm_spi_alloc_controller(dev, size, false);
}

static inline struct spi_controller *devm_spi_alloc_slave(struct device *dev,
							  unsigned int size)
{
	if (!IS_ENABLED(CONFIG_SPI_SLAVE))
		return NULL;

	return __devm_spi_alloc_controller(dev, size, true);
}

static inline struct spi_controller *devm_spi_alloc_host(struct device *dev,
							 unsigned int size)
{
	return __devm_spi_alloc_controller(dev, size, false);
}

static inline struct spi_controller *devm_spi_alloc_target(struct device *dev,
							   unsigned int size)
{
	if (!IS_ENABLED(CONFIG_SPI_SLAVE))
		return NULL;

	return __devm_spi_alloc_controller(dev, size, true);
}

extern int spi_register_controller(struct spi_controller *ctlr);
extern int devm_spi_register_controller(struct device *dev,
					struct spi_controller *ctlr);
extern void spi_unregister_controller(struct spi_controller *ctlr);

#if IS_ENABLED(CONFIG_ACPI)
extern struct spi_device *acpi_spi_device_alloc(struct spi_controller *ctlr,
						struct acpi_device *adev,
						int index);
int acpi_spi_count_resources(struct acpi_device *adev);
#endif



typedef void (*spi_res_release_t)(struct spi_controller *ctlr,
				  struct spi_message *msg,
				  void *res);


struct spi_res {
	struct list_head        entry;
	spi_res_release_t       release;
	unsigned long long      data[]; 
};






struct spi_transfer {
	
	const void	*tx_buf;
	void		*rx_buf;
	unsigned	len;

#define SPI_TRANS_FAIL_NO_START	BIT(0)
	u16		error;

	dma_addr_t	tx_dma;
	dma_addr_t	rx_dma;
	struct sg_table tx_sg;
	struct sg_table rx_sg;

	unsigned	dummy_data:1;
	unsigned	cs_off:1;
	unsigned	cs_change:1;
	unsigned	tx_nbits:3;
	unsigned	rx_nbits:3;
	unsigned	timestamped:1;
#define	SPI_NBITS_SINGLE	0x01 
#define	SPI_NBITS_DUAL		0x02 
#define	SPI_NBITS_QUAD		0x04 
	u8		bits_per_word;
	struct spi_delay	delay;
	struct spi_delay	cs_change_delay;
	struct spi_delay	word_delay;
	u32		speed_hz;
	u32		dummy;

	u32		effective_speed_hz;

	unsigned int	ptp_sts_word_pre;
	unsigned int	ptp_sts_word_post;

	struct ptp_system_timestamp *ptp_sts;

	struct list_head transfer_list;
};


struct spi_message {
	struct list_head	transfers;

	struct spi_device	*spi;

	unsigned		is_dma_mapped:1;

	
	bool			prepared;

	

	
	int			status;
	void			(*complete)(void *context);
	void			*context;
	unsigned		frame_length;
	unsigned		actual_length;

	
	struct list_head	queue;
	void			*state;

	
	struct list_head        resources;

	
	struct spi_transfer	t[];
};

static inline void spi_message_init_no_memset(struct spi_message *m)
{
	INIT_LIST_HEAD(&m->transfers);
	INIT_LIST_HEAD(&m->resources);
}

static inline void spi_message_init(struct spi_message *m)
{
	memset(m, 0, sizeof *m);
	spi_message_init_no_memset(m);
}

static inline void
spi_message_add_tail(struct spi_transfer *t, struct spi_message *m)
{
	list_add_tail(&t->transfer_list, &m->transfers);
}

static inline void
spi_transfer_del(struct spi_transfer *t)
{
	list_del(&t->transfer_list);
}

static inline int
spi_transfer_delay_exec(struct spi_transfer *t)
{
	return spi_delay_exec(&t->delay, t);
}


static inline void
spi_message_init_with_transfers(struct spi_message *m,
struct spi_transfer *xfers, unsigned int num_xfers)
{
	unsigned int i;

	spi_message_init(m);
	for (i = 0; i < num_xfers; ++i)
		spi_message_add_tail(&xfers[i], m);
}


static inline struct spi_message *spi_message_alloc(unsigned ntrans, gfp_t flags)
{
	struct spi_message *m;

	m = kzalloc(struct_size(m, t, ntrans), flags);
	if (m) {
		unsigned i;

		spi_message_init_no_memset(m);
		for (i = 0; i < ntrans; i++)
			spi_message_add_tail(&m->t[i], m);
	}
	return m;
}

static inline void spi_message_free(struct spi_message *m)
{
	kfree(m);
}

extern int spi_setup(struct spi_device *spi);
extern int spi_async(struct spi_device *spi, struct spi_message *message);
extern int spi_slave_abort(struct spi_device *spi);
extern int spi_target_abort(struct spi_device *spi);

static inline size_t
spi_max_message_size(struct spi_device *spi)
{
	struct spi_controller *ctlr = spi->controller;

	if (!ctlr->max_message_size)
		return SIZE_MAX;
	return ctlr->max_message_size(spi);
}

static inline size_t
spi_max_transfer_size(struct spi_device *spi)
{
	struct spi_controller *ctlr = spi->controller;
	size_t tr_max = SIZE_MAX;
	size_t msg_max = spi_max_message_size(spi);

	if (ctlr->max_transfer_size)
		tr_max = ctlr->max_transfer_size(spi);

	
	return min(tr_max, msg_max);
}


static inline bool spi_is_bpw_supported(struct spi_device *spi, u32 bpw)
{
	u32 bpw_mask = spi->master->bits_per_word_mask;

	if (bpw == 8 || (bpw <= 32 && bpw_mask & SPI_BPW_MASK(bpw)))
		return true;

	return false;
}


static inline unsigned int spi_controller_xfer_timeout(struct spi_controller *ctlr,
						       struct spi_transfer *xfer)
{
	return max(xfer->len * 8 * 2 / (xfer->speed_hz / 1000), 500U);
}





struct spi_replaced_transfers;
typedef void (*spi_replaced_release_t)(struct spi_controller *ctlr,
				       struct spi_message *msg,
				       struct spi_replaced_transfers *res);

struct spi_replaced_transfers {
	spi_replaced_release_t release;
	void *extradata;
	struct list_head replaced_transfers;
	struct list_head *replaced_after;
	size_t inserted;
	struct spi_transfer inserted_transfers[];
};





extern int spi_split_transfers_maxsize(struct spi_controller *ctlr,
				       struct spi_message *msg,
				       size_t maxsize,
				       gfp_t gfp);
extern int spi_split_transfers_maxwords(struct spi_controller *ctlr,
					struct spi_message *msg,
					size_t maxwords,
					gfp_t gfp);





extern int spi_sync(struct spi_device *spi, struct spi_message *message);
extern int spi_sync_locked(struct spi_device *spi, struct spi_message *message);
extern int spi_bus_lock(struct spi_controller *ctlr);
extern int spi_bus_unlock(struct spi_controller *ctlr);


static inline int
spi_sync_transfer(struct spi_device *spi, struct spi_transfer *xfers,
	unsigned int num_xfers)
{
	struct spi_message msg;

	spi_message_init_with_transfers(&msg, xfers, num_xfers);

	return spi_sync(spi, &msg);
}


static inline int
spi_write(struct spi_device *spi, const void *buf, size_t len)
{
	struct spi_transfer	t = {
			.tx_buf		= buf,
			.len		= len,
		};

	return spi_sync_transfer(spi, &t, 1);
}


static inline int
spi_read(struct spi_device *spi, void *buf, size_t len)
{
	struct spi_transfer	t = {
			.rx_buf		= buf,
			.len		= len,
		};

	return spi_sync_transfer(spi, &t, 1);
}


extern int spi_write_then_read(struct spi_device *spi,
		const void *txbuf, unsigned n_tx,
		void *rxbuf, unsigned n_rx);


static inline ssize_t spi_w8r8(struct spi_device *spi, u8 cmd)
{
	ssize_t			status;
	u8			result;

	status = spi_write_then_read(spi, &cmd, 1, &result, 1);

	
	return (status < 0) ? status : result;
}


static inline ssize_t spi_w8r16(struct spi_device *spi, u8 cmd)
{
	ssize_t			status;
	u16			result;

	status = spi_write_then_read(spi, &cmd, 1, &result, 2);

	
	return (status < 0) ? status : result;
}


static inline ssize_t spi_w8r16be(struct spi_device *spi, u8 cmd)

{
	ssize_t status;
	__be16 result;

	status = spi_write_then_read(spi, &cmd, 1, &result, 2);
	if (status < 0)
		return status;

	return be16_to_cpu(result);
}






struct spi_board_info {
	
	char		modalias[SPI_NAME_SIZE];
	const void	*platform_data;
	const struct software_node *swnode;
	void		*controller_data;
	int		irq;

	
	u32		max_speed_hz;


	
	u16		bus_num;
	u16		chip_select;

	
	u32		mode;

	
};

#ifdef	CONFIG_SPI
extern int
spi_register_board_info(struct spi_board_info const *info, unsigned n);
#else

static inline int
spi_register_board_info(struct spi_board_info const *info, unsigned n)
	{ return 0; }
#endif


extern struct spi_device *
spi_alloc_device(struct spi_controller *ctlr);

extern int
spi_add_device(struct spi_device *spi);

extern struct spi_device *
spi_new_device(struct spi_controller *, struct spi_board_info *);

extern void spi_unregister_device(struct spi_device *spi);

extern const struct spi_device_id *
spi_get_device_id(const struct spi_device *sdev);

extern const void *
spi_get_device_match_data(const struct spi_device *sdev);

static inline bool
spi_transfer_is_last(struct spi_controller *ctlr, struct spi_transfer *xfer)
{
	return list_is_last(&xfer->transfer_list, &ctlr->cur_msg->transfers);
}


#define spi_master			spi_controller

#define SPI_MASTER_HALF_DUPLEX		SPI_CONTROLLER_HALF_DUPLEX

#define spi_master_get_devdata(_ctlr)	spi_controller_get_devdata(_ctlr)
#define spi_master_set_devdata(_ctlr, _data)	\
	spi_controller_set_devdata(_ctlr, _data)
#define spi_master_get(_ctlr)		spi_controller_get(_ctlr)
#define spi_master_put(_ctlr)		spi_controller_put(_ctlr)
#define spi_master_suspend(_ctlr)	spi_controller_suspend(_ctlr)
#define spi_master_resume(_ctlr)	spi_controller_resume(_ctlr)

#define spi_register_master(_ctlr)	spi_register_controller(_ctlr)
#define devm_spi_register_master(_dev, _ctlr) \
	devm_spi_register_controller(_dev, _ctlr)
#define spi_unregister_master(_ctlr)	spi_unregister_controller(_ctlr)

#endif 
