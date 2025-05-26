/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_TOSHIBA_H
#define _LINUX_TOSHIBA_H

#include <uapi/linux/toshiba.h>

int tosh_smm(SMMRegisters *regs);
#endif
