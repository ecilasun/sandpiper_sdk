/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GPIO_DRIVER_H
#define __LINUX_GPIO_DRIVER_H

#include <linux/bits.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/irqhandler.h>
#include <linux/lockdep.h>
#include <linux/pinctrl/pinconf-generic.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/property.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>

#ifdef CONFIG_GENERIC_MSI_IRQ
#include <asm/msi.h>
#endif

struct device;
struct irq_chip;
struct irq_data;
struct module;
struct of_phandle_args;
struct pinctrl_dev;
struct seq_file;

struct gpio_chip;
struct gpio_desc;
struct gpio_device;

enum gpio_lookup_flags;
enum gpiod_flags;

union gpio_irq_fwspec {
	struct irq_fwspec	fwspec;
#ifdef CONFIG_GENERIC_MSI_IRQ
	msi_alloc_info_t	msiinfo;
#endif
};

#define GPIO_LINE_DIRECTION_IN	1
#define GPIO_LINE_DIRECTION_OUT	0


struct gpio_irq_chip {
	
	struct irq_chip *chip;

	
	struct irq_domain *domain;

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY
	
	struct fwnode_handle *fwnode;

	
	struct irq_domain *parent_domain;

	
	int (*child_to_parent_hwirq)(struct gpio_chip *gc,
				     unsigned int child_hwirq,
				     unsigned int child_type,
				     unsigned int *parent_hwirq,
				     unsigned int *parent_type);

	
	int (*populate_parent_alloc_arg)(struct gpio_chip *gc,
					 union gpio_irq_fwspec *fwspec,
					 unsigned int parent_hwirq,
					 unsigned int parent_type);

	
	unsigned int (*child_offset_to_irq)(struct gpio_chip *gc,
					    unsigned int pin);

	
	struct irq_domain_ops child_irq_domain_ops;
#endif

	
	irq_flow_handler_t handler;

	
	unsigned int default_type;

	
	struct lock_class_key *lock_key;

	
	struct lock_class_key *request_key;

	
	irq_flow_handler_t parent_handler;

	union {
		
		void *parent_handler_data;

		
		void **parent_handler_data_array;
	};

	
	unsigned int num_parents;

	
	unsigned int *parents;

	
	unsigned int *map;

	
	bool threaded;

	
	bool per_parent_data;

	
	bool initialized;

	
	bool domain_is_allocated_externally;

	
	int (*init_hw)(struct gpio_chip *gc);

	
	void (*init_valid_mask)(struct gpio_chip *gc,
				unsigned long *valid_mask,
				unsigned int ngpios);

	
	unsigned long *valid_mask;

	
	unsigned int first;

	
	void		(*irq_enable)(struct irq_data *data);

	
	void		(*irq_disable)(struct irq_data *data);
	
	void		(*irq_unmask)(struct irq_data *data);

	
	void		(*irq_mask)(struct irq_data *data);
};


struct gpio_chip {
	const char		*label;
	struct gpio_device	*gpiodev;
	struct device		*parent;
	struct fwnode_handle	*fwnode;
	struct module		*owner;

	int			(*request)(struct gpio_chip *gc,
						unsigned int offset);
	void			(*free)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*get_direction)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*direction_input)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*direction_output)(struct gpio_chip *gc,
						unsigned int offset, int value);
	int			(*get)(struct gpio_chip *gc,
						unsigned int offset);
	int			(*get_multiple)(struct gpio_chip *gc,
						unsigned long *mask,
						unsigned long *bits);
	void			(*set)(struct gpio_chip *gc,
						unsigned int offset, int value);
	void			(*set_multiple)(struct gpio_chip *gc,
						unsigned long *mask,
						unsigned long *bits);
	int			(*set_config)(struct gpio_chip *gc,
					      unsigned int offset,
					      unsigned long config);
	int			(*to_irq)(struct gpio_chip *gc,
						unsigned int offset);

	void			(*dbg_show)(struct seq_file *s,
						struct gpio_chip *gc);

	int			(*init_valid_mask)(struct gpio_chip *gc,
						   unsigned long *valid_mask,
						   unsigned int ngpios);

	int			(*add_pin_ranges)(struct gpio_chip *gc);

	int			(*en_hw_timestamp)(struct gpio_chip *gc,
						   u32 offset,
						   unsigned long flags);
	int			(*dis_hw_timestamp)(struct gpio_chip *gc,
						    u32 offset,
						    unsigned long flags);
	int			base;
	u16			ngpio;
	u16			offset;
	const char		*const *names;
	bool			can_sleep;

