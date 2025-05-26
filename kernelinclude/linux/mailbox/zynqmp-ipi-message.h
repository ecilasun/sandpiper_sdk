/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_ZYNQMP_IPI_MESSAGE_H_
#define _LINUX_ZYNQMP_IPI_MESSAGE_H_


struct zynqmp_ipi_message {
	size_t len;
	u8 data[];
};

#endif 
