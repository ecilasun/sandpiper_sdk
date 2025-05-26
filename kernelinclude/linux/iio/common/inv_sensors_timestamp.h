/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef INV_SENSORS_TIMESTAMP_H_
#define INV_SENSORS_TIMESTAMP_H_


struct inv_sensors_timestamp_chip {
	uint32_t clock_period;
	uint32_t jitter;
	uint32_t init_period;
};


struct inv_sensors_timestamp_interval {
	int64_t lo;
	int64_t up;
};


struct inv_sensors_timestamp_acc {
	uint32_t val;
	size_t idx;
	uint32_t values[32];
};


struct inv_sensors_timestamp {
	struct inv_sensors_timestamp_chip chip;
	uint32_t min_period;
	uint32_t max_period;
	struct inv_sensors_timestamp_interval it;
	int64_t timestamp;
	uint32_t mult;
	uint32_t new_mult;
	uint32_t period;
	struct inv_sensors_timestamp_acc chip_period;
};

void inv_sensors_timestamp_init(struct inv_sensors_timestamp *ts,
				const struct inv_sensors_timestamp_chip *chip);

int inv_sensors_timestamp_update_odr(struct inv_sensors_timestamp *ts,
				     uint32_t period, bool fifo);

void inv_sensors_timestamp_interrupt(struct inv_sensors_timestamp *ts,
				     uint32_t fifo_period, size_t fifo_nb,
				     size_t sensor_nb, int64_t timestamp);

static inline int64_t inv_sensors_timestamp_pop(struct inv_sensors_timestamp *ts)
{
	ts->timestamp += ts->period;
	return ts->timestamp;
}

void inv_sensors_timestamp_apply_odr(struct inv_sensors_timestamp *ts,
				     uint32_t fifo_period, size_t fifo_nb,
				     unsigned int fifo_no);

static inline void inv_sensors_timestamp_reset(struct inv_sensors_timestamp *ts)
{
	const struct inv_sensors_timestamp_interval interval_init = {0LL, 0LL};

	ts->it = interval_init;
	ts->timestamp = 0;
}

#endif
