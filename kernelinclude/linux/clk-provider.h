/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_CLK_PROVIDER_H
#define __LINUX_CLK_PROVIDER_H

#include <linux/of.h>
#include <linux/of_clk.h>


#define CLK_SET_RATE_GATE	BIT(0) 
#define CLK_SET_PARENT_GATE	BIT(1) 
#define CLK_SET_RATE_PARENT	BIT(2) 
#define CLK_IGNORE_UNUSED	BIT(3) 
				
				
#define CLK_GET_RATE_NOCACHE	BIT(6) 
#define CLK_SET_RATE_NO_REPARENT BIT(7) 
#define CLK_GET_ACCURACY_NOCACHE BIT(8) 
#define CLK_RECALC_NEW_RATES	BIT(9) 
#define CLK_SET_RATE_UNGATE	BIT(10) 
#define CLK_IS_CRITICAL		BIT(11) 

#define CLK_OPS_PARENT_ENABLE	BIT(12)

#define CLK_DUTY_CYCLE_PARENT	BIT(13)

struct clk;
struct clk_hw;
struct clk_core;
struct dentry;


struct clk_rate_request {
	struct clk_core *core;
	unsigned long rate;
	unsigned long min_rate;
	unsigned long max_rate;
	unsigned long best_parent_rate;
	struct clk_hw *best_parent_hw;
};

void clk_hw_init_rate_request(const struct clk_hw *hw,
			      struct clk_rate_request *req,
			      unsigned long rate);
void clk_hw_forward_rate_request(const struct clk_hw *core,
				 const struct clk_rate_request *old_req,
				 const struct clk_hw *parent,
				 struct clk_rate_request *req,
				 unsigned long parent_rate);


struct clk_duty {
	unsigned int num;
	unsigned int den;
};


struct clk_ops {
	int		(*prepare)(struct clk_hw *hw);
	void		(*unprepare)(struct clk_hw *hw);
	int		(*is_prepared)(struct clk_hw *hw);
	void		(*unprepare_unused)(struct clk_hw *hw);
	int		(*enable)(struct clk_hw *hw);
	void		(*disable)(struct clk_hw *hw);
	int		(*is_enabled)(struct clk_hw *hw);
	void		(*disable_unused)(struct clk_hw *hw);
	int		(*save_context)(struct clk_hw *hw);
	void		(*restore_context)(struct clk_hw *hw);
	unsigned long	(*recalc_rate)(struct clk_hw *hw,
					unsigned long parent_rate);
	long		(*round_rate)(struct clk_hw *hw, unsigned long rate,
					unsigned long *parent_rate);
	int		(*determine_rate)(struct clk_hw *hw,
					  struct clk_rate_request *req);
	int		(*set_parent)(struct clk_hw *hw, u8 index);
	u8		(*get_parent)(struct clk_hw *hw);
	int		(*set_rate)(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate);
	int		(*set_rate_and_parent)(struct clk_hw *hw,
				    unsigned long rate,
				    unsigned long parent_rate, u8 index);
	unsigned long	(*recalc_accuracy)(struct clk_hw *hw,
					   unsigned long parent_accuracy);
	int		(*get_phase)(struct clk_hw *hw);
	int		(*set_phase)(struct clk_hw *hw, int degrees);
	int		(*get_duty_cycle)(struct clk_hw *hw,
					  struct clk_duty *duty);
	int		(*set_duty_cycle)(struct clk_hw *hw,
					  struct clk_duty *duty);
	int		(*init)(struct clk_hw *hw);
	void		(*terminate)(struct clk_hw *hw);
	void		(*debug_init)(struct clk_hw *hw, struct dentry *dentry);
};


struct clk_parent_data {
	const struct clk_hw	*hw;
	const char		*fw_name;
	const char		*name;
	int			index;
};


struct clk_init_data {
	const char		*name;
	const struct clk_ops	*ops;
	
	const char		* const *parent_names;
	const struct clk_parent_data	*parent_data;
	const struct clk_hw		**parent_hws;
	u8			num_parents;
	unsigned long		flags;
};


struct clk_hw {
	struct clk_core *core;
	struct clk *clk;
	const struct clk_init_data *init;
};




struct clk_fixed_rate {
	struct		clk_hw hw;
	unsigned long	fixed_rate;
	unsigned long	fixed_accuracy;
	unsigned long	flags;
};

#define CLK_FIXED_RATE_PARENT_ACCURACY	BIT(0)

extern const struct clk_ops clk_fixed_rate_ops;
struct clk_hw *__clk_hw_register_fixed_rate(struct device *dev,
		struct device_node *np, const char *name,
		const char *parent_name, const struct clk_hw *parent_hw,
		const struct clk_parent_data *parent_data, unsigned long flags,
		unsigned long fixed_rate, unsigned long fixed_accuracy,
		unsigned long clk_fixed_flags, bool devm);
struct clk *clk_register_fixed_rate(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		unsigned long fixed_rate);

#define clk_hw_register_fixed_rate(dev, name, parent_name, flags, fixed_rate)  \
	__clk_hw_register_fixed_rate((dev), NULL, (name), (parent_name), NULL, \
				     NULL, (flags), (fixed_rate), 0, 0, false)


