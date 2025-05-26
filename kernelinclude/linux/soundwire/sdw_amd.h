/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __SDW_AMD_H
#define __SDW_AMD_H

#include <linux/soundwire/sdw.h>




#define AMD_SDW_CLK_STOP_MODE		1


#define AMD_SDW_POWER_OFF_MODE		2
#define ACP_SDW0	0
#define ACP_SDW1	1

struct acp_sdw_pdata {
	u16 instance;
	
	struct mutex *acp_sdw_lock;
};

struct sdw_manager_reg_mask {
	u32 sw_pad_enable_mask;
	u32 sw_pad_pulldown_mask;
	u32 acp_sdw_intr_mask;
};


struct sdw_amd_dai_runtime {
	char *name;
	struct sdw_stream_runtime *stream;
	struct sdw_bus *bus;
	enum sdw_stream_type stream_type;
};


struct amd_sdw_manager {
	struct sdw_bus bus;
	struct device *dev;

	void __iomem *mmio;
	void __iomem *acp_mmio;

	struct sdw_manager_reg_mask *reg_mask;
	struct work_struct amd_sdw_irq_thread;
	struct work_struct amd_sdw_work;
	struct work_struct probe_work;
	
	struct mutex *acp_sdw_lock;

	enum sdw_slave_status status[SDW_MAX_DEVICES + 1];

	int num_din_ports;
	int num_dout_ports;

	int cols_index;
	int rows_index;

	u32 instance;
	u32 quirks;
	u32 wake_en_mask;
	u32 power_mode_mask;
	bool clk_stopped;

	struct sdw_amd_dai_runtime **dai_runtime_array;
};
#endif