#if IS_ENABLED(CONFIG_GPIO_GENERIC)
	unsigned long (*read_reg)(void __iomem *reg);
	void (*write_reg)(void __iomem *reg, unsigned long data);
	bool be_bits;
	void __iomem *reg_dat;
	void __iomem *reg_set;
	void __iomem *reg_clr;
	void __iomem *reg_dir_out;
	void __iomem *reg_dir_in;
	bool bgpio_dir_unreadable;
	int bgpio_bits;
	raw_spinlock_t bgpio_lock;
	unsigned long bgpio_data;
	unsigned long bgpio_dir;
#endif 

#ifdef CONFIG_GPIOLIB_IRQCHIP
	

	
	struct gpio_irq_chip irq;
#endif 

	
	unsigned long *valid_mask;

#if defined(CONFIG_OF_GPIO)
	

	
	unsigned int of_gpio_n_cells;

	
	int (*of_xlate)(struct gpio_chip *gc,
			const struct of_phandle_args *gpiospec, u32 *flags);
#endif 
};

extern const char *gpiochip_is_requested(struct gpio_chip *gc,
			unsigned int offset);


#define for_each_requested_gpio_in_range(chip, i, base, size, label)			\
	for (i = 0; i < size; i++)							\
		if ((label = gpiochip_is_requested(chip, base + i)) == NULL) {} else


#define for_each_requested_gpio(chip, i, label)						\
	for_each_requested_gpio_in_range(chip, i, 0, chip->ngpio, label)


extern int gpiochip_add_data_with_key(struct gpio_chip *gc, void *data,
				      struct lock_class_key *lock_key,
				      struct lock_class_key *request_key);


#ifdef CONFIG_LOCKDEP
#define gpiochip_add_data(gc, data) ({		\
		static struct lock_class_key lock_key;	\
		static struct lock_class_key request_key;	  \
		gpiochip_add_data_with_key(gc, data, &lock_key, \
					   &request_key);	  \
	})
#define devm_gpiochip_add_data(dev, gc, data) ({ \
		static struct lock_class_key lock_key;	\
		static struct lock_class_key request_key;	  \
		devm_gpiochip_add_data_with_key(dev, gc, data, &lock_key, \
					   &request_key);	  \
	})
#else
#define gpiochip_add_data(gc, data) gpiochip_add_data_with_key(gc, data, NULL, NULL)
#define devm_gpiochip_add_data(dev, gc, data) \
	devm_gpiochip_add_data_with_key(dev, gc, data, NULL, NULL)
#endif 

static inline int gpiochip_add(struct gpio_chip *gc)
{
	return gpiochip_add_data(gc, NULL);
}
extern void gpiochip_remove(struct gpio_chip *gc);
extern int devm_gpiochip_add_data_with_key(struct device *dev, struct gpio_chip *gc, void *data,
					   struct lock_class_key *lock_key,
					   struct lock_class_key *request_key);

extern struct gpio_chip *gpiochip_find(void *data,
			      int (*match)(struct gpio_chip *gc, void *data));

struct gpio_device *gpio_device_find(void *data,
				int (*match)(struct gpio_chip *gc, void *data));

struct gpio_device *gpio_device_get(struct gpio_device *gdev);
void gpio_device_put(struct gpio_device *gdev);

bool gpiochip_line_is_irq(struct gpio_chip *gc, unsigned int offset);
int gpiochip_reqres_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_relres_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_disable_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_enable_irq(struct gpio_chip *gc, unsigned int offset);


int gpiochip_irq_reqres(struct irq_data *data);
void gpiochip_irq_relres(struct irq_data *data);


#define	GPIOCHIP_IRQ_RESOURCE_HELPERS					\
		.irq_request_resources  = gpiochip_irq_reqres,		\
		.irq_release_resources  = gpiochip_irq_relres

static inline void gpio_irq_chip_set_chip(struct gpio_irq_chip *girq,
					  const struct irq_chip *chip)
{
	
	girq->chip = (struct irq_chip *)chip;
}


bool gpiochip_line_is_open_drain(struct gpio_chip *gc, unsigned int offset);
bool gpiochip_line_is_open_source(struct gpio_chip *gc, unsigned int offset);


bool gpiochip_line_is_persistent(struct gpio_chip *gc, unsigned int offset);
bool gpiochip_line_is_valid(const struct gpio_chip *gc, unsigned int offset);


void *gpiochip_get_data(struct gpio_chip *gc);

struct bgpio_pdata {
	const char *label;
	int base;
	int ngpio;
};

#ifdef CONFIG_IRQ_DOMAIN_HIERARCHY

int gpiochip_populate_parent_fwspec_twocell(struct gpio_chip *gc,
					    union gpio_irq_fwspec *gfwspec,
					    unsigned int parent_hwirq,
					    unsigned int parent_type);
int gpiochip_populate_parent_fwspec_fourcell(struct gpio_chip *gc,
					     union gpio_irq_fwspec *gfwspec,
					     unsigned int parent_hwirq,
					     unsigned int parent_type);

#endif 

