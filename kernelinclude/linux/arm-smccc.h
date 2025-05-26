/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_ARM_SMCCC_H
#define __LINUX_ARM_SMCCC_H

#include <linux/args.h>
#include <linux/init.h>
#include <uapi/linux/const.h>



#define ARM_SMCCC_STD_CALL	        _AC(0,U)
#define ARM_SMCCC_FAST_CALL	        _AC(1,U)
#define ARM_SMCCC_TYPE_SHIFT		31

#define ARM_SMCCC_SMC_32		0
#define ARM_SMCCC_SMC_64		1
#define ARM_SMCCC_CALL_CONV_SHIFT	30

#define ARM_SMCCC_OWNER_MASK		0x3F
#define ARM_SMCCC_OWNER_SHIFT		24

#define ARM_SMCCC_FUNC_MASK		0xFFFF

#define ARM_SMCCC_IS_FAST_CALL(smc_val)	\
	((smc_val) & (ARM_SMCCC_FAST_CALL << ARM_SMCCC_TYPE_SHIFT))
#define ARM_SMCCC_IS_64(smc_val) \
	((smc_val) & (ARM_SMCCC_SMC_64 << ARM_SMCCC_CALL_CONV_SHIFT))
#define ARM_SMCCC_FUNC_NUM(smc_val)	((smc_val) & ARM_SMCCC_FUNC_MASK)
#define ARM_SMCCC_OWNER_NUM(smc_val) \
	(((smc_val) >> ARM_SMCCC_OWNER_SHIFT) & ARM_SMCCC_OWNER_MASK)

#define ARM_SMCCC_CALL_VAL(type, calling_convention, owner, func_num) \
	(((type) << ARM_SMCCC_TYPE_SHIFT) | \
	((calling_convention) << ARM_SMCCC_CALL_CONV_SHIFT) | \
	(((owner) & ARM_SMCCC_OWNER_MASK) << ARM_SMCCC_OWNER_SHIFT) | \
	((func_num) & ARM_SMCCC_FUNC_MASK))

#define ARM_SMCCC_OWNER_ARCH		0
#define ARM_SMCCC_OWNER_CPU		1
#define ARM_SMCCC_OWNER_SIP		2
#define ARM_SMCCC_OWNER_OEM		3
#define ARM_SMCCC_OWNER_STANDARD	4
#define ARM_SMCCC_OWNER_STANDARD_HYP	5
#define ARM_SMCCC_OWNER_VENDOR_HYP	6
#define ARM_SMCCC_OWNER_TRUSTED_APP	48
#define ARM_SMCCC_OWNER_TRUSTED_APP_END	49
#define ARM_SMCCC_OWNER_TRUSTED_OS	50
#define ARM_SMCCC_OWNER_TRUSTED_OS_END	63

#define ARM_SMCCC_FUNC_QUERY_CALL_UID  0xff01

#define ARM_SMCCC_QUIRK_NONE		0
#define ARM_SMCCC_QUIRK_QCOM_A6		1 

#define ARM_SMCCC_VERSION_1_0		0x10000
#define ARM_SMCCC_VERSION_1_1		0x10001
#define ARM_SMCCC_VERSION_1_2		0x10002
#define ARM_SMCCC_VERSION_1_3		0x10003

#define ARM_SMCCC_1_3_SVE_HINT		0x10000
#define ARM_SMCCC_CALL_HINTS		ARM_SMCCC_1_3_SVE_HINT


#define ARM_SMCCC_VERSION_FUNC_ID					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 0)

#define ARM_SMCCC_ARCH_FEATURES_FUNC_ID					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 1)

#define ARM_SMCCC_ARCH_SOC_ID						\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 2)

#define ARM_SMCCC_ARCH_WORKAROUND_1					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 0x8000)

#define ARM_SMCCC_ARCH_WORKAROUND_2					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 0x7fff)

#define ARM_SMCCC_ARCH_WORKAROUND_3					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   0, 0x3fff)

