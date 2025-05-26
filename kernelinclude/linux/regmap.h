/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __LINUX_REGMAP_H
#define __LINUX_REGMAP_H



#include <linux/list.h>
#include <linux/rbtree.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/bug.h>
#include <linux/lockdep.h>
#include <linux/iopoll.h>
#include <linux/fwnode.h>

struct module;
struct clk;
struct device;
struct device_node;
struct fsi_device;
struct i2c_client;
struct i3c_device;
struct irq_domain;
struct mdio_device;
struct slim_device;
struct spi_device;
struct spmi_device;
struct regmap;
struct regmap_range_cfg;
struct regmap_field;
struct snd_ac97;
struct sdw_slave;


#define REGMAP_MDIO_C45_DEVAD_SHIFT	16
#define REGMAP_MDIO_C45_DEVAD_MASK	GENMASK(20, 16)
#define REGMAP_MDIO_C45_REGNUM_MASK	GENMASK(15, 0)


#define REGMAP_UPSHIFT(s)	(-(s))
#define REGMAP_DOWNSHIFT(s)	(s)


enum regcache_type {
	REGCACHE_NONE,
	REGCACHE_RBTREE,
	REGCACHE_FLAT,
	REGCACHE_MAPLE,
};


struct reg_default {
	unsigned int reg;
	unsigned int def;
};


struct reg_sequence {
	unsigned int reg;
	unsigned int def;
	unsigned int delay_us;
};

#define REG_SEQ(_reg, _def, _delay_us) {		\
				.reg = _reg,		\
				.def = _def,		\
				.delay_us = _delay_us,	\
				}
#define REG_SEQ0(_reg, _def)	REG_SEQ(_reg, _def, 0)


#define regmap_read_poll_timeout(map, addr, val, cond, sleep_us, timeout_us) \
({ \
	int __ret, __tmp; \
	__tmp = read_poll_timeout(regmap_read, __ret, __ret || (cond), \
			sleep_us, timeout_us, false, (map), (addr), &(val)); \
	__ret ?: __tmp; \
})


#define regmap_read_poll_timeout_atomic(map, addr, val, cond, delay_us, timeout_us) \
({ \
	u64 __timeout_us = (timeout_us); \
	unsigned long __delay_us = (delay_us); \
	ktime_t __timeout = ktime_add_us(ktime_get(), __timeout_us); \
	int __ret; \
	for (;;) { \
		__ret = regmap_read((map), (addr), &(val)); \
		if (__ret) \
			break; \
		if (cond) \
			break; \
		if ((__timeout_us) && \
		    ktime_compare(ktime_get(), __timeout) > 0) { \
			__ret = regmap_read((map), (addr), &(val)); \
			break; \
		} \
		if (__delay_us) \
			udelay(__delay_us); \
	} \
	__ret ?: ((cond) ? 0 : -ETIMEDOUT); \
})


#define regmap_field_read_poll_timeout(field, val, cond, sleep_us, timeout_us) \
({ \
	int __ret, __tmp; \
	__tmp = read_poll_timeout(regmap_field_read, __ret, __ret || (cond), \
			sleep_us, timeout_us, false, (field), &(val)); \
	__ret ?: __tmp; \
})

#ifdef CONFIG_REGMAP

enum regmap_endian {
	
	REGMAP_ENDIAN_DEFAULT = 0,
	REGMAP_ENDIAN_BIG,
	REGMAP_ENDIAN_LITTLE,
	REGMAP_ENDIAN_NATIVE,
};


struct regmap_range {
	unsigned int range_min;
	unsigned int range_max;
};

#define regmap_reg_range(low, high) { .range_min = low, .range_max = high, }


struct regmap_access_table {
	const struct regmap_range *yes_ranges;
	unsigned int n_yes_ranges;
	const struct regmap_range *no_ranges;
	unsigned int n_no_ranges;
};

typedef void (*regmap_lock)(void *);
typedef void (*regmap_unlock)(void *);


struct regmap_config {
	const char *name;

	int reg_bits;
	int reg_stride;
	int reg_shift;
	unsigned int reg_base;
	int pad_bits;
	int val_bits;

	bool (*writeable_reg)(struct device *dev, unsigned int reg);
	bool (*readable_reg)(struct device *dev, unsigned int reg);
	bool (*volatile_reg)(struct device *dev, unsigned int reg);
	bool (*precious_reg)(struct device *dev, unsigned int reg);
	bool (*writeable_noinc_reg)(struct device *dev, unsigned int reg);
	bool (*readable_noinc_reg)(struct device *dev, unsigned int reg);