#define devm_clk_hw_register_fixed_rate(dev, name, parent_name, flags, fixed_rate)  \
	__clk_hw_register_fixed_rate((dev), NULL, (name), (parent_name), NULL, \
				     NULL, (flags), (fixed_rate), 0, 0, true)

#define clk_hw_register_fixed_rate_parent_hw(dev, name, parent_hw, flags,     \
					     fixed_rate)		      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), NULL, (parent_hw),  \
				     NULL, (flags), (fixed_rate), 0, 0, false)

#define clk_hw_register_fixed_rate_parent_data(dev, name, parent_data, flags, \
					     fixed_rate)		      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), NULL, NULL,	      \
				     (parent_data), (flags), (fixed_rate), 0, \
				     0, false)

#define clk_hw_register_fixed_rate_with_accuracy(dev, name, parent_name,      \
						 flags, fixed_rate,	      \
						 fixed_accuracy)	      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), (parent_name),      \
				     NULL, NULL, (flags), (fixed_rate),       \
				     (fixed_accuracy), 0, false)

#define clk_hw_register_fixed_rate_with_accuracy_parent_hw(dev, name,	      \
		parent_hw, flags, fixed_rate, fixed_accuracy)		      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), NULL, (parent_hw),  \
				     NULL, (flags), (fixed_rate),	      \
				     (fixed_accuracy), 0, false)

#define clk_hw_register_fixed_rate_with_accuracy_parent_data(dev, name,	      \
		parent_data, flags, fixed_rate, fixed_accuracy)		      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), NULL, NULL,	      \
				     (parent_data), NULL, (flags),	      \
				     (fixed_rate), (fixed_accuracy), 0, false)

#define clk_hw_register_fixed_rate_parent_accuracy(dev, name, parent_data,    \
						   flags, fixed_rate)	      \
	__clk_hw_register_fixed_rate((dev), NULL, (name), NULL, NULL,      \
				     (parent_data), (flags), (fixed_rate), 0,    \
				     CLK_FIXED_RATE_PARENT_ACCURACY, false)

void clk_unregister_fixed_rate(struct clk *clk);
void clk_hw_unregister_fixed_rate(struct clk_hw *hw);

void of_fixed_clk_setup(struct device_node *np);


struct clk_gate {
	struct clk_hw hw;
	void __iomem	*reg;
	u8		bit_idx;
	u8		flags;
	spinlock_t	*lock;
};

#define to_clk_gate(_hw) container_of(_hw, struct clk_gate, hw)

#define CLK_GATE_SET_TO_DISABLE		BIT(0)
#define CLK_GATE_HIWORD_MASK		BIT(1)
#define CLK_GATE_BIG_ENDIAN		BIT(2)

extern const struct clk_ops clk_gate_ops;
struct clk_hw *__clk_hw_register_gate(struct device *dev,
		struct device_node *np, const char *name,
		const char *parent_name, const struct clk_hw *parent_hw,
		const struct clk_parent_data *parent_data,
		unsigned long flags,
		void __iomem *reg, u8 bit_idx,
		u8 clk_gate_flags, spinlock_t *lock);
struct clk_hw *__devm_clk_hw_register_gate(struct device *dev,
		struct device_node *np, const char *name,
		const char *parent_name, const struct clk_hw *parent_hw,
		const struct clk_parent_data *parent_data,
		unsigned long flags,
		void __iomem *reg, u8 bit_idx,
		u8 clk_gate_flags, spinlock_t *lock);
struct clk *clk_register_gate(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 bit_idx,
		u8 clk_gate_flags, spinlock_t *lock);

#define clk_hw_register_gate(dev, name, parent_name, flags, reg, bit_idx,     \
			     clk_gate_flags, lock)			      \
	__clk_hw_register_gate((dev), NULL, (name), (parent_name), NULL,      \
			       NULL, (flags), (reg), (bit_idx),		      \
			       (clk_gate_flags), (lock))

#define clk_hw_register_gate_parent_hw(dev, name, parent_hw, flags, reg,      \
				       bit_idx, clk_gate_flags, lock)	      \
	__clk_hw_register_gate((dev), NULL, (name), NULL, (parent_hw),        \
			       NULL, (flags), (reg), (bit_idx),		      \
			       (clk_gate_flags), (lock))

#define clk_hw_register_gate_parent_data(dev, name, parent_data, flags, reg,  \
				       bit_idx, clk_gate_flags, lock)	      \
	__clk_hw_register_gate((dev), NULL, (name), NULL, NULL, (parent_data), \
			       (flags), (reg), (bit_idx),		      \
			       (clk_gate_flags), (lock))

#define devm_clk_hw_register_gate(dev, name, parent_name, flags, reg, bit_idx,\
				  clk_gate_flags, lock)			      \
	__devm_clk_hw_register_gate((dev), NULL, (name), (parent_name), NULL, \
			       NULL, (flags), (reg), (bit_idx),		      \
			       (clk_gate_flags), (lock))