#define ARM_SMCCC_VENDOR_HYP_CALL_UID_FUNC_ID				\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   ARM_SMCCC_OWNER_VENDOR_HYP,			\
			   ARM_SMCCC_FUNC_QUERY_CALL_UID)


#define ARM_SMCCC_VENDOR_HYP_UID_KVM_REG_0	0xb66fb428U
#define ARM_SMCCC_VENDOR_HYP_UID_KVM_REG_1	0xe911c52eU
#define ARM_SMCCC_VENDOR_HYP_UID_KVM_REG_2	0x564bcaa9U
#define ARM_SMCCC_VENDOR_HYP_UID_KVM_REG_3	0x743a004dU


#define ARM_SMCCC_KVM_FUNC_FEATURES		0
#define ARM_SMCCC_KVM_FUNC_PTP			1
#define ARM_SMCCC_KVM_FUNC_FEATURES_2		127
#define ARM_SMCCC_KVM_NUM_FUNCS			128

#define ARM_SMCCC_VENDOR_HYP_KVM_FEATURES_FUNC_ID			\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   ARM_SMCCC_OWNER_VENDOR_HYP,			\
			   ARM_SMCCC_KVM_FUNC_FEATURES)

#define SMCCC_ARCH_WORKAROUND_RET_UNAFFECTED	1


#define ARM_SMCCC_VENDOR_HYP_KVM_PTP_FUNC_ID				\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,				\
			   ARM_SMCCC_SMC_32,				\
			   ARM_SMCCC_OWNER_VENDOR_HYP,			\
			   ARM_SMCCC_KVM_FUNC_PTP)


#define KVM_PTP_VIRT_COUNTER			0
#define KVM_PTP_PHYS_COUNTER			1


#define ARM_SMCCC_HV_PV_TIME_FEATURES				\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_64,			\
			   ARM_SMCCC_OWNER_STANDARD_HYP,	\
			   0x20)

#define ARM_SMCCC_HV_PV_TIME_ST					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_64,			\
			   ARM_SMCCC_OWNER_STANDARD_HYP,	\
			   0x21)


#define ARM_SMCCC_TRNG_VERSION					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_32,			\
			   ARM_SMCCC_OWNER_STANDARD,		\
			   0x50)

#define ARM_SMCCC_TRNG_FEATURES					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_32,			\
			   ARM_SMCCC_OWNER_STANDARD,		\
			   0x51)

#define ARM_SMCCC_TRNG_GET_UUID					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_32,			\
			   ARM_SMCCC_OWNER_STANDARD,		\
			   0x52)

#define ARM_SMCCC_TRNG_RND32					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_32,			\
			   ARM_SMCCC_OWNER_STANDARD,		\
			   0x53)

#define ARM_SMCCC_TRNG_RND64					\
	ARM_SMCCC_CALL_VAL(ARM_SMCCC_FAST_CALL,			\
			   ARM_SMCCC_SMC_64,			\
			   ARM_SMCCC_OWNER_STANDARD,		\
			   0x53)


#define SMCCC_RET_SUCCESS			0
#define SMCCC_RET_NOT_SUPPORTED			-1
#define SMCCC_RET_NOT_REQUIRED			-2
#define SMCCC_RET_INVALID_PARAMETER		-3

#ifndef __ASSEMBLY__

#include <linux/linkage.h>
#include <linux/types.h>

enum arm_smccc_conduit {
	SMCCC_CONDUIT_NONE,
	SMCCC_CONDUIT_SMC,
	SMCCC_CONDUIT_HVC,
};


enum arm_smccc_conduit arm_smccc_1_1_get_conduit(void);


u32 arm_smccc_get_version(void);

void __init arm_smccc_version_init(u32 version, enum arm_smccc_conduit conduit);

extern u64 smccc_has_sve_hint;


s32 arm_smccc_get_soc_id_version(void);


s32 arm_smccc_get_soc_id_revision(void);


struct arm_smccc_res {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
};

#ifdef CONFIG_ARM64

