/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SOC_PXA_CPU_H
#define __SOC_PXA_CPU_H

#ifdef CONFIG_ARM
#include <asm/cputype.h>
#endif


#ifdef CONFIG_PXA25x
#define __cpu_is_pxa210(id)				\
	({						\
		unsigned int _id = (id) & 0xf3f0;	\
		_id == 0x2120;				\
	})

#define __cpu_is_pxa250(id)				\
	({						\
		unsigned int _id = (id) & 0xf3ff;	\
		_id <= 0x2105;				\
	})

#define __cpu_is_pxa255(id)				\
	({						\
		unsigned int _id = (id) & 0xffff;	\
		_id == 0x2d06;				\
	})

#define __cpu_is_pxa25x(id)				\
	({						\
		unsigned int _id = (id) & 0xf300;	\
		_id == 0x2100;				\
	})
#else
#define __cpu_is_pxa210(id)	(0)
#define __cpu_is_pxa250(id)	(0)
#define __cpu_is_pxa255(id)	(0)
#define __cpu_is_pxa25x(id)	(0)
#endif

#ifdef CONFIG_PXA27x
#define __cpu_is_pxa27x(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x411;				\
	})
#else
#define __cpu_is_pxa27x(id)	(0)
#endif

#ifdef CONFIG_CPU_PXA300
#define __cpu_is_pxa300(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x688;				\
	 })
#else
#define __cpu_is_pxa300(id)	(0)
#endif

#ifdef CONFIG_CPU_PXA310
#define __cpu_is_pxa310(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x689;				\
	 })
#else
#define __cpu_is_pxa310(id)	(0)
#endif

#ifdef CONFIG_CPU_PXA320
#define __cpu_is_pxa320(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x603 || _id == 0x682;		\
	 })
#else
#define __cpu_is_pxa320(id)	(0)
#endif

#ifdef CONFIG_CPU_PXA930
#define __cpu_is_pxa930(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x683;				\
	 })
#else
#define __cpu_is_pxa930(id)	(0)
#endif

#ifdef CONFIG_CPU_PXA935
#define __cpu_is_pxa935(id)				\
	({						\
		unsigned int _id = (id) >> 4 & 0xfff;	\
		_id == 0x693;				\
	 })
#else
#define __cpu_is_pxa935(id)	(0)
#endif

#define cpu_is_pxa210()					\
	({						\
		__cpu_is_pxa210(read_cpuid_id());	\
	})

#define cpu_is_pxa250()					\
	({						\
		__cpu_is_pxa250(read_cpuid_id());	\
	})

#define cpu_is_pxa255()                                 \
	({                                              \
		__cpu_is_pxa255(read_cpuid_id());       \
	})

#define cpu_is_pxa25x()					\
	({						\
		__cpu_is_pxa25x(read_cpuid_id());	\
	})

#define cpu_is_pxa27x()					\
	({						\
		__cpu_is_pxa27x(read_cpuid_id());	\
	})

#define cpu_is_pxa300()					\
	({						\
		__cpu_is_pxa300(read_cpuid_id());	\
	 })

#define cpu_is_pxa310()					\
	({						\
		__cpu_is_pxa310(read_cpuid_id());	\
	 })

#define cpu_is_pxa320()					\
	({						\
		__cpu_is_pxa320(read_cpuid_id());	\
	 })

#define cpu_is_pxa930()					\
	({						\
		__cpu_is_pxa930(read_cpuid_id());	\
	 })

#define cpu_is_pxa935()					\
	({						\
		__cpu_is_pxa935(read_cpuid_id());	\
	 })




#if defined(CONFIG_PXA25x) || defined(CONFIG_PXA27x)
#define __cpu_is_pxa2xx(id)				\
	({						\
		unsigned int _id = (id) >> 13 & 0x7;	\
		_id <= 0x2;				\
	 })
#else
#define __cpu_is_pxa2xx(id)	(0)
#endif

#ifdef CONFIG_PXA3xx
#define __cpu_is_pxa3xx(id)				\
	({						\
		__cpu_is_pxa300(id)			\
			|| __cpu_is_pxa310(id)		\
			|| __cpu_is_pxa320(id)		\
			|| __cpu_is_pxa93x(id);		\
	 })
#else
#define __cpu_is_pxa3xx(id)	(0)
#endif

#if defined(CONFIG_CPU_PXA930) || defined(CONFIG_CPU_PXA935)
#define __cpu_is_pxa93x(id)				\
	({						\
		__cpu_is_pxa930(id)			\
			|| __cpu_is_pxa935(id);		\
	 })
#else
#define __cpu_is_pxa93x(id)	(0)
#endif

#define cpu_is_pxa2xx()					\
	({						\
		__cpu_is_pxa2xx(read_cpuid_id());	\
	 })

#define cpu_is_pxa3xx()					\
	({						\
		__cpu_is_pxa3xx(read_cpuid_id());	\
	 })

#define cpu_is_pxa93x()					\
	({						\
		__cpu_is_pxa93x(read_cpuid_id());	\
	 })

#endif