#define devm_clk_hw_register_gate_parent_data(dev, name, parent_data, flags,  \
					      reg, bit_idx, clk_gate_flags,   \
					      lock)			      \
	__devm_clk_hw_register_gate((dev), NULL, (name), NULL, NULL,	      \
				    (parent_data), (flags), (reg), (bit_idx), \
				    (clk_gate_flags), (lock))

void clk_unregister_gate(struct clk *clk);
void clk_hw_unregister_gate(struct clk_hw *hw);
int clk_gate_is_enabled(struct clk_hw *hw);

struct clk_div_table {
	unsigned int	val;
	unsigned int	div;
};


struct clk_divider {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		shift;
	u8		width;
	u8		flags;
	const struct clk_div_table	*table;
	spinlock_t	*lock;
};

#define clk_div_mask(width)	((1 << (width)) - 1)
#define to_clk_divider(_hw) container_of(_hw, struct clk_divider, hw)

#define CLK_DIVIDER_ONE_BASED		BIT(0)
#define CLK_DIVIDER_POWER_OF_TWO	BIT(1)
#define CLK_DIVIDER_ALLOW_ZERO		BIT(2)
#define CLK_DIVIDER_HIWORD_MASK		BIT(3)
#define CLK_DIVIDER_ROUND_CLOSEST	BIT(4)
#define CLK_DIVIDER_READ_ONLY		BIT(5)
#define CLK_DIVIDER_MAX_AT_ZERO		BIT(6)
#define CLK_DIVIDER_BIG_ENDIAN		BIT(7)

extern const struct clk_ops clk_divider_ops;
extern const struct clk_ops clk_divider_ro_ops;

unsigned long divider_recalc_rate(struct clk_hw *hw, unsigned long parent_rate,
		unsigned int val, const struct clk_div_table *table,
		unsigned long flags, unsigned long width);
long divider_round_rate_parent(struct clk_hw *hw, struct clk_hw *parent,
			       unsigned long rate, unsigned long *prate,
			       const struct clk_div_table *table,
			       u8 width, unsigned long flags);
long divider_ro_round_rate_parent(struct clk_hw *hw, struct clk_hw *parent,
				  unsigned long rate, unsigned long *prate,
				  const struct clk_div_table *table, u8 width,
				  unsigned long flags, unsigned int val);
int divider_determine_rate(struct clk_hw *hw, struct clk_rate_request *req,
			   const struct clk_div_table *table, u8 width,
			   unsigned long flags);
int divider_ro_determine_rate(struct clk_hw *hw, struct clk_rate_request *req,
			      const struct clk_div_table *table, u8 width,
			      unsigned long flags, unsigned int val);
int divider_get_val(unsigned long rate, unsigned long parent_rate,
		const struct clk_div_table *table, u8 width,
		unsigned long flags);

struct clk_hw *__clk_hw_register_divider(struct device *dev,
		struct device_node *np, const char *name,
		const char *parent_name, const struct clk_hw *parent_hw,
		const struct clk_parent_data *parent_data, unsigned long flags,
		void __iomem *reg, u8 shift, u8 width, u8 clk_divider_flags,
		const struct clk_div_table *table, spinlock_t *lock);
struct clk_hw *__devm_clk_hw_register_divider(struct device *dev,
		struct device_node *np, const char *name,
		const char *parent_name, const struct clk_hw *parent_hw,
		const struct clk_parent_data *parent_data, unsigned long flags,
		void __iomem *reg, u8 shift, u8 width, u8 clk_divider_flags,
		const struct clk_div_table *table, spinlock_t *lock);
struct clk *clk_register_divider_table(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 shift, u8 width,
		u8 clk_divider_flags, const struct clk_div_table *table,
		spinlock_t *lock);

#define clk_register_divider(dev, name, parent_name, flags, reg, shift, width, \
			     clk_divider_flags, lock)			       \
	clk_register_divider_table((dev), (name), (parent_name), (flags),      \
				   (reg), (shift), (width),		       \
				   (clk_divider_flags), NULL, (lock))

#define clk_hw_register_divider(dev, name, parent_name, flags, reg, shift,    \
				width, clk_divider_flags, lock)		      \
	__clk_hw_register_divider((dev), NULL, (name), (parent_name), NULL,   \
				  NULL, (flags), (reg), (shift), (width),     \
				  (clk_divider_flags), NULL, (lock))

#define clk_hw_register_divider_parent_hw(dev, name, parent_hw, flags, reg,   \
					  shift, width, clk_divider_flags,    \
					  lock)				      \
	__clk_hw_register_divider((dev), NULL, (name), NULL, (parent_hw),     \
				  NULL, (flags), (reg), (shift), (width),     \
				  (clk_divider_flags), NULL, (lock))

#define clk_hw_register_divider_parent_data(dev, name, parent_data, flags,    \
					    reg, shift, width,		      \
					    clk_divider_flags, lock)	      \
	__clk_hw_register_divider((dev), NULL, (name), NULL, NULL,	      \
				  (parent_data), (flags), (reg), (shift),     \
				  (width), (clk_divider_flags), NULL, (lock))

