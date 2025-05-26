/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_IIO_MACHINE_H__
#define __LINUX_IIO_MACHINE_H__


struct iio_map {
	const char *adc_channel_label;
	const char *consumer_dev_name;
	const char *consumer_channel;
	void *consumer_data;
};

#define IIO_MAP(_provider_channel, _consumer_dev_name, _consumer_channel) \
{									  \
	.adc_channel_label = _provider_channel,				  \
	.consumer_dev_name = _consumer_dev_name,			  \
	.consumer_channel  = _consumer_channel,				  \
}

#endif
