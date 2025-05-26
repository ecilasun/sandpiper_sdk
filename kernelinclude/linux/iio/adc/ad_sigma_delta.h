/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AD_SIGMA_DELTA_H__
#define __AD_SIGMA_DELTA_H__

#include <linux/iio/iio.h>

enum ad_sigma_delta_mode {
	AD_SD_MODE_CONTINUOUS = 0,
	AD_SD_MODE_SINGLE = 1,
	AD_SD_MODE_IDLE = 2,
	AD_SD_MODE_POWERDOWN = 3,
};


struct ad_sd_calib_data {
	unsigned int mode;
	unsigned int channel;
};

struct ad_sigma_delta;
struct device;
struct iio_dev;


struct ad_sigma_delta_info {
	int (*set_channel)(struct ad_sigma_delta *, unsigned int channel);
	int (*append_status)(struct ad_sigma_delta *, bool append);
	int (*set_mode)(struct ad_sigma_delta *, enum ad_sigma_delta_mode mode);
	int (*disable_all)(struct ad_sigma_delta *);
	int (*postprocess_sample)(struct ad_sigma_delta *, unsigned int raw_sample);
	bool has_registers;
	unsigned int addr_shift;
	unsigned int read_mask;
	unsigned int status_ch_mask;
	unsigned int data_reg;
	unsigned long irq_flags;
	unsigned int num_slots;
};


struct ad_sigma_delta {
	struct spi_device	*spi;
	struct iio_trigger	*trig;


	struct completion	completion;
	bool			irq_dis;

	bool			bus_locked;
	bool			keep_cs_asserted;

	uint8_t			comm;

	const struct ad_sigma_delta_info *info;
	unsigned int		active_slots;
	unsigned int		current_slot;
	unsigned int		num_slots;
	bool			status_appended;
	
	unsigned int		*slots;
	uint8_t			*samples_buf;

	
	uint8_t				tx_buf[4] __aligned(IIO_DMA_MINALIGN);
	uint8_t				rx_buf[16] __aligned(8);
};

static inline int ad_sigma_delta_set_channel(struct ad_sigma_delta *sd,
	unsigned int channel)
{
	if (sd->info->set_channel)
		return sd->info->set_channel(sd, channel);

	return 0;
}

static inline int ad_sigma_delta_append_status(struct ad_sigma_delta *sd, bool append)
{
	int ret;

	if (sd->info->append_status) {
		ret = sd->info->append_status(sd, append);
		if (ret < 0)
			return ret;

		sd->status_appended = append;
	}

	return 0;
}

static inline int ad_sigma_delta_disable_all(struct ad_sigma_delta *sd)
{
	if (sd->info->disable_all)
		return sd->info->disable_all(sd);

	return 0;
}

static inline int ad_sigma_delta_set_mode(struct ad_sigma_delta *sd,
	unsigned int mode)
{
	if (sd->info->set_mode)
		return sd->info->set_mode(sd, mode);

	return 0;
}

static inline int ad_sigma_delta_postprocess_sample(struct ad_sigma_delta *sd,
	unsigned int raw_sample)
{
	if (sd->info->postprocess_sample)
		return sd->info->postprocess_sample(sd, raw_sample);

	return 0;
}

void ad_sd_set_comm(struct ad_sigma_delta *sigma_delta, uint8_t comm);
int ad_sd_write_reg(struct ad_sigma_delta *sigma_delta, unsigned int reg,
	unsigned int size, unsigned int val);
int ad_sd_read_reg(struct ad_sigma_delta *sigma_delta, unsigned int reg,
	unsigned int size, unsigned int *val);

int ad_sd_reset(struct ad_sigma_delta *sigma_delta,
	unsigned int reset_length);

int ad_sigma_delta_single_conversion(struct iio_dev *indio_dev,
	const struct iio_chan_spec *chan, int *val);
int ad_sd_calibrate(struct ad_sigma_delta *sigma_delta,
	unsigned int mode, unsigned int channel);
int ad_sd_calibrate_all(struct ad_sigma_delta *sigma_delta,
	const struct ad_sd_calib_data *cd, unsigned int n);
int ad_sd_init(struct ad_sigma_delta *sigma_delta, struct iio_dev *indio_dev,
	struct spi_device *spi, const struct ad_sigma_delta_info *info);

int devm_ad_sd_setup_buffer_and_trigger(struct device *dev, struct iio_dev *indio_dev);

int ad_sd_validate_trigger(struct iio_dev *indio_dev, struct iio_trigger *trig);

#endif