#define clk_hw_register_divider_table(dev, name, parent_name, flags, reg,     \
				      shift, width, clk_divider_flags, table, \
				      lock)				      \
	__clk_hw_register_divider((dev), NULL, (name), (parent_name), NULL,   \
				  NULL, (flags), (reg), (shift), (width),     \
				  (clk_divider_flags), (table), (lock))

#define clk_hw_register_divider_table_parent_hw(dev, name, parent_hw, flags,  \
						reg, shift, width,	      \
						clk_divider_flags, table,     \
						lock)			      \
	__clk_hw_register_divider((dev), NULL, (name), NULL, (parent_hw),     \
				  NULL, (flags), (reg), (shift), (width),     \
				  (clk_divider_flags), (table), (lock))

#define clk_hw_register_divider_table_parent_data(dev, name, parent_data,     \
						  flags, reg, shift, width,   \
						  clk_divider_flags, table,   \
						  lock)			      \
	__clk_hw_register_divider((dev), NULL, (name), NULL, NULL,	      \
				  (parent_data), (flags), (reg), (shift),     \
				  (width), (clk_divider_flags), (table),      \
				  (lock))

#define devm_clk_hw_register_divider(dev, name, parent_name, flags, reg, shift,    \
				width, clk_divider_flags, lock)		      \
	__devm_clk_hw_register_divider((dev), NULL, (name), (parent_name), NULL,   \
				  NULL, (flags), (reg), (shift), (width),     \
				  (clk_divider_flags), NULL, (lock))

#define devm_clk_hw_register_divider_parent_hw(dev, name, parent_hw, flags,   \
					       reg, shift, width,	      \
					       clk_divider_flags, lock)       \
	__devm_clk_hw_register_divider((dev), NULL, (name), NULL,	      \
				       (parent_hw), NULL, (flags), (reg),     \
				       (shift), (width), (clk_divider_flags), \
				       NULL, (lock))

#define devm_clk_hw_register_divider_table(dev, name, parent_name, flags,     \
					   reg, shift, width,		      \
					   clk_divider_flags, table, lock)    \
	__devm_clk_hw_register_divider((dev), NULL, (name), (parent_name),    \
				       NULL, NULL, (flags), (reg), (shift),   \
				       (width), (clk_divider_flags), (table), \
				       (lock))

void clk_unregister_divider(struct clk *clk);
void clk_hw_unregister_divider(struct clk_hw *hw);


struct clk_mux {
	struct clk_hw	hw;
	void __iomem	*reg;
	const u32	*table;
	u32		mask;
	u8		shift;
	u8		flags;
	spinlock_t	*lock;
};

#define to_clk_mux(_hw) container_of(_hw, struct clk_mux, hw)

#define CLK_MUX_INDEX_ONE		BIT(0)
#define CLK_MUX_INDEX_BIT		BIT(1)
#define CLK_MUX_HIWORD_MASK		BIT(2)
#define CLK_MUX_READ_ONLY		BIT(3) 
#define CLK_MUX_ROUND_CLOSEST		BIT(4)
#define CLK_MUX_BIG_ENDIAN		BIT(5)

extern const struct clk_ops clk_mux_ops;
extern const struct clk_ops clk_mux_ro_ops;

struct clk_hw *__clk_hw_register_mux(struct device *dev, struct device_node *np,
		const char *name, u8 num_parents,
		const char * const *parent_names,
		const struct clk_hw **parent_hws,
		const struct clk_parent_data *parent_data,
		unsigned long flags, void __iomem *reg, u8 shift, u32 mask,
		u8 clk_mux_flags, const u32 *table, spinlock_t *lock);
struct clk_hw *__devm_clk_hw_register_mux(struct device *dev, struct device_node *np,
		const char *name, u8 num_parents,
		const char * const *parent_names,
		const struct clk_hw **parent_hws,
		const struct clk_parent_data *parent_data,
		unsigned long flags, void __iomem *reg, u8 shift, u32 mask,
		u8 clk_mux_flags, const u32 *table, spinlock_t *lock);
struct clk *clk_register_mux_table(struct device *dev, const char *name,
		const char * const *parent_names, u8 num_parents,
		unsigned long flags, void __iomem *reg, u8 shift, u32 mask,
		u8 clk_mux_flags, const u32 *table, spinlock_t *lock);

#define clk_register_mux(dev, name, parent_names, num_parents, flags, reg,    \
			 shift, width, clk_mux_flags, lock)		      \
	clk_register_mux_table((dev), (name), (parent_names), (num_parents),  \
			       (flags), (reg), (shift), BIT((width)) - 1,     \
			       (clk_mux_flags), NULL, (lock))
#define clk_hw_register_mux_table(dev, name, parent_names, num_parents,	      \
				  flags, reg, shift, mask, clk_mux_flags,     \
				  table, lock)				      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents),	      \
			      (parent_names), NULL, NULL, (flags), (reg),     \
			      (shift), (mask), (clk_mux_flags), (table),      \
			      (lock))
