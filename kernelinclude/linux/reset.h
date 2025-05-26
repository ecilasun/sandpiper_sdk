/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RESET_H_
#define _LINUX_RESET_H_

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/types.h>

struct device;
struct device_node;
struct reset_control;


struct reset_control_bulk_data {
	const char			*id;
	struct reset_control		*rstc;
};

#ifdef CONFIG_RESET_CONTROLLER

int reset_control_reset(struct reset_control *rstc);
int reset_control_rearm(struct reset_control *rstc);
int reset_control_assert(struct reset_control *rstc);
int reset_control_deassert(struct reset_control *rstc);
int reset_control_status(struct reset_control *rstc);
int reset_control_acquire(struct reset_control *rstc);
void reset_control_release(struct reset_control *rstc);

int reset_control_bulk_reset(int num_rstcs, struct reset_control_bulk_data *rstcs);
int reset_control_bulk_assert(int num_rstcs, struct reset_control_bulk_data *rstcs);
int reset_control_bulk_deassert(int num_rstcs, struct reset_control_bulk_data *rstcs);
int reset_control_bulk_acquire(int num_rstcs, struct reset_control_bulk_data *rstcs);
void reset_control_bulk_release(int num_rstcs, struct reset_control_bulk_data *rstcs);

struct reset_control *__of_reset_control_get(struct device_node *node,
				     const char *id, int index, bool shared,
				     bool optional, bool acquired);
struct reset_control *__reset_control_get(struct device *dev, const char *id,
					  int index, bool shared,
					  bool optional, bool acquired);
void reset_control_put(struct reset_control *rstc);
int __reset_control_bulk_get(struct device *dev, int num_rstcs,
			     struct reset_control_bulk_data *rstcs,
			     bool shared, bool optional, bool acquired);
void reset_control_bulk_put(int num_rstcs, struct reset_control_bulk_data *rstcs);

int __device_reset(struct device *dev, bool optional);
struct reset_control *__devm_reset_control_get(struct device *dev,
				     const char *id, int index, bool shared,
				     bool optional, bool acquired);
int __devm_reset_control_bulk_get(struct device *dev, int num_rstcs,
				  struct reset_control_bulk_data *rstcs,
				  bool shared, bool optional, bool acquired);

struct reset_control *devm_reset_control_array_get(struct device *dev,
						   bool shared, bool optional);
struct reset_control *of_reset_control_array_get(struct device_node *np,
						 bool shared, bool optional,
						 bool acquired);

int reset_control_get_count(struct device *dev);

#else

static inline int reset_control_reset(struct reset_control *rstc)
{
	return 0;
}

static inline int reset_control_rearm(struct reset_control *rstc)
{
	return 0;
}

static inline int reset_control_assert(struct reset_control *rstc)
{
	return 0;
}

static inline int reset_control_deassert(struct reset_control *rstc)
{
	return 0;
}

static inline int reset_control_status(struct reset_control *rstc)
{
	return 0;
}

static inline int reset_control_acquire(struct reset_control *rstc)
{
	return 0;
}

static inline void reset_control_release(struct reset_control *rstc)
{
}

static inline void reset_control_put(struct reset_control *rstc)
{
}

static inline int __device_reset(struct device *dev, bool optional)
{
	return optional ? 0 : -ENOTSUPP;
}

static inline struct reset_control *__of_reset_control_get(
					struct device_node *node,
					const char *id, int index, bool shared,
					bool optional, bool acquired)
{
	return optional ? NULL : ERR_PTR(-ENOTSUPP);
}

static inline struct reset_control *__reset_control_get(
					struct device *dev, const char *id,
					int index, bool shared, bool optional,
					bool acquired)
{
	return optional ? NULL : ERR_PTR(-ENOTSUPP);
}

static inline int
reset_control_bulk_reset(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
	return 0;
}

static inline int
reset_control_bulk_assert(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
	return 0;
}

static inline int
reset_control_bulk_deassert(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
	return 0;
}

static inline int
reset_control_bulk_acquire(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
	return 0;
}

static inline void
reset_control_bulk_release(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
}

static inline int
__reset_control_bulk_get(struct device *dev, int num_rstcs,
			 struct reset_control_bulk_data *rstcs,
			 bool shared, bool optional, bool acquired)
{
	return optional ? 0 : -EOPNOTSUPP;
}

static inline void
reset_control_bulk_put(int num_rstcs, struct reset_control_bulk_data *rstcs)
{
}