int bgpio_init(struct gpio_chip *gc, struct device *dev,
	       unsigned long sz, void __iomem *dat, void __iomem *set,
	       void __iomem *clr, void __iomem *dirout, void __iomem *dirin,
	       unsigned long flags);

#define BGPIOF_BIG_ENDIAN		BIT(0)
#define BGPIOF_UNREADABLE_REG_SET	BIT(1) 
#define BGPIOF_UNREADABLE_REG_DIR	BIT(2) 
#define BGPIOF_BIG_ENDIAN_BYTE_ORDER	BIT(3)
#define BGPIOF_READ_OUTPUT_REG_SET	BIT(4) 
#define BGPIOF_NO_OUTPUT		BIT(5) 
#define BGPIOF_NO_SET_ON_INPUT		BIT(6)

int gpiochip_irq_map(struct irq_domain *d, unsigned int irq,
		     irq_hw_number_t hwirq);
void gpiochip_irq_unmap(struct irq_domain *d, unsigned int irq);

int gpiochip_irq_domain_activate(struct irq_domain *domain,
				 struct irq_data *data, bool reserve);
void gpiochip_irq_domain_deactivate(struct irq_domain *domain,
				    struct irq_data *data);

bool gpiochip_irqchip_irq_valid(const struct gpio_chip *gc,
				unsigned int offset);

#ifdef CONFIG_GPIOLIB_IRQCHIP
int gpiochip_irqchip_add_domain(struct gpio_chip *gc,
				struct irq_domain *domain);
#else

#include <asm/bug.h>
#include <asm/errno.h>

static inline int gpiochip_irqchip_add_domain(struct gpio_chip *gc,
					      struct irq_domain *domain)
{
	WARN_ON(1);
	return -EINVAL;
}
#endif

int gpiochip_generic_request(struct gpio_chip *gc, unsigned int offset);
void gpiochip_generic_free(struct gpio_chip *gc, unsigned int offset);
int gpiochip_generic_config(struct gpio_chip *gc, unsigned int offset,
			    unsigned long config);


struct gpio_pin_range {
	struct list_head node;
	struct pinctrl_dev *pctldev;
	struct pinctrl_gpio_range range;
};

#ifdef CONFIG_PINCTRL

int gpiochip_add_pin_range(struct gpio_chip *gc, const char *pinctl_name,
			   unsigned int gpio_offset, unsigned int pin_offset,
			   unsigned int npins);
int gpiochip_add_pingroup_range(struct gpio_chip *gc,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group);
void gpiochip_remove_pin_ranges(struct gpio_chip *gc);

#else 

static inline int
gpiochip_add_pin_range(struct gpio_chip *gc, const char *pinctl_name,
		       unsigned int gpio_offset, unsigned int pin_offset,
		       unsigned int npins)
{
	return 0;
}
static inline int
gpiochip_add_pingroup_range(struct gpio_chip *gc,
			struct pinctrl_dev *pctldev,
			unsigned int gpio_offset, const char *pin_group)
{
	return 0;
}

static inline void
gpiochip_remove_pin_ranges(struct gpio_chip *gc)
{
}

#endif 

struct gpio_desc *gpiochip_request_own_desc(struct gpio_chip *gc,
					    unsigned int hwnum,
					    const char *label,
					    enum gpio_lookup_flags lflags,
					    enum gpiod_flags dflags);
void gpiochip_free_own_desc(struct gpio_desc *desc);

#ifdef CONFIG_GPIOLIB


int gpiochip_lock_as_irq(struct gpio_chip *gc, unsigned int offset);
void gpiochip_unlock_as_irq(struct gpio_chip *gc, unsigned int offset);


struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc);

#else 

#include <linux/err.h>

#include <asm/bug.h>

static inline struct gpio_chip *gpiod_to_chip(const struct gpio_desc *desc)
{
	
	WARN_ON(1);
	return ERR_PTR(-ENODEV);
}

static inline int gpiochip_lock_as_irq(struct gpio_chip *gc,
				       unsigned int offset)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline void gpiochip_unlock_as_irq(struct gpio_chip *gc,
					  unsigned int offset)
{
	WARN_ON(1);
}
#endif 

#define for_each_gpiochip_node(dev, child)					\
	device_for_each_child_node(dev, child)					\
		if (!fwnode_property_present(child, "gpio-controller")) {} else

static inline unsigned int gpiochip_node_count(struct device *dev)
{
	struct fwnode_handle *child;
	unsigned int count = 0;

	for_each_gpiochip_node(dev, child)
		count++;

	return count;
}

static inline struct fwnode_handle *gpiochip_node_get_first(struct device *dev)
{
	struct fwnode_handle *fwnode;

	for_each_gpiochip_node(dev, fwnode)
		return fwnode;

	return NULL;
}

#endif 