	bool disable_locking;
	regmap_lock lock;
	regmap_unlock unlock;
	void *lock_arg;

	int (*reg_read)(void *context, unsigned int reg, unsigned int *val);
	int (*reg_write)(void *context, unsigned int reg, unsigned int val);
	int (*reg_update_bits)(void *context, unsigned int reg,
			       unsigned int mask, unsigned int val);
	
	int (*read)(void *context, const void *reg_buf, size_t reg_size,
		    void *val_buf, size_t val_size);
	int (*write)(void *context, const void *data, size_t count);
	size_t max_raw_read;
	size_t max_raw_write;

	bool fast_io;
	bool io_port;

	unsigned int max_register;
	const struct regmap_access_table *wr_table;
	const struct regmap_access_table *rd_table;
	const struct regmap_access_table *volatile_table;
	const struct regmap_access_table *precious_table;
	const struct regmap_access_table *wr_noinc_table;
	const struct regmap_access_table *rd_noinc_table;
	const struct reg_default *reg_defaults;
	unsigned int num_reg_defaults;
	enum regcache_type cache_type;
	const void *reg_defaults_raw;
	unsigned int num_reg_defaults_raw;

	unsigned long read_flag_mask;
	unsigned long write_flag_mask;
	bool zero_flag_mask;

	bool use_single_read;
	bool use_single_write;
	bool use_relaxed_mmio;
	bool can_multi_write;

	enum regmap_endian reg_format_endian;
	enum regmap_endian val_format_endian;

	const struct regmap_range_cfg *ranges;
	unsigned int num_ranges;

	bool use_hwlock;
	bool use_raw_spinlock;
	unsigned int hwlock_id;
	unsigned int hwlock_mode;

	bool can_sleep;
};


struct regmap_range_cfg {
	const char *name;

	
	unsigned int range_min;
	unsigned int range_max;

	
	unsigned int selector_reg;
	unsigned int selector_mask;
	int selector_shift;

	
	unsigned int window_start;
	unsigned int window_len;
};

struct regmap_async;

typedef int (*regmap_hw_write)(void *context, const void *data,
			       size_t count);
typedef int (*regmap_hw_gather_write)(void *context,
				      const void *reg, size_t reg_len,
				      const void *val, size_t val_len);
typedef int (*regmap_hw_async_write)(void *context,
				     const void *reg, size_t reg_len,
				     const void *val, size_t val_len,
				     struct regmap_async *async);
typedef int (*regmap_hw_read)(void *context,
			      const void *reg_buf, size_t reg_size,
			      void *val_buf, size_t val_size);
typedef int (*regmap_hw_reg_read)(void *context, unsigned int reg,
				  unsigned int *val);
typedef int (*regmap_hw_reg_noinc_read)(void *context, unsigned int reg,
					void *val, size_t val_count);
typedef int (*regmap_hw_reg_write)(void *context, unsigned int reg,
				   unsigned int val);
typedef int (*regmap_hw_reg_noinc_write)(void *context, unsigned int reg,
					 const void *val, size_t val_count);
typedef int (*regmap_hw_reg_update_bits)(void *context, unsigned int reg,
					 unsigned int mask, unsigned int val);
typedef struct regmap_async *(*regmap_hw_async_alloc)(void);
typedef void (*regmap_hw_free_context)(void *context);


struct regmap_bus {
	bool fast_io;
	bool free_on_exit;
	regmap_hw_write write;
	regmap_hw_gather_write gather_write;
	regmap_hw_async_write async_write;
	regmap_hw_reg_write reg_write;
	regmap_hw_reg_noinc_write reg_noinc_write;
	regmap_hw_reg_update_bits reg_update_bits;
	regmap_hw_read read;
	regmap_hw_reg_read reg_read;
	regmap_hw_reg_noinc_read reg_noinc_read;
	regmap_hw_free_context free_context;
	regmap_hw_async_alloc async_alloc;
	u8 read_flag_mask;
	enum regmap_endian reg_format_endian_default;
	enum regmap_endian val_format_endian_default;
	size_t max_raw_read;
	size_t max_raw_write;
};


struct regmap *__regmap_init(struct device *dev,
			     const struct regmap_bus *bus,
			     void *bus_context,
			     const struct regmap_config *config,
			     struct lock_class_key *lock_key,
			     const char *lock_name);
