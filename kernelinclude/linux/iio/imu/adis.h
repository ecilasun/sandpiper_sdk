/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __IIO_ADIS_H__
#define __IIO_ADIS_H__

#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/iio/iio.h>
#include <linux/iio/types.h>

#define ADIS_WRITE_REG(reg) ((0x80 | (reg)))
#define ADIS_READ_REG(reg) ((reg) & 0x7f)

#define ADIS_PAGE_SIZE 0x80
#define ADIS_REG_PAGE_ID 0x00

struct adis;


struct adis_timeout {
	u16 reset_ms;
	u16 sw_reset_ms;
	u16 self_test_ms;
};


struct adis_data {
	unsigned int read_delay;
	unsigned int write_delay;
	unsigned int cs_change_delay;

	unsigned int glob_cmd_reg;
	unsigned int msc_ctrl_reg;
	unsigned int diag_stat_reg;
	unsigned int prod_id_reg;

	unsigned int prod_id;

	unsigned int self_test_mask;
	unsigned int self_test_reg;
	bool self_test_no_autoclear;
	const struct adis_timeout *timeouts;

	const char * const *status_error_msgs;
	unsigned int status_error_mask;

	int (*enable_irq)(struct adis *adis, bool enable);
	bool unmasked_drdy;

	bool has_paging;

	unsigned int burst_reg_cmd;
	unsigned int burst_len;
	unsigned int burst_max_len;
	unsigned int burst_max_speed_hz;
};


struct adis {
	struct spi_device	*spi;
	struct iio_trigger	*trig;

	const struct adis_data	*data;
	unsigned int		burst_extra_len;
	
	struct mutex		state_lock;
	struct spi_message	msg;
	struct spi_transfer	*xfer;
	unsigned int		current_page;
	unsigned long		irq_flag;
	void			*buffer;

	u8			tx[10] __aligned(IIO_DMA_MINALIGN);
	u8			rx[4];
};

int adis_init(struct adis *adis, struct iio_dev *indio_dev,
	      struct spi_device *spi, const struct adis_data *data);
int __adis_reset(struct adis *adis);


static inline int adis_reset(struct adis *adis)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_reset(adis);
	mutex_unlock(&adis->state_lock);

	return ret;
}

int __adis_write_reg(struct adis *adis, unsigned int reg,
		     unsigned int val, unsigned int size);
int __adis_read_reg(struct adis *adis, unsigned int reg,
		    unsigned int *val, unsigned int size);


static inline int __adis_write_reg_8(struct adis *adis, unsigned int reg,
				     u8 val)
{
	return __adis_write_reg(adis, reg, val, 1);
}


static inline int __adis_write_reg_16(struct adis *adis, unsigned int reg,
				      u16 val)
{
	return __adis_write_reg(adis, reg, val, 2);
}


static inline int __adis_write_reg_32(struct adis *adis, unsigned int reg,
				      u32 val)
{
	return __adis_write_reg(adis, reg, val, 4);
}


static inline int __adis_read_reg_16(struct adis *adis, unsigned int reg,
				     u16 *val)
{
	unsigned int tmp;
	int ret;

	ret = __adis_read_reg(adis, reg, &tmp, 2);
	if (ret == 0)
		*val = tmp;

	return ret;
}


static inline int __adis_read_reg_32(struct adis *adis, unsigned int reg,
				     u32 *val)
{
	unsigned int tmp;
	int ret;

	ret = __adis_read_reg(adis, reg, &tmp, 4);
	if (ret == 0)
		*val = tmp;

	return ret;
}


static inline int adis_write_reg(struct adis *adis, unsigned int reg,
				 unsigned int val, unsigned int size)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_write_reg(adis, reg, val, size);
	mutex_unlock(&adis->state_lock);

	return ret;
}


static int adis_read_reg(struct adis *adis, unsigned int reg,
			 unsigned int *val, unsigned int size)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_read_reg(adis, reg, val, size);
	mutex_unlock(&adis->state_lock);

	return ret;
}


static inline int adis_write_reg_8(struct adis *adis, unsigned int reg,
				   u8 val)
{
	return adis_write_reg(adis, reg, val, 1);
}


static inline int adis_write_reg_16(struct adis *adis, unsigned int reg,
				    u16 val)
{
	return adis_write_reg(adis, reg, val, 2);
}


static inline int adis_write_reg_32(struct adis *adis, unsigned int reg,
				    u32 val)
{
	return adis_write_reg(adis, reg, val, 4);
}


static inline int adis_read_reg_16(struct adis *adis, unsigned int reg,
				   u16 *val)
{
	unsigned int tmp;
	int ret;

	ret = adis_read_reg(adis, reg, &tmp, 2);
	if (ret == 0)
		*val = tmp;

	return ret;
}


static inline int adis_read_reg_32(struct adis *adis, unsigned int reg,
				   u32 *val)
{
	unsigned int tmp;
	int ret;

	ret = adis_read_reg(adis, reg, &tmp, 4);
	if (ret == 0)
		*val = tmp;

	return ret;
}

int __adis_update_bits_base(struct adis *adis, unsigned int reg, const u32 mask,
			    const u32 val, u8 size);

static inline int adis_update_bits_base(struct adis *adis, unsigned int reg,
					const u32 mask, const u32 val, u8 size)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_update_bits_base(adis, reg, mask, val, size);
	mutex_unlock(&adis->state_lock);
	return ret;
}


