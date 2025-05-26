/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __LINUX_PLATFORM_DATA_MLXREG_H
#define __LINUX_PLATFORM_DATA_MLXREG_H

#define MLXREG_CORE_LABEL_MAX_SIZE	32
#define MLXREG_CORE_WD_FEATURE_NOWAYOUT		BIT(0)
#define MLXREG_CORE_WD_FEATURE_START_AT_BOOT	BIT(1)


enum mlxreg_wdt_type {
	MLX_WDT_TYPE1,
	MLX_WDT_TYPE2,
	MLX_WDT_TYPE3,
};


enum mlxreg_hotplug_kind {
	MLXREG_HOTPLUG_DEVICE_NA = 0,
	MLXREG_HOTPLUG_LC_PRESENT = 1,
	MLXREG_HOTPLUG_LC_VERIFIED = 2,
	MLXREG_HOTPLUG_LC_POWERED = 3,
	MLXREG_HOTPLUG_LC_SYNCED = 4,
	MLXREG_HOTPLUG_LC_READY = 5,
	MLXREG_HOTPLUG_LC_ACTIVE = 6,
	MLXREG_HOTPLUG_LC_THERMAL = 7,
};


enum mlxreg_hotplug_device_action {
	MLXREG_HOTPLUG_DEVICE_DEFAULT_ACTION = 0,
	MLXREG_HOTPLUG_DEVICE_PLATFORM_ACTION = 1,
	MLXREG_HOTPLUG_DEVICE_NO_ACTION = 2,
};


struct mlxreg_core_hotplug_notifier {
	char identity[MLXREG_CORE_LABEL_MAX_SIZE];
	void *handle;
	int (*user_handler)(void *handle, enum mlxreg_hotplug_kind kind, u8 action);
};


struct mlxreg_hotplug_device {
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	struct i2c_board_info *brdinfo;
	int nr;
	struct platform_device *pdev;
	enum mlxreg_hotplug_device_action action;
	void *handle;
	int (*user_handler)(void *handle, enum mlxreg_hotplug_kind kind, u8 action);
	struct mlxreg_core_hotplug_notifier *notifier;
};


struct mlxreg_core_data {
	char label[MLXREG_CORE_LABEL_MAX_SIZE];
	u32 reg;
	u32 mask;
	u32 bit;
	u32 capability;
	u32 reg_prsnt;
	u32 reg_sync;
	u32 reg_pwr;
	u32 reg_ena;
	umode_t	mode;
	struct device_node *np;
	struct mlxreg_hotplug_device hpdev;
	struct mlxreg_core_hotplug_notifier *notifier;
	u32 health_cntr;
	bool attached;
	u8 regnum;
	u8 slot;
	u8 secured;
};


struct mlxreg_core_item {
	struct mlxreg_core_data *data;
	enum mlxreg_hotplug_kind kind;
	u32 aggr_mask;
	u32 reg;
	u32 mask;
	u32 capability;
	u32 cache;
	u8 count;
	u8 ind;
	u8 inversed;
	u8 health;
};


struct mlxreg_core_platform_data {
	struct mlxreg_core_data *data;
	void *regmap;
	int counter;
	u32 features;
	u32 version;
	char identity[MLXREG_CORE_LABEL_MAX_SIZE];
	u32 capability;
};


struct mlxreg_core_hotplug_platform_data {
	struct mlxreg_core_item *items;
	int irq;
	void *regmap;
	int counter;
	u32 cell;
	u32 mask;
	u32 cell_low;
	u32 mask_low;
	int deferred_nr;
	int shift_nr;
	void __iomem *addr;
	void *handle;
	int (*completion_notify)(void *handle, int id);
};

#endif 