static inline struct reset_control *__devm_reset_control_get(
					struct device *dev, const char *id,
					int index, bool shared, bool optional,
					bool acquired)
{
	return optional ? NULL : ERR_PTR(-ENOTSUPP);
}

static inline int
__devm_reset_control_bulk_get(struct device *dev, int num_rstcs,
			      struct reset_control_bulk_data *rstcs,
			      bool shared, bool optional, bool acquired)
{
	return optional ? 0 : -EOPNOTSUPP;
}

static inline struct reset_control *
devm_reset_control_array_get(struct device *dev, bool shared, bool optional)
{
	return optional ? NULL : ERR_PTR(-ENOTSUPP);
}

static inline struct reset_control *
of_reset_control_array_get(struct device_node *np, bool shared, bool optional,
			   bool acquired)
{
	return optional ? NULL : ERR_PTR(-ENOTSUPP);
}

static inline int reset_control_get_count(struct device *dev)
{
	return -ENOENT;
}

#endif 

static inline int __must_check device_reset(struct device *dev)
{
	return __device_reset(dev, false);
}

static inline int device_reset_optional(struct device *dev)
{
	return __device_reset(dev, true);
}


static inline struct reset_control *
__must_check reset_control_get_exclusive(struct device *dev, const char *id)
{
	return __reset_control_get(dev, id, 0, false, false, true);
}


static inline int __must_check
reset_control_bulk_get_exclusive(struct device *dev, int num_rstcs,
				 struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, false, false, true);
}


static inline struct reset_control *
__must_check reset_control_get_exclusive_released(struct device *dev,
						  const char *id)
{
	return __reset_control_get(dev, id, 0, false, false, false);
}


static inline int __must_check
reset_control_bulk_get_exclusive_released(struct device *dev, int num_rstcs,
					  struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, false, false, false);
}


static inline int __must_check
reset_control_bulk_get_optional_exclusive_released(struct device *dev, int num_rstcs,
						   struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, false, true, false);
}


static inline struct reset_control *reset_control_get_shared(
					struct device *dev, const char *id)
{
	return __reset_control_get(dev, id, 0, true, false, false);
}


static inline int __must_check
reset_control_bulk_get_shared(struct device *dev, int num_rstcs,
			      struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, true, false, false);
}


static inline struct reset_control *reset_control_get_optional_exclusive(
					struct device *dev, const char *id)
{
	return __reset_control_get(dev, id, 0, false, true, true);
}


static inline int __must_check
reset_control_bulk_get_optional_exclusive(struct device *dev, int num_rstcs,
					  struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, false, true, true);
}


static inline struct reset_control *reset_control_get_optional_shared(
					struct device *dev, const char *id)
{
	return __reset_control_get(dev, id, 0, true, true, false);
}


static inline int __must_check
reset_control_bulk_get_optional_shared(struct device *dev, int num_rstcs,
				       struct reset_control_bulk_data *rstcs)
{
	return __reset_control_bulk_get(dev, num_rstcs, rstcs, true, true, false);
}


static inline struct reset_control *of_reset_control_get_exclusive(
				struct device_node *node, const char *id)
{
	return __of_reset_control_get(node, id, 0, false, false, true);
}


static inline struct reset_control *of_reset_control_get_optional_exclusive(
				struct device_node *node, const char *id)
{
	return __of_reset_control_get(node, id, 0, false, true, true);
}


static inline struct reset_control *of_reset_control_get_shared(
				struct device_node *node, const char *id)
{
	return __of_reset_control_get(node, id, 0, true, false, false);
}


static inline struct reset_control *of_reset_control_get_exclusive_by_index(
					struct device_node *node, int index)
{
	return __of_reset_control_get(node, NULL, index, false, false, true);
}


static inline struct reset_control *of_reset_control_get_shared_by_index(
					struct device_node *node, int index)
{
	return __of_reset_control_get(node, NULL, index, true, false, false);
}


static inline struct reset_control *
__must_check devm_reset_control_get_exclusive(struct device *dev,
					      const char *id)
{
	return __devm_reset_control_get(dev, id, 0, false, false, true);
}


static inline int __must_check
devm_reset_control_bulk_get_exclusive(struct device *dev, int num_rstcs,
				      struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, false, false, true);
}