struct regmap *__regmap_init_i2c(struct i2c_client *i2c,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_mdio(struct mdio_device *mdio_dev,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_sccb(struct i2c_client *i2c,
				  const struct regmap_config *config,
				  struct lock_class_key *lock_key,
				  const char *lock_name);
struct regmap *__regmap_init_slimbus(struct slim_device *slimbus,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_spi(struct spi_device *dev,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_spmi_base(struct spmi_device *dev,
				       const struct regmap_config *config,
				       struct lock_class_key *lock_key,
				       const char *lock_name);
struct regmap *__regmap_init_spmi_ext(struct spmi_device *dev,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__regmap_init_w1(struct device *w1_dev,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_mmio_clk(struct device *dev, const char *clk_id,
				      void __iomem *regs,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__regmap_init_ac97(struct snd_ac97 *ac97,
				  const struct regmap_config *config,
				  struct lock_class_key *lock_key,
				  const char *lock_name);
struct regmap *__regmap_init_sdw(struct sdw_slave *sdw,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__regmap_init_sdw_mbq(struct sdw_slave *sdw,
				     const struct regmap_config *config,
				     struct lock_class_key *lock_key,
				     const char *lock_name);
struct regmap *__regmap_init_spi_avmm(struct spi_device *spi,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__regmap_init_fsi(struct fsi_device *fsi_dev,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);

struct regmap *__devm_regmap_init(struct device *dev,
				  const struct regmap_bus *bus,
				  void *bus_context,
				  const struct regmap_config *config,
				  struct lock_class_key *lock_key,
				  const char *lock_name);
struct regmap *__devm_regmap_init_i2c(struct i2c_client *i2c,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__devm_regmap_init_mdio(struct mdio_device *mdio_dev,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__devm_regmap_init_sccb(struct i2c_client *i2c,
				       const struct regmap_config *config,
				       struct lock_class_key *lock_key,
				       const char *lock_name);
struct regmap *__devm_regmap_init_spi(struct spi_device *dev,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__devm_regmap_init_spmi_base(struct spmi_device *dev,
					    const struct regmap_config *config,
					    struct lock_class_key *lock_key,
					    const char *lock_name);
struct regmap *__devm_regmap_init_spmi_ext(struct spmi_device *dev,
					   const struct regmap_config *config,
					   struct lock_class_key *lock_key,
					   const char *lock_name);
struct regmap *__devm_regmap_init_w1(struct device *w1_dev,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);
struct regmap *__devm_regmap_init_mmio_clk(struct device *dev,
					   const char *clk_id,
					   void __iomem *regs,
					   const struct regmap_config *config,
					   struct lock_class_key *lock_key,
					   const char *lock_name);
struct regmap *__devm_regmap_init_ac97(struct snd_ac97 *ac97,
				       const struct regmap_config *config,
				       struct lock_class_key *lock_key,
				       const char *lock_name);
struct regmap *__devm_regmap_init_sdw(struct sdw_slave *sdw,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__devm_regmap_init_sdw_mbq(struct sdw_slave *sdw,
					  const struct regmap_config *config,
					  struct lock_class_key *lock_key,
					  const char *lock_name);
struct regmap *__devm_regmap_init_slimbus(struct slim_device *slimbus,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__devm_regmap_init_i3c(struct i3c_device *i3c,
				 const struct regmap_config *config,
				 struct lock_class_key *lock_key,
				 const char *lock_name);
struct regmap *__devm_regmap_init_spi_avmm(struct spi_device *spi,
					   const struct regmap_config *config,
					   struct lock_class_key *lock_key,
					   const char *lock_name);
struct regmap *__devm_regmap_init_fsi(struct fsi_device *fsi_dev,
				      const struct regmap_config *config,
				      struct lock_class_key *lock_key,
				      const char *lock_name);


#ifdef CONFIG_LOCKDEP
#define __regmap_lockdep_wrapper(fn, name, ...)				\
(									\
	({								\
		static struct lock_class_key _key;			\
		fn(__VA_ARGS__, &_key,					\
			KBUILD_BASENAME ":"				\
			__stringify(__LINE__) ":"			\
			"(" name ")->lock");				\
	})								\
)
#else
#define __regmap_lockdep_wrapper(fn, name, ...) fn(__VA_ARGS__, NULL, NULL)
#endif


#define regmap_init(dev, bus, bus_context, config)			\
	__regmap_lockdep_wrapper(__regmap_init, #config,		\
				dev, bus, bus_context, config)
int regmap_attach_dev(struct device *dev, struct regmap *map,
		      const struct regmap_config *config);


#define regmap_init_i2c(i2c, config)					\
	__regmap_lockdep_wrapper(__regmap_init_i2c, #config,		\
				i2c, config)


#define regmap_init_mdio(mdio_dev, config)				\
	__regmap_lockdep_wrapper(__regmap_init_mdio, #config,		\
				mdio_dev, config)


#define regmap_init_sccb(i2c, config)					\
	__regmap_lockdep_wrapper(__regmap_init_sccb, #config,		\
				i2c, config)


#define regmap_init_slimbus(slimbus, config)				\
	__regmap_lockdep_wrapper(__regmap_init_slimbus, #config,	\
				slimbus, config)


#define regmap_init_spi(dev, config)					\
	__regmap_lockdep_wrapper(__regmap_init_spi, #config,		\
				dev, config)


#define regmap_init_spmi_base(dev, config)				\
	__regmap_lockdep_wrapper(__regmap_init_spmi_base, #config,	\
				dev, config)


#define regmap_init_spmi_ext(dev, config)				\
	__regmap_lockdep_wrapper(__regmap_init_spmi_ext, #config,	\
				dev, config)


#define regmap_init_w1(w1_dev, config)					\
	__regmap_lockdep_wrapper(__regmap_init_w1, #config,		\
				w1_dev, config)


#define regmap_init_mmio_clk(dev, clk_id, regs, config)			\
	__regmap_lockdep_wrapper(__regmap_init_mmio_clk, #config,	\
				dev, clk_id, regs, config)


#define regmap_init_mmio(dev, regs, config)		\
	regmap_init_mmio_clk(dev, NULL, regs, config)


#define regmap_init_ac97(ac97, config)					\
	__regmap_lockdep_wrapper(__regmap_init_ac97, #config,		\
				ac97, config)
bool regmap_ac97_default_volatile(struct device *dev, unsigned int reg);


#define regmap_init_sdw(sdw, config)					\
	__regmap_lockdep_wrapper(__regmap_init_sdw, #config,		\
				sdw, config)


#define regmap_init_sdw_mbq(sdw, config)					\
	__regmap_lockdep_wrapper(__regmap_init_sdw_mbq, #config,		\
				sdw, config)


#define regmap_init_spi_avmm(spi, config)					\
	__regmap_lockdep_wrapper(__regmap_init_spi_avmm, #config,		\
				 spi, config)


#define regmap_init_fsi(fsi_dev, config)				\
	__regmap_lockdep_wrapper(__regmap_init_fsi, #config, fsi_dev,	\
				 config)


#define devm_regmap_init(dev, bus, bus_context, config)			\
	__regmap_lockdep_wrapper(__devm_regmap_init, #config,		\
				dev, bus, bus_context, config)


#define devm_regmap_init_i2c(i2c, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_i2c, #config,	\
				i2c, config)


#define devm_regmap_init_mdio(mdio_dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_mdio, #config,	\
				mdio_dev, config)


#define devm_regmap_init_sccb(i2c, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_sccb, #config,	\
				i2c, config)


#define devm_regmap_init_spi(dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_spi, #config,	\
				dev, config)


#define devm_regmap_init_spmi_base(dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_spmi_base, #config,	\
				dev, config)


#define devm_regmap_init_spmi_ext(dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_spmi_ext, #config,	\
				dev, config)


#define devm_regmap_init_w1(w1_dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_w1, #config,	\
				w1_dev, config)

#define devm_regmap_init_mmio_clk(dev, clk_id, regs, config)		\
	__regmap_lockdep_wrapper(__devm_regmap_init_mmio_clk, #config,	\
				dev, clk_id, regs, config)


#define devm_regmap_init_mmio(dev, regs, config)		\
	devm_regmap_init_mmio_clk(dev, NULL, regs, config)


#define devm_regmap_init_ac97(ac97, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_ac97, #config,	\
				ac97, config)


#define devm_regmap_init_sdw(sdw, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_sdw, #config,	\
				sdw, config)


#define devm_regmap_init_sdw_mbq(sdw, config)			\
	__regmap_lockdep_wrapper(__devm_regmap_init_sdw_mbq, #config,   \
				sdw, config)


#define devm_regmap_init_slimbus(slimbus, config)			\
	__regmap_lockdep_wrapper(__devm_regmap_init_slimbus, #config,	\
				slimbus, config)


#define devm_regmap_init_i3c(i3c, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_i3c, #config,	\
				i3c, config)


#define devm_regmap_init_spi_avmm(spi, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_spi_avmm, #config,	\
				 spi, config)


#define devm_regmap_init_fsi(fsi_dev, config)				\
	__regmap_lockdep_wrapper(__devm_regmap_init_fsi, #config,	\
				 fsi_dev, config)

int regmap_mmio_attach_clk(struct regmap *map, struct clk *clk);
void regmap_mmio_detach_clk(struct regmap *map);
void regmap_exit(struct regmap *map);
int regmap_reinit_cache(struct regmap *map,
			const struct regmap_config *config);
struct regmap *dev_get_regmap(struct device *dev, const char *name);
struct device *regmap_get_device(struct regmap *map);
int regmap_write(struct regmap *map, unsigned int reg, unsigned int val);
int regmap_write_async(struct regmap *map, unsigned int reg, unsigned int val);
int regmap_raw_write(struct regmap *map, unsigned int reg,
		     const void *val, size_t val_len);
int regmap_noinc_write(struct regmap *map, unsigned int reg,
		     const void *val, size_t val_len);
int regmap_bulk_write(struct regmap *map, unsigned int reg, const void *val,
			size_t val_count);
int regmap_multi_reg_write(struct regmap *map, const struct reg_sequence *regs,
			int num_regs);
int regmap_multi_reg_write_bypassed(struct regmap *map,
				    const struct reg_sequence *regs,
				    int num_regs);
int regmap_raw_write_async(struct regmap *map, unsigned int reg,
			   const void *val, size_t val_len);
int regmap_read(struct regmap *map, unsigned int reg, unsigned int *val);
int regmap_read_bypassed(struct regmap *map, unsigned int reg, unsigned int *val);
int regmap_raw_read(struct regmap *map, unsigned int reg,
		    void *val, size_t val_len);
int regmap_noinc_read(struct regmap *map, unsigned int reg,
		      void *val, size_t val_len);
int regmap_bulk_read(struct regmap *map, unsigned int reg, void *val,
		     size_t val_count);
int regmap_update_bits_base(struct regmap *map, unsigned int reg,
			    unsigned int mask, unsigned int val,
			    bool *change, bool async, bool force);

static inline int regmap_update_bits(struct regmap *map, unsigned int reg,
				     unsigned int mask, unsigned int val)
{
	return regmap_update_bits_base(map, reg, mask, val, NULL, false, false);
}

static inline int regmap_update_bits_async(struct regmap *map, unsigned int reg,
					   unsigned int mask, unsigned int val)
{
	return regmap_update_bits_base(map, reg, mask, val, NULL, true, false);
}

static inline int regmap_update_bits_check(struct regmap *map, unsigned int reg,
					   unsigned int mask, unsigned int val,
					   bool *change)
{
	return regmap_update_bits_base(map, reg, mask, val,
				       change, false, false);
}

static inline int
regmap_update_bits_check_async(struct regmap *map, unsigned int reg,
			       unsigned int mask, unsigned int val,
			       bool *change)
{
	return regmap_update_bits_base(map, reg, mask, val,
				       change, true, false);
}

static inline int regmap_write_bits(struct regmap *map, unsigned int reg,
				    unsigned int mask, unsigned int val)
{
	return regmap_update_bits_base(map, reg, mask, val, NULL, false, true);
}

int regmap_get_val_bytes(struct regmap *map);
int regmap_get_max_register(struct regmap *map);
int regmap_get_reg_stride(struct regmap *map);
bool regmap_might_sleep(struct regmap *map);
int regmap_async_complete(struct regmap *map);
bool regmap_can_raw_write(struct regmap *map);
size_t regmap_get_raw_read_max(struct regmap *map);
size_t regmap_get_raw_write_max(struct regmap *map);

int regcache_sync(struct regmap *map);
int regcache_sync_region(struct regmap *map, unsigned int min,
			 unsigned int max);
int regcache_drop_region(struct regmap *map, unsigned int min,
			 unsigned int max);
void regcache_cache_only(struct regmap *map, bool enable);
void regcache_cache_bypass(struct regmap *map, bool enable);
void regcache_mark_dirty(struct regmap *map);
bool regcache_reg_cached(struct regmap *map, unsigned int reg);

bool regmap_check_range_table(struct regmap *map, unsigned int reg,
			      const struct regmap_access_table *table);

int regmap_register_patch(struct regmap *map, const struct reg_sequence *regs,
			  int num_regs);
int regmap_parse_val(struct regmap *map, const void *buf,
				unsigned int *val);

static inline bool regmap_reg_in_range(unsigned int reg,
				       const struct regmap_range *range)
{
	return reg >= range->range_min && reg <= range->range_max;
}

bool regmap_reg_in_ranges(unsigned int reg,
			  const struct regmap_range *ranges,
			  unsigned int nranges);

static inline int regmap_set_bits(struct regmap *map,
				  unsigned int reg, unsigned int bits)
{
	return regmap_update_bits_base(map, reg, bits, bits,
				       NULL, false, false);
}

static inline int regmap_clear_bits(struct regmap *map,
				    unsigned int reg, unsigned int bits)
{
	return regmap_update_bits_base(map, reg, bits, 0, NULL, false, false);
}

int regmap_test_bits(struct regmap *map, unsigned int reg, unsigned int bits);


struct reg_field {
	unsigned int reg;
	unsigned int lsb;
	unsigned int msb;
	unsigned int id_size;
	unsigned int id_offset;
};

#define REG_FIELD(_reg, _lsb, _msb) {		\
				.reg = _reg,	\
				.lsb = _lsb,	\
				.msb = _msb,	\
				}

#define REG_FIELD_ID(_reg, _lsb, _msb, _size, _offset) {	\
				.reg = _reg,			\
				.lsb = _lsb,			\
				.msb = _msb,			\
				.id_size = _size,		\
				.id_offset = _offset,		\
				}

struct regmap_field *regmap_field_alloc(struct regmap *regmap,
		struct reg_field reg_field);
void regmap_field_free(struct regmap_field *field);

struct regmap_field *devm_regmap_field_alloc(struct device *dev,
		struct regmap *regmap, struct reg_field reg_field);
void devm_regmap_field_free(struct device *dev,	struct regmap_field *field);

int regmap_field_bulk_alloc(struct regmap *regmap,
			     struct regmap_field **rm_field,
			     const struct reg_field *reg_field,
			     int num_fields);
void regmap_field_bulk_free(struct regmap_field *field);
int devm_regmap_field_bulk_alloc(struct device *dev, struct regmap *regmap,
				 struct regmap_field **field,
				 const struct reg_field *reg_field,
				 int num_fields);
void devm_regmap_field_bulk_free(struct device *dev,
				 struct regmap_field *field);

int regmap_field_read(struct regmap_field *field, unsigned int *val);
int regmap_field_update_bits_base(struct regmap_field *field,
				  unsigned int mask, unsigned int val,
				  bool *change, bool async, bool force);
int regmap_fields_read(struct regmap_field *field, unsigned int id,
		       unsigned int *val);
int regmap_fields_update_bits_base(struct regmap_field *field,  unsigned int id,
				   unsigned int mask, unsigned int val,
				   bool *change, bool async, bool force);

static inline int regmap_field_write(struct regmap_field *field,
				     unsigned int val)
{
	return regmap_field_update_bits_base(field, ~0, val,
					     NULL, false, false);
}

static inline int regmap_field_force_write(struct regmap_field *field,
					   unsigned int val)
{
	return regmap_field_update_bits_base(field, ~0, val, NULL, false, true);
}

static inline int regmap_field_update_bits(struct regmap_field *field,
					   unsigned int mask, unsigned int val)
{
	return regmap_field_update_bits_base(field, mask, val,
					     NULL, false, false);
}

static inline int regmap_field_set_bits(struct regmap_field *field,
					unsigned int bits)
{
	return regmap_field_update_bits_base(field, bits, bits, NULL, false,
					     false);
}

static inline int regmap_field_clear_bits(struct regmap_field *field,
					  unsigned int bits)
{
	return regmap_field_update_bits_base(field, bits, 0, NULL, false,
					     false);
}

int regmap_field_test_bits(struct regmap_field *field, unsigned int bits);

static inline int
regmap_field_force_update_bits(struct regmap_field *field,
			       unsigned int mask, unsigned int val)
{
	return regmap_field_update_bits_base(field, mask, val,
					     NULL, false, true);
}

static inline int regmap_fields_write(struct regmap_field *field,
				      unsigned int id, unsigned int val)
{
	return regmap_fields_update_bits_base(field, id, ~0, val,
					      NULL, false, false);
}

static inline int regmap_fields_force_write(struct regmap_field *field,
					    unsigned int id, unsigned int val)
{
	return regmap_fields_update_bits_base(field, id, ~0, val,
					      NULL, false, true);
}

static inline int
regmap_fields_update_bits(struct regmap_field *field, unsigned int id,
			  unsigned int mask, unsigned int val)
{
	return regmap_fields_update_bits_base(field, id, mask, val,
					      NULL, false, false);
}

static inline int
regmap_fields_force_update_bits(struct regmap_field *field, unsigned int id,
				unsigned int mask, unsigned int val)
{
	return regmap_fields_update_bits_base(field, id, mask, val,
					      NULL, false, true);
}


struct regmap_irq_type {
	unsigned int type_reg_offset;
	unsigned int type_reg_mask;
	unsigned int type_rising_val;
	unsigned int type_falling_val;
	unsigned int type_level_low_val;
	unsigned int type_level_high_val;
	unsigned int types_supported;
};


struct regmap_irq {
	unsigned int reg_offset;
	unsigned int mask;
	struct regmap_irq_type type;
};

#define REGMAP_IRQ_REG(_irq, _off, _mask)		\
	[_irq] = { .reg_offset = (_off), .mask = (_mask) }

#define REGMAP_IRQ_REG_LINE(_id, _reg_bits) \
	[_id] = {				\
		.mask = BIT((_id) % (_reg_bits)),	\
		.reg_offset = (_id) / (_reg_bits),	\
	}

#define REGMAP_IRQ_MAIN_REG_OFFSET(arr)				\
	{ .num_regs = ARRAY_SIZE((arr)), .offset = &(arr)[0] }

struct regmap_irq_sub_irq_map {
	unsigned int num_regs;
	unsigned int *offset;
};

struct regmap_irq_chip_data;


struct regmap_irq_chip {
	const char *name;

	unsigned int main_status;
	unsigned int num_main_status_bits;
	struct regmap_irq_sub_irq_map *sub_reg_offsets;
	int num_main_regs;

	unsigned int status_base;
	unsigned int mask_base;
	unsigned int unmask_base;
	unsigned int ack_base;
	unsigned int wake_base;
	const unsigned int *config_base;
	unsigned int irq_reg_stride;
	unsigned int init_ack_masked:1;
	unsigned int mask_unmask_non_inverted:1;
	unsigned int use_ack:1;
	unsigned int ack_invert:1;
	unsigned int clear_ack:1;
	unsigned int status_invert:1;
	unsigned int wake_invert:1;
	unsigned int type_in_mask:1;
	unsigned int clear_on_unmask:1;
	unsigned int runtime_pm:1;
	unsigned int no_status:1;

	int num_regs;

	const struct regmap_irq *irqs;
	int num_irqs;

	int num_config_bases;
	int num_config_regs;

	int (*handle_pre_irq)(void *irq_drv_data);
	int (*handle_post_irq)(void *irq_drv_data);
	int (*handle_mask_sync)(int index, unsigned int mask_buf_def,
				unsigned int mask_buf, void *irq_drv_data);
	int (*set_type_config)(unsigned int **buf, unsigned int type,
			       const struct regmap_irq *irq_data, int idx,
			       void *irq_drv_data);
	unsigned int (*get_irq_reg)(struct regmap_irq_chip_data *data,
				    unsigned int base, int index);
	void *irq_drv_data;
};

unsigned int regmap_irq_get_irq_reg_linear(struct regmap_irq_chip_data *data,
					   unsigned int base, int index);
int regmap_irq_set_type_config_simple(unsigned int **buf, unsigned int type,
				      const struct regmap_irq *irq_data,
				      int idx, void *irq_drv_data);

int regmap_add_irq_chip(struct regmap *map, int irq, int irq_flags,
			int irq_base, const struct regmap_irq_chip *chip,
			struct regmap_irq_chip_data **data);
int regmap_add_irq_chip_fwnode(struct fwnode_handle *fwnode,
			       struct regmap *map, int irq,
			       int irq_flags, int irq_base,
			       const struct regmap_irq_chip *chip,
			       struct regmap_irq_chip_data **data);
void regmap_del_irq_chip(int irq, struct regmap_irq_chip_data *data);

int devm_regmap_add_irq_chip(struct device *dev, struct regmap *map, int irq,
			     int irq_flags, int irq_base,
			     const struct regmap_irq_chip *chip,
			     struct regmap_irq_chip_data **data);
int devm_regmap_add_irq_chip_fwnode(struct device *dev,
				    struct fwnode_handle *fwnode,
				    struct regmap *map, int irq,
				    int irq_flags, int irq_base,
				    const struct regmap_irq_chip *chip,
				    struct regmap_irq_chip_data **data);
void devm_regmap_del_irq_chip(struct device *dev, int irq,
			      struct regmap_irq_chip_data *data);

int regmap_irq_chip_get_base(struct regmap_irq_chip_data *data);
int regmap_irq_get_virq(struct regmap_irq_chip_data *data, int irq);
struct irq_domain *regmap_irq_get_domain(struct regmap_irq_chip_data *data);

#else



static inline int regmap_write(struct regmap *map, unsigned int reg,
			       unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_write_async(struct regmap *map, unsigned int reg,
				     unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_raw_write(struct regmap *map, unsigned int reg,
				   const void *val, size_t val_len)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_raw_write_async(struct regmap *map, unsigned int reg,
					 const void *val, size_t val_len)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_noinc_write(struct regmap *map, unsigned int reg,
				    const void *val, size_t val_len)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_bulk_write(struct regmap *map, unsigned int reg,
				    const void *val, size_t val_count)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_read(struct regmap *map, unsigned int reg,
			      unsigned int *val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_read_bypassed(struct regmap *map, unsigned int reg,
				       unsigned int *val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_raw_read(struct regmap *map, unsigned int reg,
				  void *val, size_t val_len)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_noinc_read(struct regmap *map, unsigned int reg,
				    void *val, size_t val_len)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_bulk_read(struct regmap *map, unsigned int reg,
				   void *val, size_t val_count)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_update_bits_base(struct regmap *map, unsigned int reg,
					  unsigned int mask, unsigned int val,
					  bool *change, bool async, bool force)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_set_bits(struct regmap *map,
				  unsigned int reg, unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_clear_bits(struct regmap *map,
				    unsigned int reg, unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_test_bits(struct regmap *map,
				   unsigned int reg, unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_update_bits_base(struct regmap_field *field,
					unsigned int mask, unsigned int val,
					bool *change, bool async, bool force)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_fields_update_bits_base(struct regmap_field *field,
				   unsigned int id,
				   unsigned int mask, unsigned int val,
				   bool *change, bool async, bool force)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_update_bits(struct regmap *map, unsigned int reg,
				     unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_update_bits_async(struct regmap *map, unsigned int reg,
					   unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_update_bits_check(struct regmap *map, unsigned int reg,
					   unsigned int mask, unsigned int val,
					   bool *change)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int
regmap_update_bits_check_async(struct regmap *map, unsigned int reg,
			       unsigned int mask, unsigned int val,
			       bool *change)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_write_bits(struct regmap *map, unsigned int reg,
				    unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_write(struct regmap_field *field,
				     unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_force_write(struct regmap_field *field,
					   unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_update_bits(struct regmap_field *field,
					   unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int
regmap_field_force_update_bits(struct regmap_field *field,
			       unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_set_bits(struct regmap_field *field,
					unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_clear_bits(struct regmap_field *field,
					  unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_field_test_bits(struct regmap_field *field,
					 unsigned int bits)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_fields_write(struct regmap_field *field,
				      unsigned int id, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_fields_force_write(struct regmap_field *field,
					    unsigned int id, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int
regmap_fields_update_bits(struct regmap_field *field, unsigned int id,
			  unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int
regmap_fields_force_update_bits(struct regmap_field *field, unsigned int id,
				unsigned int mask, unsigned int val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_get_val_bytes(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_get_max_register(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_get_reg_stride(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline bool regmap_might_sleep(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return true;
}

static inline int regcache_sync(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regcache_sync_region(struct regmap *map, unsigned int min,
				       unsigned int max)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regcache_drop_region(struct regmap *map, unsigned int min,
				       unsigned int max)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline void regcache_cache_only(struct regmap *map, bool enable)
{
	WARN_ONCE(1, "regmap API is disabled");
}

static inline void regcache_cache_bypass(struct regmap *map, bool enable)
{
	WARN_ONCE(1, "regmap API is disabled");
}

static inline void regcache_mark_dirty(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
}

static inline void regmap_async_complete(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
}

static inline int regmap_register_patch(struct regmap *map,
					const struct reg_sequence *regs,
					int num_regs)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline int regmap_parse_val(struct regmap *map, const void *buf,
				unsigned int *val)
{
	WARN_ONCE(1, "regmap API is disabled");
	return -EINVAL;
}

static inline struct regmap *dev_get_regmap(struct device *dev,
					    const char *name)
{
	return NULL;
}

static inline struct device *regmap_get_device(struct regmap *map)
{
	WARN_ONCE(1, "regmap API is disabled");
	return NULL;
}

#endif

#endif