#define clk_hw_register_mux_table_parent_data(dev, name, parent_data,	      \
				  num_parents, flags, reg, shift, mask,	      \
				  clk_mux_flags, table, lock)		      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents),	      \
			      NULL, NULL, (parent_data), (flags), (reg),      \
			      (shift), (mask), (clk_mux_flags), (table),      \
			      (lock))
#define clk_hw_register_mux(dev, name, parent_names, num_parents, flags, reg, \
			    shift, width, clk_mux_flags, lock)		      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents),	      \
			      (parent_names), NULL, NULL, (flags), (reg),     \
			      (shift), BIT((width)) - 1, (clk_mux_flags),     \
			      NULL, (lock))
#define clk_hw_register_mux_hws(dev, name, parent_hws, num_parents, flags,    \
				reg, shift, width, clk_mux_flags, lock)	      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents), NULL,	      \
			      (parent_hws), NULL, (flags), (reg), (shift),    \
			      BIT((width)) - 1, (clk_mux_flags), NULL, (lock))
#define clk_hw_register_mux_parent_data(dev, name, parent_data, num_parents,  \
					flags, reg, shift, width,	      \
					clk_mux_flags, lock)		      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents), NULL, NULL, \
			      (parent_data), (flags), (reg), (shift),	      \
			      BIT((width)) - 1, (clk_mux_flags), NULL, (lock))
#define clk_hw_register_mux_parent_data_table(dev, name, parent_data,	      \
					      num_parents, flags, reg, shift, \
					      width, clk_mux_flags, table,    \
					      lock)			      \
	__clk_hw_register_mux((dev), NULL, (name), (num_parents), NULL, NULL, \
			      (parent_data), (flags), (reg), (shift),	      \
			      BIT((width)) - 1, (clk_mux_flags), table, (lock))
#define devm_clk_hw_register_mux(dev, name, parent_names, num_parents, flags, reg, \
			    shift, width, clk_mux_flags, lock)		      \
	__devm_clk_hw_register_mux((dev), NULL, (name), (num_parents),	      \
			      (parent_names), NULL, NULL, (flags), (reg),     \
			      (shift), BIT((width)) - 1, (clk_mux_flags),     \
			      NULL, (lock))
#define devm_clk_hw_register_mux_parent_hws(dev, name, parent_hws,	      \
					    num_parents, flags, reg, shift,   \
					    width, clk_mux_flags, lock)       \
	__devm_clk_hw_register_mux((dev), NULL, (name), (num_parents), NULL,  \
				   (parent_hws), NULL, (flags), (reg),        \
				   (shift), BIT((width)) - 1,		      \
				   (clk_mux_flags), NULL, (lock))
#define devm_clk_hw_register_mux_parent_data_table(dev, name, parent_data,    \
					      num_parents, flags, reg, shift, \
					      width, clk_mux_flags, table,    \
					      lock)			      \
	__devm_clk_hw_register_mux((dev), NULL, (name), (num_parents), NULL,  \
			      NULL, (parent_data), (flags), (reg), (shift),   \
			      BIT((width)) - 1, (clk_mux_flags), table, (lock))

int clk_mux_val_to_index(struct clk_hw *hw, const u32 *table, unsigned int flags,
			 unsigned int val);
unsigned int clk_mux_index_to_val(const u32 *table, unsigned int flags, u8 index);

void clk_unregister_mux(struct clk *clk);
void clk_hw_unregister_mux(struct clk_hw *hw);

void of_fixed_factor_clk_setup(struct device_node *node);



struct clk_fixed_factor {
	struct clk_hw	hw;
	unsigned int	mult;
	unsigned int	div;
};

#define to_clk_fixed_factor(_hw) container_of(_hw, struct clk_fixed_factor, hw)

extern const struct clk_ops clk_fixed_factor_ops;
struct clk *clk_register_fixed_factor(struct device *dev, const char *name,
		const char *parent_name, unsigned long flags,
		unsigned int mult, unsigned int div);
void clk_unregister_fixed_factor(struct clk *clk);
struct clk_hw *clk_hw_register_fixed_factor(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		unsigned int mult, unsigned int div);
void clk_hw_unregister_fixed_factor(struct clk_hw *hw);
struct clk_hw *devm_clk_hw_register_fixed_factor(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		unsigned int mult, unsigned int div);
struct clk_hw *devm_clk_hw_register_fixed_factor_index(struct device *dev,
		const char *name, unsigned int index, unsigned long flags,
		unsigned int mult, unsigned int div);

struct clk_hw *devm_clk_hw_register_fixed_factor_parent_hw(struct device *dev,
		const char *name, const struct clk_hw *parent_hw,
		unsigned long flags, unsigned int mult, unsigned int div);

struct clk_hw *clk_hw_register_fixed_factor_parent_hw(struct device *dev,
		const char *name, const struct clk_hw *parent_hw,
		unsigned long flags, unsigned int mult, unsigned int div);

struct clk_fractional_divider {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		mshift;
	u8		mwidth;
	u8		nshift;
	u8		nwidth;
	u8		flags;
	void		(*approximation)(struct clk_hw *hw,
				unsigned long rate, unsigned long *parent_rate,
				unsigned long *m, unsigned long *n);
	spinlock_t	*lock;
};

