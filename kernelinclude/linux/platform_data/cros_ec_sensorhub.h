/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_PLATFORM_DATA_CROS_EC_SENSORHUB_H
#define __LINUX_PLATFORM_DATA_CROS_EC_SENSORHUB_H

#include <linux/ktime.h>
#include <linux/mutex.h>
#include <linux/notifier.h>
#include <linux/platform_data/cros_ec_commands.h>

struct iio_dev;


struct cros_ec_sensor_platform {
	u8 sensor_num;
};


typedef int (*cros_ec_sensorhub_push_data_cb_t)(struct iio_dev *indio_dev,
						s16 *data,
						s64 timestamp);

struct cros_ec_sensorhub_sensor_push_data {
	struct iio_dev *indio_dev;
	cros_ec_sensorhub_push_data_cb_t push_data_cb;
};

enum {
	CROS_EC_SENSOR_LAST_TS,
	CROS_EC_SENSOR_NEW_TS,
	CROS_EC_SENSOR_ALL_TS
};

struct cros_ec_sensors_ring_sample {
	u8  sensor_id;
	u8  flag;
	s16 vector[3];
	s64 timestamp;
} __packed;


struct cros_ec_sensors_ec_overflow_state {
	s64 offset;
	s64 last;
};


#define CROS_EC_SENSORHUB_TS_HISTORY_SIZE 64


struct cros_ec_sensors_ts_filter_state {
	s64 x_offset, y_offset;
	s64 x_history[CROS_EC_SENSORHUB_TS_HISTORY_SIZE];
	s64 y_history[CROS_EC_SENSORHUB_TS_HISTORY_SIZE];
	s64 m_history[CROS_EC_SENSORHUB_TS_HISTORY_SIZE];
	int history_len;

	s64 temp_buf[CROS_EC_SENSORHUB_TS_HISTORY_SIZE];

	s64 median_m;
	s64 median_error;
};


struct cros_ec_sensors_ts_batch_state {
	s64 penul_ts;
	int penul_len;
	s64 last_ts;
	int last_len;
	s64 newest_sensor_event;
};


struct cros_ec_sensorhub {
	struct device *dev;
	struct cros_ec_dev *ec;
	int sensor_num;

	struct cros_ec_command *msg;
	struct ec_params_motion_sense *params;
	struct ec_response_motion_sense *resp;
	struct mutex cmd_lock;  

	struct notifier_block notifier;

	struct cros_ec_sensors_ring_sample *ring;

	ktime_t fifo_timestamp[CROS_EC_SENSOR_ALL_TS];
	struct ec_response_motion_sense_fifo_info *fifo_info;
	int fifo_size;

	struct cros_ec_sensors_ts_batch_state *batch_state;

	struct cros_ec_sensors_ec_overflow_state overflow_a;
	struct cros_ec_sensors_ec_overflow_state overflow_b;

	struct cros_ec_sensors_ts_filter_state filter;

	int tight_timestamps;

	s32 future_timestamp_count;
	s64 future_timestamp_total_ns;

	struct cros_ec_sensorhub_sensor_push_data *push_data;
};

int cros_ec_sensorhub_register_push_data(struct cros_ec_sensorhub *sensorhub,
					 u8 sensor_num,
					 struct iio_dev *indio_dev,
					 cros_ec_sensorhub_push_data_cb_t cb);

void cros_ec_sensorhub_unregister_push_data(struct cros_ec_sensorhub *sensorhub,
					    u8 sensor_num);

int cros_ec_sensorhub_ring_allocate(struct cros_ec_sensorhub *sensorhub);
int cros_ec_sensorhub_ring_add(struct cros_ec_sensorhub *sensorhub);
void cros_ec_sensorhub_ring_remove(void *arg);
int cros_ec_sensorhub_ring_fifo_enable(struct cros_ec_sensorhub *sensorhub,
				       bool on);

#endif   
