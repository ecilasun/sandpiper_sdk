/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACENVEX_H__
#define __ACENVEX_H__





#if defined(_LINUX) || defined(__linux__)
#include <acpi/platform/aclinuxex.h>

#elif defined(__DragonFly__)
#include "acdragonflyex.h"


#elif defined(_AED_EFI) || defined(_GNU_EFI) || defined(_EDK2_EFI)
#include "acefiex.h"

#endif

#if defined(__GNUC__)
#include "acgccex.h"

#elif defined(_MSC_VER)
#include "acmsvcex.h"

#endif



#endif				
