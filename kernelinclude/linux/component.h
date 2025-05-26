/* SPDX-License-Identifier: GPL-2.0 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <linux/stddef.h>


struct device;


struct component_ops {
	
	int (*bind)(struct device *comp, struct device *master,
		    void *master_data);
	
	void (*unbind)(struct device *comp, struct device *master,
		       void *master_data);
};

int component_add(struct device *, const struct component_ops *);
int component_add_typed(struct device *dev, const struct component_ops *ops,
	int subcomponent);
void component_del(struct device *, const struct component_ops *);

int component_bind_all(struct device *parent, void *data);
void component_unbind_all(struct device *parent, void *data);

struct aggregate_device;


struct component_master_ops {
	
	int (*bind)(struct device *master);
	
	void (*unbind)(struct device *master);
};


int component_compare_of(struct device *dev, void *data);
void component_release_of(struct device *dev, void *data);
int component_compare_dev(struct device *dev, void *data);
int component_compare_dev_name(struct device *dev, void *data);

void component_master_del(struct device *,
	const struct component_master_ops *);

struct component_match;

int component_master_add_with_match(struct device *,
	const struct component_master_ops *, struct component_match *);
void component_match_add_release(struct device *parent,
	struct component_match **matchptr,
	void (*release)(struct device *, void *),
	int (*compare)(struct device *, void *), void *compare_data);
void component_match_add_typed(struct device *parent,
	struct component_match **matchptr,
	int (*compare_typed)(struct device *, int, void *), void *compare_data);


static inline void component_match_add(struct device *parent,
	struct component_match **matchptr,
	int (*compare)(struct device *, void *), void *compare_data)
{
	component_match_add_release(parent, matchptr, NULL, compare,
				    compare_data);
}

#endif
