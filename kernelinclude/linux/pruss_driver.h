/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _PRUSS_DRIVER_H_
#define _PRUSS_DRIVER_H_

#include <linux/mutex.h>
#include <linux/remoteproc/pruss.h>
#include <linux/types.h>
#include <linux/err.h>


enum pruss_gp_mux_sel {
	PRUSS_GP_MUX_SEL_GP,
	PRUSS_GP_MUX_SEL_ENDAT,
	PRUSS_GP_MUX_SEL_RESERVED,
	PRUSS_GP_MUX_SEL_SD,
	PRUSS_GP_MUX_SEL_MII2,
	PRUSS_GP_MUX_SEL_MAX,
};


enum pruss_gpi_mode {
	PRUSS_GPI_MODE_DIRECT,
	PRUSS_GPI_MODE_PARALLEL,
	PRUSS_GPI_MODE_28BIT_SHIFT,
	PRUSS_GPI_MODE_MII,
	PRUSS_GPI_MODE_MAX,
};


enum pru_type {
	PRU_TYPE_PRU,
	PRU_TYPE_RTU,
	PRU_TYPE_TX_PRU,
	PRU_TYPE_MAX,
};


enum pruss_mem {
	PRUSS_MEM_DRAM0 = 0,
	PRUSS_MEM_DRAM1,
	PRUSS_MEM_SHRD_RAM2,
	PRUSS_MEM_MAX,
};


struct pruss_mem_region {
	void __iomem *va;
	phys_addr_t pa;
	size_t size;
};


struct pruss {
	struct device *dev;
	void __iomem *cfg_base;
	struct regmap *cfg_regmap;
	struct pruss_mem_region mem_regions[PRUSS_MEM_MAX];
	struct pruss_mem_region *mem_in_use[PRUSS_MEM_MAX];
	struct mutex lock; 
	struct clk *core_clk_mux;
	struct clk *iep_clk_mux;
};

#if IS_ENABLED(CONFIG_TI_PRUSS)

struct pruss *pruss_get(struct rproc *rproc);
void pruss_put(struct pruss *pruss);
int pruss_request_mem_region(struct pruss *pruss, enum pruss_mem mem_id,
			     struct pruss_mem_region *region);
int pruss_release_mem_region(struct pruss *pruss,
			     struct pruss_mem_region *region);
int pruss_cfg_get_gpmux(struct pruss *pruss, enum pruss_pru_id pru_id, u8 *mux);
int pruss_cfg_set_gpmux(struct pruss *pruss, enum pruss_pru_id pru_id, u8 mux);
int pruss_cfg_gpimode(struct pruss *pruss, enum pruss_pru_id pru_id,
		      enum pruss_gpi_mode mode);
int pruss_cfg_miirt_enable(struct pruss *pruss, bool enable);
int pruss_cfg_xfr_enable(struct pruss *pruss, enum pru_type pru_type,
			 bool enable);

#else

static inline struct pruss *pruss_get(struct rproc *rproc)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline void pruss_put(struct pruss *pruss) { }

static inline int pruss_request_mem_region(struct pruss *pruss,
					   enum pruss_mem mem_id,
					   struct pruss_mem_region *region)
{
	return -EOPNOTSUPP;
}

static inline int pruss_release_mem_region(struct pruss *pruss,
					   struct pruss_mem_region *region)
{
	return -EOPNOTSUPP;
}

static inline int pruss_cfg_get_gpmux(struct pruss *pruss,
				      enum pruss_pru_id pru_id, u8 *mux)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int pruss_cfg_set_gpmux(struct pruss *pruss,
				      enum pruss_pru_id pru_id, u8 mux)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int pruss_cfg_gpimode(struct pruss *pruss,
				    enum pruss_pru_id pru_id,
				    enum pruss_gpi_mode mode)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int pruss_cfg_miirt_enable(struct pruss *pruss, bool enable)
{
	return ERR_PTR(-EOPNOTSUPP);
}

static inline int pruss_cfg_xfr_enable(struct pruss *pruss,
				       enum pru_type pru_type,
				       bool enable);
{
	return ERR_PTR(-EOPNOTSUPP);
}

#endif 

#endif	
