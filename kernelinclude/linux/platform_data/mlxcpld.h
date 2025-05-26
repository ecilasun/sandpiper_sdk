/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef _LINUX_I2C_MLXCPLD_H
#define _LINUX_I2C_MLXCPLD_H




struct mlxcpld_mux_plat_data {
	int *chan_ids;
	int num_adaps;
	int sel_reg_addr;
	u8 reg_size;
	void *handle;
	int (*completion_notify)(void *handle, struct i2c_adapter *parent,
				 struct i2c_adapter *adapters[]);
};

#endif 
