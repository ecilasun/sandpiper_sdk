

#ifndef _DRM_UTIL_H_
#define _DRM_UTIL_H_



#include <linux/interrupt.h>
#include <linux/kgdb.h>
#include <linux/preempt.h>
#include <linux/smp.h>


#if defined(CONFIG_DRM_EXPORT_FOR_TESTS)
#define EXPORT_SYMBOL_FOR_TESTS_ONLY(x) EXPORT_SYMBOL(x)
#else
#define EXPORT_SYMBOL_FOR_TESTS_ONLY(x)
#endif


#define for_each_if(condition) if (!(condition)) {} else


static inline bool drm_can_sleep(void)
{
	if (in_atomic() || in_dbg_master() || irqs_disabled())
		return false;
	return true;
}

#endif
