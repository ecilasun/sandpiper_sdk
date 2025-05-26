/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_HTE_H
#define __LINUX_HTE_H

#include <linux/errno.h>

struct hte_chip;
struct hte_device;
struct of_phandle_args;


enum hte_edge {
	HTE_EDGE_NO_SETUP = 1U << 0,
	HTE_RISING_EDGE_TS = 1U << 1,
	HTE_FALLING_EDGE_TS = 1U << 2,
};


enum hte_return {
	HTE_CB_HANDLED,
	HTE_RUN_SECOND_CB,
};


struct hte_ts_data {
	u64 tsc;
	u64 seq;
	int raw_level;
};


struct hte_clk_info {
	u64 hz;
	clockid_t type;
};


typedef enum hte_return (*hte_ts_cb_t)(struct hte_ts_data *ts, void *data);


typedef enum hte_return (*hte_ts_sec_cb_t)(void *data);


struct hte_line_attr {
	u32 line_id;
	void *line_data;
	unsigned long edge_flags;
	const char *name;
};


struct hte_ts_desc {
	struct hte_line_attr attr;
	void *hte_data;
};


struct hte_ops {
	int (*request)(struct hte_chip *chip, struct hte_ts_desc *desc,
		       u32 xlated_id);
	int (*release)(struct hte_chip *chip, struct hte_ts_desc *desc,
		       u32 xlated_id);
	int (*enable)(struct hte_chip *chip, u32 xlated_id);
	int (*disable)(struct hte_chip *chip, u32 xlated_id);
	int (*get_clk_src_info)(struct hte_chip *chip,
				struct hte_clk_info *ci);
};


struct hte_chip {
	const char *name;
	struct device *dev;
	const struct hte_ops *ops;
	u32 nlines;
	int (*xlate_of)(struct hte_chip *gc,
			const struct of_phandle_args *args,
			struct hte_ts_desc *desc, u32 *xlated_id);
	int (*xlate_plat)(struct hte_chip *gc, struct hte_ts_desc *desc,
			 u32 *xlated_id);
	bool (*match_from_linedata)(const struct hte_chip *chip,
				    const struct hte_ts_desc *hdesc);
	u8 of_hte_n_cells;

	struct hte_device *gdev;
	void *data;
};

#if IS_ENABLED(CONFIG_HTE)

int devm_hte_register_chip(struct hte_chip *chip);
int hte_push_ts_ns(const struct hte_chip *chip, u32 xlated_id,
		   struct hte_ts_data *data);


int hte_init_line_attr(struct hte_ts_desc *desc, u32 line_id,
		       unsigned long edge_flags, const char *name,
		       void *data);
int hte_ts_get(struct device *dev, struct hte_ts_desc *desc, int index);
int hte_ts_put(struct hte_ts_desc *desc);
int hte_request_ts_ns(struct hte_ts_desc *desc, hte_ts_cb_t cb,
		      hte_ts_sec_cb_t tcb, void *data);
int devm_hte_request_ts_ns(struct device *dev, struct hte_ts_desc *desc,
			   hte_ts_cb_t cb, hte_ts_sec_cb_t tcb, void *data);
int of_hte_req_count(struct device *dev);
int hte_enable_ts(struct hte_ts_desc *desc);
int hte_disable_ts(struct hte_ts_desc *desc);
int hte_get_clk_src_info(const struct hte_ts_desc *desc,
			 struct hte_clk_info *ci);

#else 
static inline int devm_hte_register_chip(struct hte_chip *chip)
{
	return -EOPNOTSUPP;
}

static inline int hte_push_ts_ns(const struct hte_chip *chip,
				 u32 xlated_id,
				 const struct hte_ts_data *data)
{
	return -EOPNOTSUPP;
}

static inline int hte_init_line_attr(struct hte_ts_desc *desc, u32 line_id,
				     unsigned long edge_flags,
				     const char *name, void *data)
{
	return -EOPNOTSUPP;
}

static inline int hte_ts_get(struct device *dev, struct hte_ts_desc *desc,
			     int index)
{
	return -EOPNOTSUPP;
}

static inline int hte_ts_put(struct hte_ts_desc *desc)
{
	return -EOPNOTSUPP;
}

static inline int hte_request_ts_ns(struct hte_ts_desc *desc, hte_ts_cb_t cb,
				    hte_ts_sec_cb_t tcb, void *data)
{
	return -EOPNOTSUPP;
}

static inline int devm_hte_request_ts_ns(struct device *dev,
					 struct hte_ts_desc *desc,
					 hte_ts_cb_t cb,
					 hte_ts_sec_cb_t tcb,
					 void *data)
{
	return -EOPNOTSUPP;
}

static inline int of_hte_req_count(struct device *dev)
{
	return -EOPNOTSUPP;
}

static inline int hte_enable_ts(struct hte_ts_desc *desc)
{
	return -EOPNOTSUPP;
}

static inline int hte_disable_ts(struct hte_ts_desc *desc)
{
	return -EOPNOTSUPP;
}

static inline int hte_get_clk_src_info(const struct hte_ts_desc *desc,
				       struct hte_clk_info *ci)
{
	return -EOPNOTSUPP;
}
#endif 

#endif
