/* SPDX-License-Identifier: GPL-2.0-only */

#ifdef __KERNEL__

#ifdef __STDC__
#define ____glue(name,fn)	name##fn
#else
#define ____glue(name,fn)	namefn
#endif
#define __glue(name,fn)		____glue(name,fn)

#endif