#define to_clk_fd(_hw) container_of(_hw, struct clk_fractional_divider, hw)

#define CLK_FRAC_DIVIDER_ZERO_BASED		BIT(0)
#define CLK_FRAC_DIVIDER_BIG_ENDIAN		BIT(1)
#define CLK_FRAC_DIVIDER_POWER_OF_TWO_PS	BIT(2)

struct clk *clk_register_fractional_divider(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags, spinlock_t *lock);
struct clk_hw *clk_hw_register_fractional_divider(struct device *dev,
		const char *name, const char *parent_name, unsigned long flags,
		void __iomem *reg, u8 mshift, u8 mwidth, u8 nshift, u8 nwidth,
		u8 clk_divider_flags, spinlock_t *lock);
void clk_hw_unregister_fractional_divider(struct clk_hw *hw);


struct clk_multiplier {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		shift;
	u8		width;
	u8		flags;
	spinlock_t	*lock;
};

#define to_clk_multiplier(_hw) container_of(_hw, struct clk_multiplier, hw)

#define CLK_MULTIPLIER_ZERO_BYPASS	BIT(0)
#define CLK_MULTIPLIER_ROUND_CLOSEST	BIT(1)
#define CLK_MULTIPLIER_BIG_ENDIAN	BIT(2)

extern const struct clk_ops clk_multiplier_ops;


struct clk_composite {
	struct clk_hw	hw;
	struct clk_ops	ops;

	struct clk_hw	*mux_hw;
	struct clk_hw	*rate_hw;
	struct clk_hw	*gate_hw;

	const struct clk_ops	*mux_ops;
	const struct clk_ops	*rate_ops;
	const struct clk_ops	*gate_ops;
};

#define to_clk_composite(_hw) container_of(_hw, struct clk_composite, hw)

struct clk *clk_register_composite(struct device *dev, const char *name,
		const char * const *parent_names, int num_parents,
		struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
		struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
		struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
		unsigned long flags);
struct clk *clk_register_composite_pdata(struct device *dev, const char *name,
		const struct clk_parent_data *parent_data, int num_parents,
		struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
		struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
		struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
		unsigned long flags);
void clk_unregister_composite(struct clk *clk);
struct clk_hw *clk_hw_register_composite(struct device *dev, const char *name,
		const char * const *parent_names, int num_parents,
		struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
		struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
		struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
		unsigned long flags);
struct clk_hw *clk_hw_register_composite_pdata(struct device *dev,
		const char *name,
		const struct clk_parent_data *parent_data, int num_parents,
		struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
		struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
		struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
		unsigned long flags);
struct clk_hw *devm_clk_hw_register_composite_pdata(struct device *dev,
		const char *name, const struct clk_parent_data *parent_data,
		int num_parents,
		struct clk_hw *mux_hw, const struct clk_ops *mux_ops,
		struct clk_hw *rate_hw, const struct clk_ops *rate_ops,
		struct clk_hw *gate_hw, const struct clk_ops *gate_ops,
		unsigned long flags);
void clk_hw_unregister_composite(struct clk_hw *hw);

struct clk *clk_register(struct device *dev, struct clk_hw *hw);
struct clk *devm_clk_register(struct device *dev, struct clk_hw *hw);

int __must_check clk_hw_register(struct device *dev, struct clk_hw *hw);
int __must_check devm_clk_hw_register(struct device *dev, struct clk_hw *hw);
int __must_check of_clk_hw_register(struct device_node *node, struct clk_hw *hw);

void clk_unregister(struct clk *clk);

void clk_hw_unregister(struct clk_hw *hw);


const char *__clk_get_name(const struct clk *clk);
const char *clk_hw_get_name(const struct clk_hw *hw);
#ifdef CONFIG_COMMON_CLK
struct clk_hw *__clk_get_hw(struct clk *clk);
#else
static inline struct clk_hw *__clk_get_hw(struct clk *clk)
{
	return (struct clk_hw *)clk;
}
#endif

struct clk *clk_hw_get_clk(struct clk_hw *hw, const char *con_id);
struct clk *devm_clk_hw_get_clk(struct device *dev, struct clk_hw *hw,
				const char *con_id);

unsigned int clk_hw_get_num_parents(const struct clk_hw *hw);
struct clk_hw *clk_hw_get_parent(const struct clk_hw *hw);
struct clk_hw *clk_hw_get_parent_by_index(const struct clk_hw *hw,
					  unsigned int index);
int clk_hw_get_parent_index(struct clk_hw *hw);
int clk_hw_set_parent(struct clk_hw *hw, struct clk_hw *new_parent);
unsigned int __clk_get_enable_count(struct clk *clk);
unsigned long clk_hw_get_rate(const struct clk_hw *hw);
unsigned long clk_hw_get_flags(const struct clk_hw *hw);
#define clk_hw_can_set_rate_parent(hw) \
	(clk_hw_get_flags((hw)) & CLK_SET_RATE_PARENT)

