/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SCSI_SCSI_DEVINFO_H
#define _SCSI_SCSI_DEVINFO_H



#define BLIST_NOLUN		((__force blist_flags_t)(1ULL << 0))

#define BLIST_FORCELUN		((__force blist_flags_t)(1ULL << 1))

#define BLIST_BORKEN		((__force blist_flags_t)(1ULL << 2))

#define BLIST_KEY		((__force blist_flags_t)(1ULL << 3))

#define BLIST_SINGLELUN		((__force blist_flags_t)(1ULL << 4))

#define BLIST_NOTQ		((__force blist_flags_t)(1ULL << 5))

#define BLIST_SPARSELUN		((__force blist_flags_t)(1ULL << 6))

#define BLIST_MAX5LUN		((__force blist_flags_t)(1ULL << 7))

#define BLIST_ISROM		((__force blist_flags_t)(1ULL << 8))

#define BLIST_LARGELUN		((__force blist_flags_t)(1ULL << 9))

#define BLIST_INQUIRY_36	((__force blist_flags_t)(1ULL << 10))

#define BLIST_IGN_MEDIA_CHANGE	((__force blist_flags_t)(1ULL << 11))

#define BLIST_NOSTARTONADD	((__force blist_flags_t)(1ULL << 12))

#define BLIST_NO_VPD_SIZE	((__force blist_flags_t)(1ULL << 13))
#define __BLIST_UNUSED_14	((__force blist_flags_t)(1ULL << 14))
#define __BLIST_UNUSED_15	((__force blist_flags_t)(1ULL << 15))
#define __BLIST_UNUSED_16	((__force blist_flags_t)(1ULL << 16))

#define BLIST_REPORTLUN2	((__force blist_flags_t)(1ULL << 17))

#define BLIST_NOREPORTLUN	((__force blist_flags_t)(1ULL << 18))

#define BLIST_NOT_LOCKABLE	((__force blist_flags_t)(1ULL << 19))

#define BLIST_NO_ULD_ATTACH	((__force blist_flags_t)(1ULL << 20))

#define BLIST_SELECT_NO_ATN	((__force blist_flags_t)(1ULL << 21))

#define BLIST_RETRY_HWERROR	((__force blist_flags_t)(1ULL << 22))

#define BLIST_MAX_512		((__force blist_flags_t)(1ULL << 23))
#define __BLIST_UNUSED_24	((__force blist_flags_t)(1ULL << 24))

#define BLIST_NO_DIF		((__force blist_flags_t)(1ULL << 25))

#define BLIST_SKIP_VPD_PAGES	((__force blist_flags_t)(1ULL << 26))
#define __BLIST_UNUSED_27	((__force blist_flags_t)(1ULL << 27))

#define BLIST_TRY_VPD_PAGES	((__force blist_flags_t)(1ULL << 28))

#define BLIST_NO_RSOC		((__force blist_flags_t)(1ULL << 29))

#define BLIST_MAX_1024		((__force blist_flags_t)(1ULL << 30))

#define BLIST_UNMAP_LIMIT_WS	((__force blist_flags_t)(1ULL << 31))

#define BLIST_RETRY_ITF		((__force blist_flags_t)(1ULL << 32))

#define BLIST_RETRY_ASC_C1	((__force blist_flags_t)(1ULL << 33))

#define __BLIST_LAST_USED BLIST_RETRY_ASC_C1

#define __BLIST_HIGH_UNUSED (~(__BLIST_LAST_USED | \
			       (__force blist_flags_t) \
			       ((__force __u64)__BLIST_LAST_USED - 1ULL)))
#define __BLIST_UNUSED_MASK (__BLIST_UNUSED_14 | \
			     __BLIST_UNUSED_15 | \
			     __BLIST_UNUSED_16 | \
			     __BLIST_UNUSED_24 | \
			     __BLIST_UNUSED_27 | \
			     __BLIST_HIGH_UNUSED)

#endif
