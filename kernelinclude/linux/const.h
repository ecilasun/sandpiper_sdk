#ifndef _LINUX_CONST_H
#define _LINUX_CONST_H

#include <vdso/const.h>


#define __is_constexpr(x) \
	(sizeof(int) == sizeof(*(8 ? ((void *)((long)(x) * 0l)) : (int *)8)))

#endif 
