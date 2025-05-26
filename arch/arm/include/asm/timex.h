/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ASMARM_TIMEX_H
#define _ASMARM_TIMEX_H

typedef unsigned long cycles_t;
#define get_cycles()	({ cycles_t c; read_current_timer(&c) ? 0 : c; })
#define random_get_entropy() (((unsigned long)get_cycles()) ?: random_get_entropy_fallback())

#endif