struct arm_smccc_1_2_regs {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
	unsigned long a4;
	unsigned long a5;
	unsigned long a6;
	unsigned long a7;
	unsigned long a8;
	unsigned long a9;
	unsigned long a10;
	unsigned long a11;
	unsigned long a12;
	unsigned long a13;
	unsigned long a14;
	unsigned long a15;
	unsigned long a16;
	unsigned long a17;
};


asmlinkage void arm_smccc_1_2_hvc(const struct arm_smccc_1_2_regs *args,
				  struct arm_smccc_1_2_regs *res);


asmlinkage void arm_smccc_1_2_smc(const struct arm_smccc_1_2_regs *args,
				  struct arm_smccc_1_2_regs *res);
#endif


struct arm_smccc_quirk {
	int	id;
	union {
		unsigned long a6;
	} state;
};


asmlinkage unsigned long __arm_smccc_sve_check(unsigned long x0);


#ifdef CONFIG_HAVE_ARM_SMCCC
asmlinkage void __arm_smccc_smc(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3, unsigned long a4,
			unsigned long a5, unsigned long a6, unsigned long a7,
			struct arm_smccc_res *res, struct arm_smccc_quirk *quirk);
#else
static inline void __arm_smccc_smc(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3, unsigned long a4,
			unsigned long a5, unsigned long a6, unsigned long a7,
			struct arm_smccc_res *res, struct arm_smccc_quirk *quirk)
{
	*res = (struct arm_smccc_res){};
}
#endif


asmlinkage void __arm_smccc_hvc(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3, unsigned long a4,
			unsigned long a5, unsigned long a6, unsigned long a7,
			struct arm_smccc_res *res, struct arm_smccc_quirk *quirk);

#define arm_smccc_smc(...) __arm_smccc_smc(__VA_ARGS__, NULL)

#define arm_smccc_smc_quirk(...) __arm_smccc_smc(__VA_ARGS__)

#define arm_smccc_hvc(...) __arm_smccc_hvc(__VA_ARGS__, NULL)

#define arm_smccc_hvc_quirk(...) __arm_smccc_hvc(__VA_ARGS__)


#ifdef CONFIG_ARM64

#define SMCCC_SMC_INST	"smc	#0"
#define SMCCC_HVC_INST	"hvc	#0"

#elif defined(CONFIG_ARM)
#include <asm/opcodes-sec.h>
#include <asm/opcodes-virt.h>

#define SMCCC_SMC_INST	__SMC(0)
#define SMCCC_HVC_INST	__HVC(0)

#endif


#if defined(CONFIG_ARM64_SVE) && !defined(__KVM_NVHE_HYPERVISOR__)

#define SMCCC_SVE_CHECK ALTERNATIVE("nop \n",  "bl __arm_smccc_sve_check \n", \
				    ARM64_SVE)
#define smccc_sve_clobbers "x16", "x30", "cc",

#else

#define SMCCC_SVE_CHECK
#define smccc_sve_clobbers

#endif

#define __constraint_read_2	"r" (arg0)
#define __constraint_read_3	__constraint_read_2, "r" (arg1)
#define __constraint_read_4	__constraint_read_3, "r" (arg2)
#define __constraint_read_5	__constraint_read_4, "r" (arg3)
#define __constraint_read_6	__constraint_read_5, "r" (arg4)
#define __constraint_read_7	__constraint_read_6, "r" (arg5)
#define __constraint_read_8	__constraint_read_7, "r" (arg6)
#define __constraint_read_9	__constraint_read_8, "r" (arg7)

#define __declare_arg_2(a0, res)					\
	struct arm_smccc_res   *___res = res;				\
	register unsigned long arg0 asm("r0") = (u32)a0

#define __declare_arg_3(a0, a1, res)					\
	typeof(a1) __a1 = a1;						\
	struct arm_smccc_res   *___res = res;				\
	register unsigned long arg0 asm("r0") = (u32)a0;			\
	register typeof(a1) arg1 asm("r1") = __a1