bool clk_hw_is_prepared(const struct clk_hw *hw);
bool clk_hw_rate_is_protected(const struct clk_hw *hw);
bool clk_hw_is_enabled(const struct clk_hw *hw);
bool __clk_is_enabled(struct clk *clk);
struct clk *__clk_lookup(const char *name);
int __clk_mux_determine_rate(struct clk_hw *hw,
			     struct clk_rate_request *req);
int __clk_determine_rate(struct clk_hw *core, struct clk_rate_request *req);
int __clk_mux_determine_rate_closest(struct clk_hw *hw,
				     struct clk_rate_request *req);
int clk_mux_determine_rate_flags(struct clk_hw *hw,
				 struct clk_rate_request *req,
				 unsigned long flags);
int clk_hw_determine_rate_no_reparent(struct clk_hw *hw,
				      struct clk_rate_request *req);
void clk_hw_reparent(struct clk_hw *hw, struct clk_hw *new_parent);
void clk_hw_get_rate_range(struct clk_hw *hw, unsigned long *min_rate,
			   unsigned long *max_rate);
void clk_hw_set_rate_range(struct clk_hw *hw, unsigned long min_rate,
			   unsigned long max_rate);

static inline void __clk_hw_set_clk(struct clk_hw *dst, struct clk_hw *src)
{
	dst->clk = src->clk;
	dst->core = src->core;
}

static inline long divider_round_rate(struct clk_hw *hw, unsigned long rate,
				      unsigned long *prate,
				      const struct clk_div_table *table,
				      u8 width, unsigned long flags)
{
	return divider_round_rate_parent(hw, clk_hw_get_parent(hw),
					 rate, prate, table, width, flags);
}

static inline long divider_ro_round_rate(struct clk_hw *hw, unsigned long rate,
					 unsigned long *prate,
					 const struct clk_div_table *table,
					 u8 width, unsigned long flags,
					 unsigned int val)
{
	return divider_ro_round_rate_parent(hw, clk_hw_get_parent(hw),
					    rate, prate, table, width, flags,
					    val);
}


unsigned long clk_hw_round_rate(struct clk_hw *hw, unsigned long rate);

struct clk_onecell_data {
	struct clk **clks;
	unsigned int clk_num;
};

struct clk_hw_onecell_data {
	unsigned int num;
	struct clk_hw *hws[] __counted_by(num);
};

#define CLK_OF_DECLARE(name, compat, fn) \
	static void __init __##name##_of_clk_init_declare(struct device_node *np) \
	{								\
		fn(np);							\
		fwnode_dev_initialized(of_fwnode_handle(np), true);	\
	}								\
	OF_DECLARE_1(clk, name, compat, __##name##_of_clk_init_declare)


#define CLK_OF_DECLARE_DRIVER(name, compat, fn) \
	static void __init name##_of_clk_init_driver(struct device_node *np) \
	{								\
		of_node_clear_flag(np, OF_POPULATED);			\
		fn(np);							\
	}								\
	OF_DECLARE_1(clk, name, compat, name##_of_clk_init_driver)

#define CLK_HW_INIT(_name, _parent, _ops, _flags)		\
	(&(struct clk_init_data) {				\
		.flags		= _flags,			\
		.name		= _name,			\
		.parent_names	= (const char *[]) { _parent },	\
		.num_parents	= 1,				\
		.ops		= _ops,				\
	})

#define CLK_HW_INIT_HW(_name, _parent, _ops, _flags)			\
	(&(struct clk_init_data) {					\
		.flags		= _flags,				\
		.name		= _name,				\
		.parent_hws	= (const struct clk_hw*[]) { _parent },	\
		.num_parents	= 1,					\
		.ops		= _ops,					\
	})


#define CLK_HW_INIT_HWS(_name, _parent, _ops, _flags)			\
	(&(struct clk_init_data) {					\
		.flags		= _flags,				\
		.name		= _name,				\
		.parent_hws	= _parent,				\
		.num_parents	= 1,					\
		.ops		= _ops,					\
	})

#define CLK_HW_INIT_FW_NAME(_name, _parent, _ops, _flags)		\
	(&(struct clk_init_data) {					\
		.flags		= _flags,				\
		.name		= _name,				\
		.parent_data	= (const struct clk_parent_data[]) {	\
					{ .fw_name = _parent },		\
				  },					\
		.num_parents	= 1,					\
		.ops		= _ops,					\
	})

#define CLK_HW_INIT_PARENTS(_name, _parents, _ops, _flags)	\
	(&(struct clk_init_data) {				\
		.flags		= _flags,			\
		.name		= _name,			\
		.parent_names	= _parents,			\
		.num_parents	= ARRAY_SIZE(_parents),		\
		.ops		= _ops,				\
	})

#define CLK_HW_INIT_PARENTS_HW(_name, _parents, _ops, _flags)	\
	(&(struct clk_init_data) {				\
		.flags		= _flags,			\
		.name		= _name,			\
		.parent_hws	= _parents,			\
		.num_parents	= ARRAY_SIZE(_parents),		\
		.ops		= _ops,				\
	})

#define CLK_HW_INIT_PARENTS_DATA(_name, _parents, _ops, _flags)	\
	(&(struct clk_init_data) {				\
		.flags		= _flags,			\
		.name		= _name,			\
		.parent_data	= _parents,			\
		.num_parents	= ARRAY_SIZE(_parents),		\
		.ops		= _ops,				\
	})

#define CLK_HW_INIT_NO_PARENT(_name, _ops, _flags)	\
	(&(struct clk_init_data) {			\
		.flags          = _flags,		\
		.name           = _name,		\
		.parent_names   = NULL,			\
		.num_parents    = 0,			\
		.ops            = _ops,			\
	})

#define CLK_FIXED_FACTOR(_struct, _name, _parent,			\
			_div, _mult, _flags)				\
	struct clk_fixed_factor _struct = {				\
		.div		= _div,					\
		.mult		= _mult,				\
		.hw.init	= CLK_HW_INIT(_name,			\
					      _parent,			\
					      &clk_fixed_factor_ops,	\
					      _flags),			\
	}

#define CLK_FIXED_FACTOR_HW(_struct, _name, _parent,			\
			    _div, _mult, _flags)			\
	struct clk_fixed_factor _struct = {				\
		.div		= _div,					\
		.mult		= _mult,				\
		.hw.init	= CLK_HW_INIT_HW(_name,			\
						 _parent,		\
						 &clk_fixed_factor_ops,	\
						 _flags),		\
	}