#define adis_update_bits(adis, reg, mask, val) ({			\
	BUILD_BUG_ON(sizeof(val) != 2 && sizeof(val) != 4);		\
	adis_update_bits_base(adis, reg, mask, val, sizeof(val));	\
})


#define __adis_update_bits(adis, reg, mask, val) ({			\
	BUILD_BUG_ON(sizeof(val) != 2 && sizeof(val) != 4);		\
	__adis_update_bits_base(adis, reg, mask, val, sizeof(val));	\
})

int __adis_check_status(struct adis *adis);
int __adis_initial_startup(struct adis *adis);
int __adis_enable_irq(struct adis *adis, bool enable);

static inline int adis_enable_irq(struct adis *adis, bool enable)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_enable_irq(adis, enable);
	mutex_unlock(&adis->state_lock);

	return ret;
}

static inline int adis_check_status(struct adis *adis)
{
	int ret;

	mutex_lock(&adis->state_lock);
	ret = __adis_check_status(adis);
	mutex_unlock(&adis->state_lock);

	return ret;
}

static inline void adis_dev_lock(struct adis *adis)
{
	mutex_lock(&adis->state_lock);
}

static inline void adis_dev_unlock(struct adis *adis)
{
	mutex_unlock(&adis->state_lock);
}

int adis_single_conversion(struct iio_dev *indio_dev,
			   const struct iio_chan_spec *chan,
			   unsigned int error_mask, int *val);

#define ADIS_VOLTAGE_CHAN(addr, si, chan, name, info_all, bits) { \
	.type = IIO_VOLTAGE, \
	.indexed = 1, \
	.channel = (chan), \
	.extend_name = name, \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		BIT(IIO_CHAN_INFO_SCALE), \
	.info_mask_shared_by_all = info_all, \
	.address = (addr), \
	.scan_index = (si), \
	.scan_type = { \
		.sign = 'u', \
		.realbits = (bits), \
		.storagebits = 16, \
		.endianness = IIO_BE, \
	}, \
}

#define ADIS_SUPPLY_CHAN(addr, si, info_all, bits) \
	ADIS_VOLTAGE_CHAN(addr, si, 0, "supply", info_all, bits)

#define ADIS_AUX_ADC_CHAN(addr, si, info_all, bits) \
	ADIS_VOLTAGE_CHAN(addr, si, 1, NULL, info_all, bits)

#define ADIS_TEMP_CHAN(addr, si, info_all, bits) { \
	.type = IIO_TEMP, \
	.indexed = 1, \
	.channel = 0, \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		BIT(IIO_CHAN_INFO_SCALE) | \
		BIT(IIO_CHAN_INFO_OFFSET), \
	.info_mask_shared_by_all = info_all, \
	.address = (addr), \
	.scan_index = (si), \
	.scan_type = { \
		.sign = 'u', \
		.realbits = (bits), \
		.storagebits = 16, \
		.endianness = IIO_BE, \
	}, \
}

#define ADIS_MOD_CHAN(_type, mod, addr, si, info_sep, info_all, bits) { \
	.type = (_type), \
	.modified = 1, \
	.channel2 = IIO_MOD_ ## mod, \
	.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) | \
		 (info_sep), \
	.info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE), \
	.info_mask_shared_by_all = info_all, \
	.address = (addr), \
	.scan_index = (si), \
	.scan_type = { \
		.sign = 's', \
		.realbits = (bits), \
		.storagebits = 16, \
		.endianness = IIO_BE, \
	}, \
}

#define ADIS_ACCEL_CHAN(mod, addr, si, info_sep, info_all, bits) \
	ADIS_MOD_CHAN(IIO_ACCEL, mod, addr, si, info_sep, info_all, bits)

#define ADIS_GYRO_CHAN(mod, addr, si, info_sep, info_all, bits)		\
	ADIS_MOD_CHAN(IIO_ANGL_VEL, mod, addr, si, info_sep, info_all, bits)

#define ADIS_INCLI_CHAN(mod, addr, si, info_sep, info_all, bits) \
	ADIS_MOD_CHAN(IIO_INCLI, mod, addr, si, info_sep, info_all, bits)

#define ADIS_ROT_CHAN(mod, addr, si, info_sep, info_all, bits) \
	ADIS_MOD_CHAN(IIO_ROT, mod, addr, si, info_sep, info_all, bits)

#ifdef CONFIG_IIO_ADIS_LIB_BUFFER

int
devm_adis_setup_buffer_and_trigger(struct adis *adis, struct iio_dev *indio_dev,
				   irq_handler_t trigger_handler);

int devm_adis_probe_trigger(struct adis *adis, struct iio_dev *indio_dev);

int adis_update_scan_mode(struct iio_dev *indio_dev,
			  const unsigned long *scan_mask);

#else 

static inline int
devm_adis_setup_buffer_and_trigger(struct adis *adis, struct iio_dev *indio_dev,
				   irq_handler_t trigger_handler)
{
	return 0;
}

static inline int devm_adis_probe_trigger(struct adis *adis,
					  struct iio_dev *indio_dev)
{
	return 0;
}

#define adis_update_scan_mode NULL

#endif 

#ifdef CONFIG_DEBUG_FS

int adis_debugfs_reg_access(struct iio_dev *indio_dev,
			    unsigned int reg, unsigned int writeval,
			    unsigned int *readval);

#else

#define adis_debugfs_reg_access NULL

#endif

#endif