#define __declare_arg_4(a0, a1, a2, res)				\
	typeof(a1) __a1 = a1;						\
	typeof(a2) __a2 = a2;						\
	struct arm_smccc_res   *___res = res;				\
	register unsigned long arg0 asm("r0") = (u32)a0;			\
	register typeof(a1) arg1 asm("r1") = __a1;			\
	register typeof(a2) arg2 asm("r2") = __a2

#define __declare_arg_5(a0, a1, a2, a3, res)				\
	typeof(a1) __a1 = a1;						\
	typeof(a2) __a2 = a2;						\
	typeof(a3) __a3 = a3;						\
	struct arm_smccc_res   *___res = res;				\
	register unsigned long arg0 asm("r0") = (u32)a0;			\
	register typeof(a1) arg1 asm("r1") = __a1;			\
	register typeof(a2) arg2 asm("r2") = __a2;			\
	register typeof(a3) arg3 asm("r3") = __a3

#define __declare_arg_6(a0, a1, a2, a3, a4, res)			\
	typeof(a4) __a4 = a4;						\
	__declare_arg_5(a0, a1, a2, a3, res);				\
	register typeof(a4) arg4 asm("r4") = __a4

#define __declare_arg_7(a0, a1, a2, a3, a4, a5, res)			\
	typeof(a5) __a5 = a5;						\
	__declare_arg_6(a0, a1, a2, a3, a4, res);			\
	register typeof(a5) arg5 asm("r5") = __a5

#define __declare_arg_8(a0, a1, a2, a3, a4, a5, a6, res)		\
	typeof(a6) __a6 = a6;						\
	__declare_arg_7(a0, a1, a2, a3, a4, a5, res);			\
	register typeof(a6) arg6 asm("r6") = __a6

#define __declare_arg_9(a0, a1, a2, a3, a4, a5, a6, a7, res)		\
	typeof(a7) __a7 = a7;						\
	__declare_arg_8(a0, a1, a2, a3, a4, a5, a6, res);		\
	register typeof(a7) arg7 asm("r7") = __a7


#define __arm_smccc_1_1(inst, ...)					\
	do {								\
		register unsigned long r0 asm("r0");			\
		register unsigned long r1 asm("r1");			\
		register unsigned long r2 asm("r2");			\
		register unsigned long r3 asm("r3"); 			\
		CONCATENATE(__declare_arg_,				\
			    COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__);	\
		asm volatile(SMCCC_SVE_CHECK				\
			     inst "\n" :				\
			     "=r" (r0), "=r" (r1), "=r" (r2), "=r" (r3)	\
			     : CONCATENATE(__constraint_read_,		\
					   COUNT_ARGS(__VA_ARGS__))	\
			     : smccc_sve_clobbers "memory");		\
		if (___res)						\
			*___res = (typeof(*___res)){r0, r1, r2, r3};	\
	} while (0)


#define arm_smccc_1_1_smc(...)	__arm_smccc_1_1(SMCCC_SMC_INST, __VA_ARGS__)


#define arm_smccc_1_1_hvc(...)	__arm_smccc_1_1(SMCCC_HVC_INST, __VA_ARGS__)


#define __fail_smccc_1_1(...)						\
	do {								\
		CONCATENATE(__declare_arg_,				\
			    COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__);	\
		asm ("" :						\
		     : CONCATENATE(__constraint_read_,			\
				   COUNT_ARGS(__VA_ARGS__))		\
		     : smccc_sve_clobbers "memory");			\
		if (___res)						\
			___res->a0 = SMCCC_RET_NOT_SUPPORTED;		\
	} while (0)


#define arm_smccc_1_1_invoke(...) ({					\
		int method = arm_smccc_1_1_get_conduit();		\
		switch (method) {					\
		case SMCCC_CONDUIT_HVC:					\
			arm_smccc_1_1_hvc(__VA_ARGS__);			\
			break;						\
		case SMCCC_CONDUIT_SMC:					\
			arm_smccc_1_1_smc(__VA_ARGS__);			\
			break;						\
		default:						\
			__fail_smccc_1_1(__VA_ARGS__);			\
			method = SMCCC_CONDUIT_NONE;			\
			break;						\
		}							\
		method;							\
	})

#endif 
#endif 