#define CLK_FIXED_FACTOR_HWS(_struct, _name, _parent,			\
			     _div, _mult, _flags)			\
	struct clk_fixed_factor _struct = {				\
		.div		= _div,					\
		.mult		= _mult,				\
		.hw.init	= CLK_HW_INIT_HWS(_name,		\
						  _parent,		\
						  &clk_fixed_factor_ops, \
						  _flags),	\
	}

#define CLK_FIXED_FACTOR_FW_NAME(_struct, _name, _parent,		\
				 _div, _mult, _flags)			\
	struct clk_fixed_factor _struct = {				\
		.div		= _div,					\
		.mult		= _mult,				\
		.hw.init	= CLK_HW_INIT_FW_NAME(_name,		\
						      _parent,		\
						      &clk_fixed_factor_ops, \
						      _flags),		\
	}

#ifdef CONFIG_OF
int of_clk_add_provider(struct device_node *np,
			struct clk *(*clk_src_get)(struct of_phandle_args *args,
						   void *data),
			void *data);
int of_clk_add_hw_provider(struct device_node *np,
			   struct clk_hw *(*get)(struct of_phandle_args *clkspec,
						 void *data),
			   void *data);
int devm_of_clk_add_hw_provider(struct device *dev,
			   struct clk_hw *(*get)(struct of_phandle_args *clkspec,
						 void *data),
			   void *data);
void of_clk_del_provider(struct device_node *np);

struct clk *of_clk_src_simple_get(struct of_phandle_args *clkspec,
				  void *data);
struct clk_hw *of_clk_hw_simple_get(struct of_phandle_args *clkspec,
				    void *data);
struct clk *of_clk_src_onecell_get(struct of_phandle_args *clkspec, void *data);
struct clk_hw *of_clk_hw_onecell_get(struct of_phandle_args *clkspec,
				     void *data);
int of_clk_parent_fill(struct device_node *np, const char **parents,
		       unsigned int size);
int of_clk_detect_critical(struct device_node *np, int index,
			    unsigned long *flags);

#else 

static inline int of_clk_add_provider(struct device_node *np,
			struct clk *(*clk_src_get)(struct of_phandle_args *args,
						   void *data),
			void *data)
{
	return 0;
}
static inline int of_clk_add_hw_provider(struct device_node *np,
			struct clk_hw *(*get)(struct of_phandle_args *clkspec,
					      void *data),
			void *data)
{
	return 0;
}
static inline int devm_of_clk_add_hw_provider(struct device *dev,
			   struct clk_hw *(*get)(struct of_phandle_args *clkspec,
						 void *data),
			   void *data)
{
	return 0;
}
static inline void of_clk_del_provider(struct device_node *np) {}

static inline struct clk *of_clk_src_simple_get(
	struct of_phandle_args *clkspec, void *data)
{
	return ERR_PTR(-ENOENT);
}
static inline struct clk_hw *
of_clk_hw_simple_get(struct of_phandle_args *clkspec, void *data)
{
	return ERR_PTR(-ENOENT);
}
static inline struct clk *of_clk_src_onecell_get(
	struct of_phandle_args *clkspec, void *data)
{
	return ERR_PTR(-ENOENT);
}
static inline struct clk_hw *
of_clk_hw_onecell_get(struct of_phandle_args *clkspec, void *data)
{
	return ERR_PTR(-ENOENT);
}
static inline int of_clk_parent_fill(struct device_node *np,
				     const char **parents, unsigned int size)
{
	return 0;
}
static inline int of_clk_detect_critical(struct device_node *np, int index,
					  unsigned long *flags)
{
	return 0;
}
#endif 

void clk_gate_restore_context(struct clk_hw *hw);

#endif 