static inline struct reset_control *
__must_check devm_reset_control_get_exclusive_released(struct device *dev,
						       const char *id)
{
	return __devm_reset_control_get(dev, id, 0, false, false, false);
}


static inline int __must_check
devm_reset_control_bulk_get_exclusive_released(struct device *dev, int num_rstcs,
					       struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, false, false, false);
}


static inline struct reset_control *
__must_check devm_reset_control_get_optional_exclusive_released(struct device *dev,
								const char *id)
{
	return __devm_reset_control_get(dev, id, 0, false, true, false);
}


static inline int __must_check
devm_reset_control_bulk_get_optional_exclusive_released(struct device *dev, int num_rstcs,
							struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, false, true, false);
}


static inline struct reset_control *devm_reset_control_get_shared(
					struct device *dev, const char *id)
{
	return __devm_reset_control_get(dev, id, 0, true, false, false);
}


static inline int __must_check
devm_reset_control_bulk_get_shared(struct device *dev, int num_rstcs,
				   struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, true, false, false);
}


static inline struct reset_control *devm_reset_control_get_optional_exclusive(
					struct device *dev, const char *id)
{
	return __devm_reset_control_get(dev, id, 0, false, true, true);
}


static inline int __must_check
devm_reset_control_bulk_get_optional_exclusive(struct device *dev, int num_rstcs,
					       struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, false, true, true);
}


static inline struct reset_control *devm_reset_control_get_optional_shared(
					struct device *dev, const char *id)
{
	return __devm_reset_control_get(dev, id, 0, true, true, false);
}


static inline int __must_check
devm_reset_control_bulk_get_optional_shared(struct device *dev, int num_rstcs,
					    struct reset_control_bulk_data *rstcs)
{
	return __devm_reset_control_bulk_get(dev, num_rstcs, rstcs, true, true, false);
}


static inline struct reset_control *
devm_reset_control_get_exclusive_by_index(struct device *dev, int index)
{
	return __devm_reset_control_get(dev, NULL, index, false, false, true);
}


static inline struct reset_control *
devm_reset_control_get_shared_by_index(struct device *dev, int index)
{
	return __devm_reset_control_get(dev, NULL, index, true, false, false);
}


static inline struct reset_control *of_reset_control_get(
				struct device_node *node, const char *id)
{
	return of_reset_control_get_exclusive(node, id);
}

static inline struct reset_control *of_reset_control_get_by_index(
				struct device_node *node, int index)
{
	return of_reset_control_get_exclusive_by_index(node, index);
}

static inline struct reset_control *devm_reset_control_get(
				struct device *dev, const char *id)
{
	return devm_reset_control_get_exclusive(dev, id);
}

static inline struct reset_control *devm_reset_control_get_optional(
				struct device *dev, const char *id)
{
	return devm_reset_control_get_optional_exclusive(dev, id);

}

static inline struct reset_control *devm_reset_control_get_by_index(
				struct device *dev, int index)
{
	return devm_reset_control_get_exclusive_by_index(dev, index);
}


static inline struct reset_control *
devm_reset_control_array_get_exclusive(struct device *dev)
{
	return devm_reset_control_array_get(dev, false, false);
}

static inline struct reset_control *
devm_reset_control_array_get_shared(struct device *dev)
{
	return devm_reset_control_array_get(dev, true, false);
}

static inline struct reset_control *
devm_reset_control_array_get_optional_exclusive(struct device *dev)
{
	return devm_reset_control_array_get(dev, false, true);
}

static inline struct reset_control *
devm_reset_control_array_get_optional_shared(struct device *dev)
{
	return devm_reset_control_array_get(dev, true, true);
}

static inline struct reset_control *
of_reset_control_array_get_exclusive(struct device_node *node)
{
	return of_reset_control_array_get(node, false, false, true);
}

static inline struct reset_control *
of_reset_control_array_get_exclusive_released(struct device_node *node)
{
	return of_reset_control_array_get(node, false, false, false);
}

static inline struct reset_control *
of_reset_control_array_get_shared(struct device_node *node)
{
	return of_reset_control_array_get(node, true, false, true);
}

static inline struct reset_control *
of_reset_control_array_get_optional_exclusive(struct device_node *node)
{
	return of_reset_control_array_get(node, false, true, true);
}

static inline struct reset_control *
of_reset_control_array_get_optional_shared(struct device_node *node)
{
	return of_reset_control_array_get(node, true, true, true);
}
#endif
