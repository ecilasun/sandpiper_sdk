/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOUND_EMU10K1_H
#define __SOUND_EMU10K1_H


#include <sound/pcm.h>
#include <sound/rawmidi.h>
#include <sound/hwdep.h>
#include <sound/ac97_codec.h>
#include <sound/util_mem.h>
#include <sound/pcm-indirect.h>
#include <sound/timer.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/firmware.h>
#include <linux/io.h>

#include <uapi/sound/emu10k1.h>



#define EMUPAGESIZE     4096
#define MAXPAGES0       4096	
#define MAXPAGES1       8192	
#define NUM_G           64              
#define NUM_EFX_PLAYBACK    16


#define EMU10K1_DMA_MASK	0x7fffffffUL	
#define AUDIGY_DMA_MASK		0xffffffffUL	

#define TMEMSIZE        256*1024

#define IP_TO_CP(ip) ((ip == 0) ? 0 : (((0x00001000uL | (ip & 0x00000FFFL)) << (((ip >> 12) & 0x000FL) + 4)) & 0xFFFF0000uL))

// This is used to define hardware bit-fields (sub-registers) by combining
// the bit shift and count with the actual register address. The passed
// mask must represent a single run of adjacent bits.
// The non-concatenating (_NC) variant should be used directly only for
// sub-registers that do not follow the <register>_<field> naming pattern.
#define SUB_REG_NC(reg, field, mask) \
	enum { \
		field ## _MASK = mask, \
		field = reg | \
			(__builtin_ctz(mask) << 16) | \
			(__builtin_popcount(mask) << 24), \
	};
#define SUB_REG(reg, field, mask) SUB_REG_NC(reg, reg ## _ ## field, mask)

// Macros for manipulating values of bit-fields declared using the above macros.
// Best used with constant register addresses, as otherwise quite some code is
// generated. The actual register read/write functions handle combined addresses
// automatically, so use of these macros conveys no advantage when accessing a
// single sub-register at a time.
#define REG_SHIFT(r) (((r) >> 16) & 0x1f)
#define REG_SIZE(r) (((r) >> 24) & 0x1f)
#define REG_MASK0(r) ((1U << REG_SIZE(r)) - 1U)
#define REG_MASK(r) (REG_MASK0(r) << REG_SHIFT(r))
#define REG_VAL_GET(r, v) ((v & REG_MASK(r)) >> REG_SHIFT(r))
#define REG_VAL_PUT(r, v) ((v) << REG_SHIFT(r))

// List terminator for snd_emu10k1_ptr_write_multiple()
#define REGLIST_END ~0

// Audigy specify registers are prefixed with 'A_'





#define PTR			0x00		
						
						
#define PTR_CHANNELNUM_MASK	0x0000003f	
						
						
						
#define PTR_ADDRESS_MASK	0x07ff0000	
#define A_PTR_ADDRESS_MASK	0x0fff0000

#define DATA			0x04		

#define IPR			0x08		
						
						
#define IPR_P16V		0x80000000	
#define IPR_WATERMARK_REACHED	0x40000000
#define IPR_A_GPIO		0x20000000	


#define IPR_A_MIDITRANSBUFEMPTY2 0x10000000	
#define IPR_A_MIDIRECVBUFEMPTY2	0x08000000	

#define IPR_SPDIFBUFFULL	0x04000000	
#define IPR_SPDIFBUFHALFFULL	0x02000000	

#define IPR_SAMPLERATETRACKER	0x01000000	
#define IPR_FXDSP		0x00800000	
#define IPR_FORCEINT		0x00400000	
#define IPR_PCIERROR		0x00200000	
#define IPR_VOLINCR		0x00100000	
#define IPR_VOLDECR		0x00080000	
#define IPR_MUTE		0x00040000	
#define IPR_MICBUFFULL		0x00020000	
#define IPR_MICBUFHALFFULL	0x00010000	
#define IPR_ADCBUFFULL		0x00008000	
#define IPR_ADCBUFHALFFULL	0x00004000	
#define IPR_EFXBUFFULL		0x00002000	
#define IPR_EFXBUFHALFFULL	0x00001000	
#define IPR_GPSPDIFSTATUSCHANGE	0x00000800	
#define IPR_CDROMSTATUSCHANGE	0x00000400	
#define IPR_INTERVALTIMER	0x00000200	
#define IPR_MIDITRANSBUFEMPTY	0x00000100	
#define IPR_MIDIRECVBUFEMPTY	0x00000080	
#define IPR_CHANNELLOOP		0x00000040	
						
						
						
						
#define IPR_CHANNELNUMBERMASK	0x0000003f	
						
						
						
						

#define INTE			0x0c		
#define INTE_VIRTUALSB_MASK	0xc0000000	
#define INTE_VIRTUALSB_220	0x00000000	
#define INTE_VIRTUALSB_240	0x40000000	
#define INTE_VIRTUALSB_260	0x80000000	
#define INTE_VIRTUALSB_280	0xc0000000	
#define INTE_VIRTUALMPU_MASK	0x30000000	
#define INTE_VIRTUALMPU_300	0x00000000	
#define INTE_VIRTUALMPU_310	0x10000000	
#define INTE_VIRTUALMPU_320	0x20000000	
#define INTE_VIRTUALMPU_330	0x30000000	
#define INTE_MASTERDMAENABLE	0x08000000	
#define INTE_SLAVEDMAENABLE	0x04000000	
#define INTE_MASTERPICENABLE	0x02000000	
#define INTE_SLAVEPICENABLE	0x01000000	
#define INTE_VSBENABLE		0x00800000	
#define INTE_ADLIBENABLE	0x00400000	
#define INTE_MPUENABLE		0x00200000	
#define INTE_FORCEINT		0x00100000	

#define INTE_MRHANDENABLE	0x00080000	
						
						
						
						

#define INTE_A_GPIOENABLE 	0x00040000	


#define INTE_A_MIDITXENABLE2	0x00020000	
#define INTE_A_MIDIRXENABLE2	0x00010000	

#define INTE_A_SPDIF_BUFFULL_ENABLE 	0x00008000
#define INTE_A_SPDIF_HALFBUFFULL_ENABLE	0x00004000

#define INTE_SAMPLERATETRACKER	0x00002000	
						
#define INTE_FXDSPENABLE	0x00001000	
#define INTE_PCIERRORENABLE	0x00000800	
#define INTE_VOLINCRENABLE	0x00000400	
#define INTE_VOLDECRENABLE	0x00000200	
#define INTE_MUTEENABLE		0x00000100	
#define INTE_MICBUFENABLE	0x00000080	
#define INTE_ADCBUFENABLE	0x00000040	
#define INTE_EFXBUFENABLE	0x00000020	
#define INTE_GPSPDIFENABLE	0x00000010	
#define INTE_CDSPDIFENABLE	0x00000008	
#define INTE_INTERVALTIMERENB	0x00000004	
#define INTE_MIDITXENABLE	0x00000002	
#define INTE_MIDIRXENABLE	0x00000001	

#define WC			0x10		
SUB_REG(WC, SAMPLECOUNTER,	0x03FFFFC0)	
SUB_REG(WC, CURRENTCHANNEL,	0x0000003F)	
						
						

#define HCFG			0x14		
						
						
						
						
#define HCFG_LEGACYFUNC_MASK	0xe0000000	
#define HCFG_LEGACYFUNC_MPU	0x00000000	
#define HCFG_LEGACYFUNC_SB	0x40000000	
#define HCFG_LEGACYFUNC_AD	0x60000000	
#define HCFG_LEGACYFUNC_MPIC	0x80000000	
#define HCFG_LEGACYFUNC_MDMA	0xa0000000	
#define HCFG_LEGACYFUNC_SPCI	0xc0000000	
#define HCFG_LEGACYFUNC_SDMA	0xe0000000	
#define HCFG_IOCAPTUREADDR	0x1f000000	
#define HCFG_LEGACYWRITE	0x00800000	
#define HCFG_LEGACYWORD		0x00400000	
#define HCFG_LEGACYINT		0x00200000	
						
						
#define HCFG_PUSH_BUTTON_ENABLE 0x00100000	
#define HCFG_BAUD_RATE		0x00080000	
#define HCFG_EXPANDED_MEM	0x00040000	
#define HCFG_CODECFORMAT_MASK	0x00030000	



#define HCFG_CODECFORMAT_AC97_1	0x00000000	
#define HCFG_CODECFORMAT_AC97_2	0x00010000	
#define HCFG_AUTOMUTE_ASYNC	0x00008000	
						
						
						
#define HCFG_AUTOMUTE_SPDIF	0x00004000	
						
						
#define HCFG_EMU32_SLAVE	0x00002000	
#define HCFG_SLOW_RAMP		0x00001000	

#define HCFG_PHASE_TRACK_MASK	0x00000700	
						
						
#define HCFG_I2S_ASRC_ENABLE	0x00000070	
						
 						




#define HCFG_CODECFORMAT_AC97	0x00000000	
#define HCFG_CODECFORMAT_I2S	0x00010000	
#define HCFG_GPINPUT0		0x00004000	
#define HCFG_GPINPUT1		0x00002000	
#define HCFG_GPOUTPUT_MASK	0x00001c00	
#define HCFG_GPOUT0		0x00001000	
#define HCFG_GPOUT1		0x00000800	
#define HCFG_GPOUT2		0x00000400	
#define HCFG_JOYENABLE      	0x00000200	
#define HCFG_PHASETRACKENABLE	0x00000100	
						
						
#define HCFG_AC3ENABLE_MASK	0x000000e0	
#define HCFG_AC3ENABLE_ZVIDEO	0x00000080	
#define HCFG_AC3ENABLE_CDSPDIF	0x00000040	
#define HCFG_AC3ENABLE_GPSPDIF  0x00000020      
#define HCFG_AUTOMUTE		0x00000010	
						
						
						
#define HCFG_LOCKSOUNDCACHE	0x00000008	
						
SUB_REG(HCFG, LOCKTANKCACHE,	0x00000004)	
						
#define HCFG_MUTEBUTTONENABLE	0x00000002	
						
						
						
						
						
#define HCFG_AUDIOENABLE	0x00000001	
						
						

// On Audigy, the MPU port moved to the 0x70-0x74 ptr registers

#define MUDATA			0x18		

#define MUCMD			0x19		
#define MUCMD_RESET		0xff		
#define MUCMD_ENTERUARTMODE	0x3f		
						

#define MUSTAT			MUCMD		
#define MUSTAT_IRDYN		0x80		
#define MUSTAT_ORDYN		0x40		

#define A_GPIO			0x18		
#define A_GPINPUT_MASK		0xff00		
#define A3_GPINPUT_MASK		0x3f00		
#define A_GPOUTPUT_MASK		0x00ff

// The GPIO port is used for I/O config on Sound Blasters;
// card-specific info can be found in the emu_chip_details table.
// On E-MU cards the port is used as the interface to the FPGA.

// Audigy output/GPIO stuff taken from the kX drivers
#define A_IOCFG			A_GPIO
#define A_IOCFG_GPOUT0		0x0044		
#define A_IOCFG_DISABLE_ANALOG	0x0040		
#define A_IOCFG_ENABLE_DIGITAL	0x0004
#define A_IOCFG_ENABLE_DIGITAL_AUDIGY4	0x0080
#define A_IOCFG_UNKNOWN_20      0x0020
#define A_IOCFG_DISABLE_AC97_FRONT      0x0080  
#define A_IOCFG_GPOUT1		0x0002		
#define A_IOCFG_GPOUT2		0x0001		
#define A_IOCFG_MULTIPURPOSE_JACK	0x2000  
                                                
#define A_IOCFG_DIGITAL_JACK    0x1000          
#define A_IOCFG_FRONT_JACK      0x4000
#define A_IOCFG_REAR_JACK       0x8000
#define A_IOCFG_PHONES_JACK     0x0100          

#define TIMER			0x1a		
						
						
						
#define TIMER_RATE_MASK		0x03ff		
						

#define AC97DATA		0x1c		

#define AC97ADDRESS		0x1e		
#define AC97ADDRESS_READY	0x80		
#define AC97ADDRESS_ADDRESS	0x7f		


#define PTR2			0x20		
#define DATA2			0x24		
#define IPR2			0x28		
#define IPR2_PLAYBACK_CH_0_LOOP      0x00001000 
#define IPR2_PLAYBACK_CH_0_HALF_LOOP 0x00000100 
#define IPR2_CAPTURE_CH_0_LOOP       0x00100000 
#define IPR2_CAPTURE_CH_0_HALF_LOOP  0x00010000 
						
#define INTE2			0x2c		
#define INTE2_PLAYBACK_CH_0_LOOP      0x00001000 
#define INTE2_PLAYBACK_CH_0_HALF_LOOP 0x00000100 
#define INTE2_PLAYBACK_CH_1_LOOP      0x00002000 
#define INTE2_PLAYBACK_CH_1_HALF_LOOP 0x00000200 
#define INTE2_PLAYBACK_CH_2_LOOP      0x00004000 
#define INTE2_PLAYBACK_CH_2_HALF_LOOP 0x00000400 
#define INTE2_PLAYBACK_CH_3_LOOP      0x00008000 
#define INTE2_PLAYBACK_CH_3_HALF_LOOP 0x00000800 
#define INTE2_CAPTURE_CH_0_LOOP       0x00100000 
#define INTE2_CAPTURE_CH_0_HALF_LOOP  0x00010000 
#define HCFG2			0x34		
						
						
						
						
						
#define IPR3			0x38		
#define INTE3			0x3c		





#define JOYSTICK1		0x00		
#define JOYSTICK2		0x01		
#define JOYSTICK3		0x02		
#define JOYSTICK4		0x03		
#define JOYSTICK5		0x04		
#define JOYSTICK6		0x05		
#define JOYSTICK7		0x06		
#define JOYSTICK8		0x07		



#define JOYSTICK_BUTTONS	0x0f		
#define JOYSTICK_COMPARATOR	0xf0		





// No official documentation was released for EMU10K1, but some info
// about playback can be extrapolated from the EMU8K documents:
// "AWE32/EMU8000 Programmer’s Guide" (emu8kpgm.pdf) - registers
// "AWE32 Developer's Information Pack" (adip301.pdf) - high-level view

// The short version:
// - The engine has 64 playback channels, also called voices. The channels
//   operate independently, except when paired for stereo (see below).
// - PCM samples are fetched into the cache; see description of CD0 below.
// - Samples are consumed at the rate CPF_CURRENTPITCH.
// - 8-bit samples are transformed upon use: cooked = (raw ^ 0x80) << 8
// - 8 samples are read at CCR_READADDRESS:CPF_FRACADDRESS and interpolated
//   according to CCCA_INTERPROM_*. With CCCA_INTERPROM_0 selected and a zero
//   CPF_FRACADDRESS, this results in CCR_READADDRESS[3] being used verbatim.
// - The value is multiplied by CVCF_CURRENTVOL.
// - The value goes through a filter with cutoff CVCF_CURRENTFILTER;
//   delay stages Z1 and Z2.
// - The value is added by so-called `sends` to 4 (EMU10K1) / 8 (EMU10K2)
//   of the 16 (EMU10K1) / 64 (EMU10K2) FX bus accumulators via FXRT*,
//   multiplied by a per-send amount (*_FXSENDAMOUNT_*).
//   The scaling of the send amounts is exponential-ish.
// - The DSP has a go at FXBUS* and outputs the values to EXTOUT* or EMU32OUT*.
// - The pitch, volume, and filter cutoff can be modulated by two envelope
//   engines and two low frequency oscillators.
// - To avoid abrupt changes to the parameters (which may cause audible
//   distortion), the modulation engine sets the target registers, towards
//   which the current registers "swerve" gradually.

// For the odd channel in a stereo pair, these registers are meaningless:
//   CPF_STEREO, CPF_CURRENTPITCH, PTRX_PITCHTARGET, CCR_CACHEINVALIDSIZE,
//   PSST_LOOPSTARTADDR, DSL_LOOPENDADDR, CCCA_CURRADDR
// The somewhat non-obviously still meaningful ones are:
//   CPF_STOP, CPF_FRACADDRESS, CCR_READADDRESS (!),
//   CCCA_INTERPROM, CCCA_8BITSELECT (!)
// (The envelope engine is ignored here, as stereo matters only for verbatim playback.)

#define CPF			0x00		
SUB_REG(CPF, CURRENTPITCH,	0xffff0000)	
#define CPF_STEREO_MASK		0x00008000	
SUB_REG(CPF, STOP,		0x00004000)	
						
#define CPF_FRACADDRESS_MASK	0x00003fff	

#define PTRX			0x01		
SUB_REG(PTRX, PITCHTARGET,	0xffff0000)	
SUB_REG(PTRX, FXSENDAMOUNT_A,	0x0000ff00)	
SUB_REG(PTRX, FXSENDAMOUNT_B,	0x000000ff)	

// Note: the volumes are raw multpliers, so real 100% is impossible.
#define CVCF			0x02		
SUB_REG(CVCF, CURRENTVOL,	0xffff0000)	
SUB_REG(CVCF, CURRENTFILTER,	0x0000ffff)	

#define VTFT			0x03		
SUB_REG(VTFT, VOLUMETARGET,	0xffff0000)	
SUB_REG(VTFT, FILTERTARGET,	0x0000ffff)	

#define Z1			0x05		

#define Z2			0x04		

#define PSST			0x06		
SUB_REG(PSST, FXSENDAMOUNT_C,	0xff000000)	
SUB_REG(PSST, LOOPSTARTADDR,	0x00ffffff)	

#define DSL			0x07		
SUB_REG(DSL, FXSENDAMOUNT_D,	0xff000000)	
SUB_REG(DSL, LOOPENDADDR,	0x00ffffff)	

#define CCCA			0x08		
SUB_REG(CCCA, RESONANCE,	0xf0000000)	
#define CCCA_INTERPROM_MASK	0x0e000000	
						
						
						
						
						
#define CCCA_INTERPROM_0	0x00000000	
#define CCCA_INTERPROM_1	0x02000000	
#define CCCA_INTERPROM_2	0x04000000	
#define CCCA_INTERPROM_3	0x06000000	
#define CCCA_INTERPROM_4	0x08000000	
#define CCCA_INTERPROM_5	0x0a000000	
#define CCCA_INTERPROM_6	0x0c000000	
#define CCCA_INTERPROM_7	0x0e000000	
#define CCCA_8BITSELECT		0x01000000	
						
SUB_REG(CCCA, CURRADDR,		0x00ffffff)	

#define CCR			0x09		
SUB_REG(CCR, CACHEINVALIDSIZE,	0xfe000000)	
#define CCR_CACHELOOPFLAG	0x01000000	
#define CCR_INTERLEAVEDSAMPLES	0x00800000	
						
#define CCR_WORDSIZEDSAMPLES	0x00400000	
						
SUB_REG(CCR, READADDRESS,	0x003f0000)	
SUB_REG(CCR, LOOPINVALSIZE,	0x0000fe00)	
						
#define CCR_LOOPFLAG		0x00000100	
SUB_REG(CCR, CACHELOOPADDRHI,	0x000000ff)	

#define CLP			0x0a		
						
SUB_REG(CLP, CACHELOOPADDR,	0x0000ffff)	

#define FXRT			0x0b		
						
						
#define FXRT_CHANNELA		0x000f0000	
#define FXRT_CHANNELB		0x00f00000	
#define FXRT_CHANNELC		0x0f000000	
#define FXRT_CHANNELD		0xf0000000	

#define MAPA			0x0c		
#define MAPB			0x0d		

#define MAP_PTE_MASK0		0xfffff000	
#define MAP_PTI_MASK0		0x00000fff	

#define MAP_PTE_MASK1		0xffffe000	
#define MAP_PTI_MASK1		0x00001fff	



#define ENVVOL			0x10		
#define ENVVOL_MASK		0x0000ffff	  
						

#define ATKHLDV 		0x11		
#define ATKHLDV_PHASE0_MASK	0x00008000	
#define ATKHLDV_HOLDTIME_MASK	0x00007f00	
#define ATKHLDV_ATTACKTIME_MASK	0x0000007f	
						

#define DCYSUSV 		0x12		
#define DCYSUSV_PHASE1_MASK	0x00008000	
#define DCYSUSV_SUSTAINLEVEL_MASK 0x00007f00	
#define DCYSUSV_CHANNELENABLE_MASK 0x00000080	
						
						
#define DCYSUSV_DECAYTIME_MASK	0x0000007f	
						

#define LFOVAL1 		0x13		
#define LFOVAL_MASK		0x0000ffff	
						

#define ENVVAL			0x14		
#define ENVVAL_MASK		0x0000ffff	
						

#define ATKHLDM			0x15		
#define ATKHLDM_PHASE0_MASK	0x00008000	
#define ATKHLDM_HOLDTIME	0x00007f00	
#define ATKHLDM_ATTACKTIME	0x0000007f	
						

#define DCYSUSM			0x16		
#define DCYSUSM_PHASE1_MASK	0x00008000	
#define DCYSUSM_SUSTAINLEVEL_MASK 0x00007f00	
#define DCYSUSM_DECAYTIME_MASK	0x0000007f	
						

#define LFOVAL2 		0x17		
#define LFOVAL2_MASK		0x0000ffff	
						

#define IP			0x18		
#define IP_MASK			0x0000ffff	
						
#define IP_UNITY		0x0000e000	

#define IFATN			0x19		
SUB_REG(IFATN, FILTERCUTOFF,	0x0000ff00)	
						
						
SUB_REG(IFATN, ATTENUATION,	0x000000ff)	

#define PEFE			0x1a		
SUB_REG(PEFE, PITCHAMOUNT,	0x0000ff00)	
						
SUB_REG(PEFE, FILTERAMOUNT,	0x000000ff)	
						


#define FMMOD			0x1b		
#define FMMOD_MODVIBRATO	0x0000ff00	
						
#define FMMOD_MOFILTER		0x000000ff	
						

#define TREMFRQ 		0x1c		
#define TREMFRQ_DEPTH		0x0000ff00	
						
#define TREMFRQ_FREQUENCY	0x000000ff	
						

#define FM2FRQ2 		0x1d		
#define FM2FRQ2_DEPTH		0x0000ff00	
						
#define FM2FRQ2_FREQUENCY	0x000000ff	
						

#define TEMPENV 		0x1e		
#define TEMPENV_MASK		0x0000ffff	
						
						



// 32 cache registers (== 128 bytes) per channel follow.
// In stereo mode, the two channels' caches are concatenated into one,
// and hold the interleaved frames.
// The cache holds 64 frames, so the upper half is not used in 8-bit mode.
// All registers mentioned below count in frames.
// The cache is a ring buffer; CCR_READADDRESS operates modulo 64.
// The cache is filled from (CCCA_CURRADDR - CCR_CACHEINVALIDSIZE)
// into (CCR_READADDRESS - CCR_CACHEINVALIDSIZE).
// The engine has a fetch threshold of 32 bytes, so it tries to keep
// CCR_CACHEINVALIDSIZE below 8 (16-bit stereo), 16 (16-bit mono,
// 8-bit stereo), or 32 (8-bit mono). The actual transfers are pretty
// unpredictable, especially if several voices are running.
// Frames are consumed at CCR_READADDRESS, which is incremented afterwards,
// along with CCCA_CURRADDR and CCR_CACHEINVALIDSIZE. This implies that the
// actual playback position always lags CCCA_CURRADDR by exactly 64 frames.
#define CD0			0x20		

#define PTB			0x40		
#define PTB_MASK		0xfffff000	

#define TCB			0x41		
#define TCB_MASK		0xfffff000	

#define ADCCR			0x42		
#define ADCCR_RCHANENABLE	0x00000010	
#define ADCCR_LCHANENABLE	0x00000008	
						
						
#define A_ADCCR_RCHANENABLE	0x00000020
#define A_ADCCR_LCHANENABLE	0x00000010

#define A_ADCCR_SAMPLERATE_MASK 0x0000000F      
#define ADCCR_SAMPLERATE_MASK	0x00000007	
#define ADCCR_SAMPLERATE_48	0x00000000	
#define ADCCR_SAMPLERATE_44	0x00000001	
#define ADCCR_SAMPLERATE_32	0x00000002	
#define ADCCR_SAMPLERATE_24	0x00000003	
#define ADCCR_SAMPLERATE_22	0x00000004	
#define ADCCR_SAMPLERATE_16	0x00000005	
#define ADCCR_SAMPLERATE_11	0x00000006	
#define ADCCR_SAMPLERATE_8	0x00000007	
#define A_ADCCR_SAMPLERATE_12	0x00000006	
#define A_ADCCR_SAMPLERATE_11	0x00000007	
#define A_ADCCR_SAMPLERATE_8	0x00000008	

#define FXWC			0x43		
						
						
						
						
						

#define A_TBLSZ			0x43	

#define TCBS			0x44		
#define TCBS_MASK		0x00000007	
#define TCBS_BUFFSIZE_16K	0x00000000
#define TCBS_BUFFSIZE_32K	0x00000001
#define TCBS_BUFFSIZE_64K	0x00000002
#define TCBS_BUFFSIZE_128K	0x00000003
#define TCBS_BUFFSIZE_256K	0x00000004
#define TCBS_BUFFSIZE_512K	0x00000005
#define TCBS_BUFFSIZE_1024K	0x00000006
#define TCBS_BUFFSIZE_2048K	0x00000007

#define MICBA			0x45		
#define MICBA_MASK		0xfffff000	

#define ADCBA			0x46		
#define ADCBA_MASK		0xfffff000	

#define FXBA			0x47		
#define FXBA_MASK		0xfffff000	

#define A_HWM			0x48		

#define MICBS			0x49		

#define ADCBS			0x4a		

#define FXBS			0x4b		


#define ADCBS_BUFSIZE_NONE	0x00000000
#define ADCBS_BUFSIZE_384	0x00000001
#define ADCBS_BUFSIZE_448	0x00000002
#define ADCBS_BUFSIZE_512	0x00000003
#define ADCBS_BUFSIZE_640	0x00000004
#define ADCBS_BUFSIZE_768	0x00000005
#define ADCBS_BUFSIZE_896	0x00000006
#define ADCBS_BUFSIZE_1024	0x00000007
#define ADCBS_BUFSIZE_1280	0x00000008
#define ADCBS_BUFSIZE_1536	0x00000009
#define ADCBS_BUFSIZE_1792	0x0000000a
#define ADCBS_BUFSIZE_2048	0x0000000b
#define ADCBS_BUFSIZE_2560	0x0000000c
#define ADCBS_BUFSIZE_3072	0x0000000d
#define ADCBS_BUFSIZE_3584	0x0000000e
#define ADCBS_BUFSIZE_4096	0x0000000f
#define ADCBS_BUFSIZE_5120	0x00000010
#define ADCBS_BUFSIZE_6144	0x00000011
#define ADCBS_BUFSIZE_7168	0x00000012
#define ADCBS_BUFSIZE_8192	0x00000013
#define ADCBS_BUFSIZE_10240	0x00000014
#define ADCBS_BUFSIZE_12288	0x00000015
#define ADCBS_BUFSIZE_14366	0x00000016
#define ADCBS_BUFSIZE_16384	0x00000017
#define ADCBS_BUFSIZE_20480	0x00000018
#define ADCBS_BUFSIZE_24576	0x00000019
#define ADCBS_BUFSIZE_28672	0x0000001a
#define ADCBS_BUFSIZE_32768	0x0000001b
#define ADCBS_BUFSIZE_40960	0x0000001c
#define ADCBS_BUFSIZE_49152	0x0000001d
#define ADCBS_BUFSIZE_57344	0x0000001e
#define ADCBS_BUFSIZE_65536	0x0000001f

// On Audigy, the FX send amounts are not applied instantly, but determine
// targets towards which the following registers swerve gradually.
#define A_CSBA			0x4c		
#define A_CSDC			0x4d		
#define A_CSFE			0x4e		
#define A_CSHG			0x4f		

// NOTE: 0x50,51,52: 64-bit (split over voices 0 & 1)
#define CDCS			0x50		

#define GPSCS			0x51		

// Corresponding EMU10K1_DBG_* constants are in the public header
#define DBG			0x52

#define A_SPSC			0x52		

#define REG53			0x53		

// Corresponding A_DBG_* constants are in the public header
#define A_DBG			0x53

// NOTE: 0x54,55,56: 64-bit (split over voices 0 & 1)
#define SPCS0			0x54		

#define SPCS1			0x55		

#define SPCS2			0x56		

#define SPCS_CLKACCYMASK	0x30000000	
#define SPCS_CLKACCY_1000PPM	0x00000000	
#define SPCS_CLKACCY_50PPM	0x10000000	
#define SPCS_CLKACCY_VARIABLE	0x20000000	
#define SPCS_SAMPLERATEMASK	0x0f000000	
#define SPCS_SAMPLERATE_44	0x00000000	
#define SPCS_SAMPLERATE_48	0x02000000	
#define SPCS_SAMPLERATE_32	0x03000000	
#define SPCS_CHANNELNUMMASK	0x00f00000	
#define SPCS_CHANNELNUM_UNSPEC	0x00000000	
#define SPCS_CHANNELNUM_LEFT	0x00100000	
#define SPCS_CHANNELNUM_RIGHT	0x00200000	
#define SPCS_SOURCENUMMASK	0x000f0000	
#define SPCS_SOURCENUM_UNSPEC	0x00000000	
#define SPCS_GENERATIONSTATUS	0x00008000	
#define SPCS_CATEGORYCODEMASK	0x00007f00	
#define SPCS_MODEMASK		0x000000c0	
#define SPCS_EMPHASISMASK	0x00000038	
#define SPCS_EMPHASIS_NONE	0x00000000	
#define SPCS_EMPHASIS_50_15	0x00000008	
#define SPCS_COPYRIGHT		0x00000004	
#define SPCS_NOTAUDIODATA	0x00000002	
#define SPCS_PROFESSIONAL	0x00000001	




#define CLIEL			0x58		
#define CLIEH			0x59		

#define CLIPL			0x5a		
#define CLIPH			0x5b		

// These cause CPF_STOP_MASK to be set shortly after CCCA_CURRADDR passes DSL_LOOPENDADDR.
// Subsequent changes to the address registers don't resume; clearing the bit here or in CPF does.
// The registers are NOT synchronized; the next serviced channel picks up immediately.
#define SOLEL			0x5c		
#define SOLEH			0x5d		

#define SPBYPASS		0x5e		
#define SPBYPASS_SPDIF0_MASK	0x00000003	
#define SPBYPASS_SPDIF1_MASK	0x0000000c	

#define SPBYPASS_FORMAT		0x00000f00      

#define AC97SLOT		0x5f            
#define AC97SLOT_REAR_RIGHT	0x01		
#define AC97SLOT_REAR_LEFT	0x02		
#define AC97SLOT_CNTR		0x10            
#define AC97SLOT_LFE		0x20            

#define A_PCB			0x5f		

// NOTE: 0x60,61,62: 64-bit
#define CDSRCS			0x60		

#define GPSRCS			0x61		

#define ZVSRCS			0x62		
						
						


#define SRCS_SPDIFVALID		0x04000000	
#define SRCS_SPDIFLOCKED	0x02000000	
#define SRCS_RATELOCKED		0x01000000	
#define SRCS_ESTSAMPLERATE	0x0007ffff	


#define SRCS_SPDIFRATE_44	0x0003acd9
#define SRCS_SPDIFRATE_48	0x00040000
#define SRCS_SPDIFRATE_96	0x00080000

#define MICIDX                  0x63            
SUB_REG(MICIDX, IDX,		0x0000ffff)

#define ADCIDX			0x64		
SUB_REG(ADCIDX, IDX,		0x0000ffff)

#define A_ADCIDX		0x63
SUB_REG(A_ADCIDX, IDX,		0x0000ffff)

#define A_MICIDX		0x64
SUB_REG(A_MICIDX, IDX,		0x0000ffff)

#define FXIDX			0x65		
SUB_REG(FXIDX, IDX,		0x0000ffff)


#define HLIEL			0x66		
#define HLIEH			0x67		

#define HLIPL			0x68		
#define HLIPH			0x69		

#define A_SPRI			0x6a		
#define A_SPRA			0x6b		
#define A_SPRC			0x6c		

#define A_DICE			0x6d		

#define A_TTB			0x6e		
#define A_TDOF			0x6f		


#define A_MUDATA1		0x70
#define A_MUCMD1		0x71
#define A_MUSTAT1		A_MUCMD1


#define A_MUDATA2		0x72
#define A_MUCMD2		0x73
#define A_MUSTAT2		A_MUCMD2	




#define A_FXWC1			0x74            
#define A_FXWC2			0x75		

#define A_EHC			0x76		

#define A_SPDIF_SAMPLERATE	A_EHC		
#define A_SPDIF_RATE_MASK	0x000000e0	
#define A_SPDIF_48000		0x00000000	
#define A_SPDIF_192000		0x00000020
#define A_SPDIF_96000		0x00000040
#define A_SPDIF_44100		0x00000080
#define A_SPDIF_MUTED		0x000000c0

SUB_REG_NC(A_EHC, A_I2S_CAPTURE_RATE, 0x00000e00)  
						   
#define A_I2S_CAPTURE_48000	0x0
#define A_I2S_CAPTURE_192000	0x1
#define A_I2S_CAPTURE_96000	0x2
#define A_I2S_CAPTURE_44100	0x4

#define A_EHC_SRC48_MASK	0x0000e000	
#define A_EHC_SRC48_BYPASS	0x00000000
#define A_EHC_SRC48_192		0x00002000
#define A_EHC_SRC48_96		0x00004000
#define A_EHC_SRC48_44		0x00008000
#define A_EHC_SRC48_MUTED	0x0000c000

#define A_EHC_P17V_TVM		0x00000001	
#define A_EHC_P17V_SEL0_MASK	0x00030000	
#define A_EHC_P17V_SEL1_MASK	0x000c0000
#define A_EHC_P17V_SEL2_MASK	0x00300000
#define A_EHC_P17V_SEL3_MASK	0x00c00000

#define A_EHC_ASYNC_BYPASS	0x80000000

#define A_SRT3			0x77		
#define A_SRT4			0x78		
#define A_SRT5			0x79		


#define A_SRT_ESTSAMPLERATE	0x001fffff
#define A_SRT_RATELOCKED	0x01000000

#define A_TTDA			0x7a		
#define A_TTDD			0x7b		

// In A_FXRT1 & A_FXRT2, the 0x80 bit of each byte completely disables the
// filter (CVCF_CURRENTFILTER) for the corresponding channel. There is no
// effect on the volume (CVCF_CURRENTVOLUME) or the interpolator's filter
// (CCCA_INTERPROM_MASK).

#define A_FXRT2			0x7c
#define A_FXRT_CHANNELE		0x0000003f	
#define A_FXRT_CHANNELF		0x00003f00	
#define A_FXRT_CHANNELG		0x003f0000	
#define A_FXRT_CHANNELH		0x3f000000	

#define A_SENDAMOUNTS		0x7d
#define A_FXSENDAMOUNT_E_MASK	0xFF000000
#define A_FXSENDAMOUNT_F_MASK	0x00FF0000
#define A_FXSENDAMOUNT_G_MASK	0x0000FF00
#define A_FXSENDAMOUNT_H_MASK	0x000000FF


#define A_FXRT1			0x7e
#define A_FXRT_CHANNELA		0x0000003f
#define A_FXRT_CHANNELB		0x00003f00
#define A_FXRT_CHANNELC		0x003f0000
#define A_FXRT_CHANNELD		0x3f000000







#define MICROCODEBASE		0x400		
#define A_MICROCODEBASE		0x600






// - These cards use a regular PCI-attached Audigy chip (Alice2/Tina/Tina2);
//   the PCIe variants simply put the Audigy chip behind a PCI bridge.
// - All physical PCM I/O is routed through an additional FPGA; the regular
//   EXTIN/EXTOUT ports are unconnected.
// - The FPGA has a signal routing matrix, to connect each destination (output
//   socket or capture channel) to a source (input socket or playback channel).
// - The FPGA is controlled via Audigy's GPIO port, while sample data is
//   transmitted via proprietary EMU32 serial links. On first-generation
//   E-MU 1010 cards, Audigy's I2S inputs are also used for sample data.
// - The Audio/Micro Dock is attached to Hana via EDI, a "network" link.
// - The Audigy chip operates in slave mode; the clock is supplied by the FPGA.
//   Gen1 E-MU 1010 cards have two crystals (for 44.1 kHz and 48 kHz multiples),
//   while the later cards use a single crystal and a PLL chip.
// - The whole card is switched to 2x/4x mode to achieve 88.2/96/176.4/192 kHz
//   sample rates. Alice2/Tina keeps running at 44.1/48 kHz, but multiple channels
//   are bundled.
// - The number of available EMU32/EDI channels is hit in 2x/4x mode, so the total
//   number of usable inputs/outputs is limited, esp. with ADAT in use.
// - S/PDIF is unavailable in 4x mode (only over TOSLINK on newer 1010 cards) due
//   to being unspecified at 176.4/192 kHz. Therefore, the Dock's S/PDIF channels
//   can overlap with the Dock's ADC/DAC's high channels.
// - The code names are mentioned below and in the emu_chip_details table.





#define EMU_HANA_DESTHI		0x00	
#define EMU_HANA_DESTLO		0x01	

#define EMU_HANA_SRCHI		0x02	
#define EMU_HANA_SRCLO		0x03	

#define EMU_HANA_DOCK_PWR	0x04	
#define EMU_HANA_DOCK_PWR_ON		0x01 

#define EMU_HANA_WCLOCK		0x05	
					
					
#define EMU_HANA_WCLOCK_SRC_MASK	0x07
#define EMU_HANA_WCLOCK_INT_48K		0x00
#define EMU_HANA_WCLOCK_INT_44_1K	0x01
#define EMU_HANA_WCLOCK_HANA_SPDIF_IN	0x02
#define EMU_HANA_WCLOCK_HANA_ADAT_IN	0x03
#define EMU_HANA_WCLOCK_SYNC_BNC	0x04
#define EMU_HANA_WCLOCK_2ND_HANA	0x05
#define EMU_HANA_WCLOCK_SRC_RESERVED	0x06
#define EMU_HANA_WCLOCK_OFF		0x07 
#define EMU_HANA_WCLOCK_MULT_MASK	0x18
#define EMU_HANA_WCLOCK_1X		0x00
#define EMU_HANA_WCLOCK_2X		0x08
#define EMU_HANA_WCLOCK_4X		0x10
#define EMU_HANA_WCLOCK_MULT_RESERVED	0x18

// If the selected external clock source is/becomes invalid or incompatible
// with the clock multiplier, the clock source is reset to this value, and
// a WCLK_CHANGED interrupt is raised.
#define EMU_HANA_DEFCLOCK	0x06	
#define EMU_HANA_DEFCLOCK_48K		0x00
#define EMU_HANA_DEFCLOCK_44_1K		0x01

#define EMU_HANA_UNMUTE		0x07	
#define EMU_MUTE			0x00
#define EMU_UNMUTE			0x01

#define EMU_HANA_FPGA_CONFIG	0x08	
#define EMU_HANA_FPGA_CONFIG_AUDIODOCK	0x01 
#define EMU_HANA_FPGA_CONFIG_HANA	0x02 

#define EMU_HANA_IRQ_ENABLE	0x09	
#define EMU_HANA_IRQ_WCLK_CHANGED	0x01
#define EMU_HANA_IRQ_ADAT		0x02
#define EMU_HANA_IRQ_DOCK		0x04
#define EMU_HANA_IRQ_DOCK_LOST		0x08

#define EMU_HANA_SPDIF_MODE	0x0a	
#define EMU_HANA_SPDIF_MODE_TX_CONSUMER	0x00
#define EMU_HANA_SPDIF_MODE_TX_PRO	0x01
#define EMU_HANA_SPDIF_MODE_TX_NOCOPY	0x02
#define EMU_HANA_SPDIF_MODE_RX_CONSUMER	0x00
#define EMU_HANA_SPDIF_MODE_RX_PRO	0x04
#define EMU_HANA_SPDIF_MODE_RX_NOCOPY	0x08
#define EMU_HANA_SPDIF_MODE_RX_INVALID	0x10

#define EMU_HANA_OPTICAL_TYPE	0x0b	
#define EMU_HANA_OPTICAL_IN_SPDIF	0x00
#define EMU_HANA_OPTICAL_IN_ADAT	0x01
#define EMU_HANA_OPTICAL_OUT_SPDIF	0x00
#define EMU_HANA_OPTICAL_OUT_ADAT	0x02

#define EMU_HANA_MIDI_IN		0x0c	
#define EMU_HANA_MIDI_INA_FROM_HAMOA	0x01 
#define EMU_HANA_MIDI_INA_FROM_DOCK1	0x02 
#define EMU_HANA_MIDI_INA_FROM_DOCK2	0x03 
#define EMU_HANA_MIDI_INB_FROM_HAMOA	0x08 
#define EMU_HANA_MIDI_INB_FROM_DOCK1	0x10 
#define EMU_HANA_MIDI_INB_FROM_DOCK2	0x18 

#define EMU_HANA_DOCK_LEDS_1	0x0d	
#define EMU_HANA_DOCK_LEDS_1_MIDI1	0x01	
#define EMU_HANA_DOCK_LEDS_1_MIDI2	0x02	
#define EMU_HANA_DOCK_LEDS_1_SMPTE_IN	0x04	
#define EMU_HANA_DOCK_LEDS_1_SMPTE_OUT	0x08	

#define EMU_HANA_DOCK_LEDS_2	0x0e	
#define EMU_HANA_DOCK_LEDS_2_44K	0x01	
#define EMU_HANA_DOCK_LEDS_2_48K	0x02	
#define EMU_HANA_DOCK_LEDS_2_96K	0x04	
#define EMU_HANA_DOCK_LEDS_2_192K	0x08	
#define EMU_HANA_DOCK_LEDS_2_LOCK	0x10	
#define EMU_HANA_DOCK_LEDS_2_EXT	0x20	

#define EMU_HANA_DOCK_LEDS_3	0x0f	
#define EMU_HANA_DOCK_LEDS_3_CLIP_A	0x01	
#define EMU_HANA_DOCK_LEDS_3_CLIP_B	0x02	
#define EMU_HANA_DOCK_LEDS_3_SIGNAL_A	0x04	
#define EMU_HANA_DOCK_LEDS_3_SIGNAL_B	0x08	
#define EMU_HANA_DOCK_LEDS_3_MANUAL_CLIP	0x10	
#define EMU_HANA_DOCK_LEDS_3_MANUAL_SIGNAL	0x20	

#define EMU_HANA_ADC_PADS	0x10	
#define EMU_HANA_DOCK_ADC_PAD1		0x01	
#define EMU_HANA_DOCK_ADC_PAD2		0x02	
#define EMU_HANA_DOCK_ADC_PAD3		0x04	
#define EMU_HANA_0202_ADC_PAD1		0x08	

#define EMU_HANA_DOCK_MISC	0x11	
#define EMU_HANA_DOCK_DAC1_MUTE		0x01	
#define EMU_HANA_DOCK_DAC2_MUTE		0x02	
#define EMU_HANA_DOCK_DAC3_MUTE		0x04	
#define EMU_HANA_DOCK_DAC4_MUTE		0x08	
#define EMU_HANA_DOCK_PHONES_192_DAC1	0x00	
#define EMU_HANA_DOCK_PHONES_192_DAC2	0x10	
#define EMU_HANA_DOCK_PHONES_192_DAC3	0x20	
#define EMU_HANA_DOCK_PHONES_192_DAC4	0x30	

#define EMU_HANA_MIDI_OUT	0x12	
#define EMU_HANA_MIDI_OUT_0202		0x01 
#define EMU_HANA_MIDI_OUT_DOCK1		0x02 
#define EMU_HANA_MIDI_OUT_DOCK2		0x04 
#define EMU_HANA_MIDI_OUT_SYNC2		0x08 
#define EMU_HANA_MIDI_OUT_LOOP		0x10 

#define EMU_HANA_DAC_PADS	0x13	
#define EMU_HANA_DOCK_DAC_PAD1		0x01	
#define EMU_HANA_DOCK_DAC_PAD2		0x02	
#define EMU_HANA_DOCK_DAC_PAD3		0x04	
#define EMU_HANA_DOCK_DAC_PAD4		0x08	
#define EMU_HANA_0202_DAC_PAD1		0x10	



#define EMU_HANA_IRQ_STATUS	0x20	
					
					

#define EMU_HANA_OPTION_CARDS	0x21	
#define EMU_HANA_OPTION_HAMOA		0x01	
#define EMU_HANA_OPTION_SYNC		0x02	
#define EMU_HANA_OPTION_DOCK_ONLINE	0x04	
#define EMU_HANA_OPTION_DOCK_OFFLINE	0x08	

#define EMU_HANA_ID		0x22	
					

#define EMU_HANA_MAJOR_REV	0x23	
#define EMU_HANA_MINOR_REV	0x24	

#define EMU_DOCK_MAJOR_REV	0x25	
#define EMU_DOCK_MINOR_REV	0x26	

#define EMU_DOCK_BOARD_ID	0x27	
#define EMU_DOCK_BOARD_ID0		0x00	
#define EMU_DOCK_BOARD_ID1		0x03	

// The actual code disagrees about the bit width of the registers -
// the formula used is freq = 0x1770000 / (((X_HI << 5) | X_LO) + 1)

#define EMU_HANA_WC_SPDIF_HI	0x28	
#define EMU_HANA_WC_SPDIF_LO	0x29	

#define EMU_HANA_WC_ADAT_HI	0x2a	
#define EMU_HANA_WC_ADAT_LO	0x2b	

#define EMU_HANA_WC_BNC_LO	0x2c	
#define EMU_HANA_WC_BNC_HI	0x2d	

#define EMU_HANA2_WC_SPDIF_HI	0x2e	
#define EMU_HANA2_WC_SPDIF_LO	0x2f	



// The meaning of this is not clear; kX-project just calls it "lock" in some info-only code.
#define EMU_HANA_LOCK_STS_LO	0x38	
#define EMU_HANA_LOCK_STS_HI	0x39	







#define EMU_DST_ALICE2_EMU32_0	0x000f	
					
#define EMU_DST_ALICE2_EMU32_1	0x0000	
#define EMU_DST_ALICE2_EMU32_2	0x0001	
#define EMU_DST_ALICE2_EMU32_3	0x0002	
#define EMU_DST_ALICE2_EMU32_4	0x0003	
#define EMU_DST_ALICE2_EMU32_5	0x0004	
#define EMU_DST_ALICE2_EMU32_6	0x0005	
#define EMU_DST_ALICE2_EMU32_7	0x0006	
#define EMU_DST_ALICE2_EMU32_8	0x0007	
#define EMU_DST_ALICE2_EMU32_9	0x0008	
#define EMU_DST_ALICE2_EMU32_A	0x0009	
#define EMU_DST_ALICE2_EMU32_B	0x000a	
#define EMU_DST_ALICE2_EMU32_C	0x000b	
#define EMU_DST_ALICE2_EMU32_D	0x000c	
#define EMU_DST_ALICE2_EMU32_E	0x000d	
#define EMU_DST_ALICE2_EMU32_F	0x000e	
#define EMU_DST_DOCK_DAC1_LEFT1	0x0100	
#define EMU_DST_DOCK_DAC1_LEFT2	0x0101	
#define EMU_DST_DOCK_DAC1_LEFT3	0x0102	
#define EMU_DST_DOCK_DAC1_LEFT4	0x0103	
#define EMU_DST_DOCK_DAC1_RIGHT1	0x0104	
#define EMU_DST_DOCK_DAC1_RIGHT2	0x0105	
#define EMU_DST_DOCK_DAC1_RIGHT3	0x0106	
#define EMU_DST_DOCK_DAC1_RIGHT4	0x0107	
#define EMU_DST_DOCK_DAC2_LEFT1	0x0108	
#define EMU_DST_DOCK_DAC2_LEFT2	0x0109	
#define EMU_DST_DOCK_DAC2_LEFT3	0x010a	
#define EMU_DST_DOCK_DAC2_LEFT4	0x010b	
#define EMU_DST_DOCK_DAC2_RIGHT1	0x010c	
#define EMU_DST_DOCK_DAC2_RIGHT2	0x010d	
#define EMU_DST_DOCK_DAC2_RIGHT3	0x010e	
#define EMU_DST_DOCK_DAC2_RIGHT4	0x010f	
#define EMU_DST_DOCK_DAC3_LEFT1	0x0110	
#define EMU_DST_DOCK_DAC3_LEFT2	0x0111	
#define EMU_DST_DOCK_DAC3_LEFT3	0x0112	
#define EMU_DST_DOCK_DAC3_LEFT4	0x0113	
#define EMU_DST_DOCK_PHONES_LEFT1	0x0112	
#define EMU_DST_DOCK_PHONES_LEFT2	0x0113	
#define EMU_DST_DOCK_DAC3_RIGHT1	0x0114	
#define EMU_DST_DOCK_DAC3_RIGHT2	0x0115	
#define EMU_DST_DOCK_DAC3_RIGHT3	0x0116	
#define EMU_DST_DOCK_DAC3_RIGHT4	0x0117	
#define EMU_DST_DOCK_PHONES_RIGHT1	0x0116	
#define EMU_DST_DOCK_PHONES_RIGHT2	0x0117	
#define EMU_DST_DOCK_DAC4_LEFT1	0x0118	
#define EMU_DST_DOCK_DAC4_LEFT2	0x0119	
#define EMU_DST_DOCK_DAC4_LEFT3	0x011a	
#define EMU_DST_DOCK_DAC4_LEFT4	0x011b	
#define EMU_DST_DOCK_SPDIF_LEFT1	0x011a	
#define EMU_DST_DOCK_SPDIF_LEFT2	0x011b	
#define EMU_DST_DOCK_DAC4_RIGHT1	0x011c	
#define EMU_DST_DOCK_DAC4_RIGHT2	0x011d	
#define EMU_DST_DOCK_DAC4_RIGHT3	0x011e	
#define EMU_DST_DOCK_DAC4_RIGHT4	0x011f	
#define EMU_DST_DOCK_SPDIF_RIGHT1	0x011e	
#define EMU_DST_DOCK_SPDIF_RIGHT2	0x011f	
#define EMU_DST_HANA_SPDIF_LEFT1	0x0200	
#define EMU_DST_HANA_SPDIF_LEFT2	0x0202	
#define EMU_DST_HANA_SPDIF_LEFT3	0x0204	
#define EMU_DST_HANA_SPDIF_LEFT4	0x0206	
#define EMU_DST_HANA_SPDIF_RIGHT1	0x0201	
#define EMU_DST_HANA_SPDIF_RIGHT2	0x0203	
#define EMU_DST_HANA_SPDIF_RIGHT3	0x0205	
#define EMU_DST_HANA_SPDIF_RIGHT4	0x0207	
#define EMU_DST_HAMOA_DAC_LEFT1	0x0300	
#define EMU_DST_HAMOA_DAC_LEFT2	0x0302	
#define EMU_DST_HAMOA_DAC_LEFT3	0x0304	
#define EMU_DST_HAMOA_DAC_LEFT4	0x0306	
#define EMU_DST_HAMOA_DAC_RIGHT1	0x0301	
#define EMU_DST_HAMOA_DAC_RIGHT2	0x0303	
#define EMU_DST_HAMOA_DAC_RIGHT3	0x0305	
#define EMU_DST_HAMOA_DAC_RIGHT4	0x0307	
// In S/MUX mode, the samples of one channel are adjacent.
#define EMU_DST_HANA_ADAT	0x0400	
#define EMU_DST_ALICE_I2S0_LEFT		0x0500	
#define EMU_DST_ALICE_I2S0_RIGHT	0x0501	
#define EMU_DST_ALICE_I2S1_LEFT		0x0600	
#define EMU_DST_ALICE_I2S1_RIGHT	0x0601	
#define EMU_DST_ALICE_I2S2_LEFT		0x0700	
#define EMU_DST_ALICE_I2S2_RIGHT	0x0701	



#define EMU_DST_MDOCK_SPDIF_LEFT1	0x0112	
#define EMU_DST_MDOCK_SPDIF_LEFT2	0x0113	
#define EMU_DST_MDOCK_SPDIF_RIGHT1	0x0116	
#define EMU_DST_MDOCK_SPDIF_RIGHT2	0x0117	
#define EMU_DST_MDOCK_ADAT		0x0118	

#define EMU_DST_MANA_DAC_LEFT		0x0300	
#define EMU_DST_MANA_DAC_RIGHT		0x0301	







#define EMU_SRC_SILENCE		0x0000	
#define EMU_SRC_DOCK_MIC_A1	0x0100	
#define EMU_SRC_DOCK_MIC_A2	0x0101	
#define EMU_SRC_DOCK_MIC_A3	0x0102	
#define EMU_SRC_DOCK_MIC_A4	0x0103	
#define EMU_SRC_DOCK_MIC_B1	0x0104	
#define EMU_SRC_DOCK_MIC_B2	0x0105	
#define EMU_SRC_DOCK_MIC_B3	0x0106	
#define EMU_SRC_DOCK_MIC_B4	0x0107	
#define EMU_SRC_DOCK_ADC1_LEFT1	0x0108	
#define EMU_SRC_DOCK_ADC1_LEFT2	0x0109	
#define EMU_SRC_DOCK_ADC1_LEFT3	0x010a	
#define EMU_SRC_DOCK_ADC1_LEFT4	0x010b	
#define EMU_SRC_DOCK_ADC1_RIGHT1	0x010c	
#define EMU_SRC_DOCK_ADC1_RIGHT2	0x010d	
#define EMU_SRC_DOCK_ADC1_RIGHT3	0x010e	
#define EMU_SRC_DOCK_ADC1_RIGHT4	0x010f	
#define EMU_SRC_DOCK_ADC2_LEFT1	0x0110	
#define EMU_SRC_DOCK_ADC2_LEFT2	0x0111	
#define EMU_SRC_DOCK_ADC2_LEFT3	0x0112	
#define EMU_SRC_DOCK_ADC2_LEFT4	0x0113	
#define EMU_SRC_DOCK_ADC2_RIGHT1	0x0114	
#define EMU_SRC_DOCK_ADC2_RIGHT2	0x0115	
#define EMU_SRC_DOCK_ADC2_RIGHT3	0x0116	
#define EMU_SRC_DOCK_ADC2_RIGHT4	0x0117	
#define EMU_SRC_DOCK_ADC3_LEFT1	0x0118	
#define EMU_SRC_DOCK_ADC3_LEFT2	0x0119	
#define EMU_SRC_DOCK_ADC3_LEFT3	0x011a	
#define EMU_SRC_DOCK_ADC3_LEFT4	0x011b	
#define EMU_SRC_DOCK_ADC3_RIGHT1	0x011c	
#define EMU_SRC_DOCK_ADC3_RIGHT2	0x011d	
#define EMU_SRC_DOCK_ADC3_RIGHT3	0x011e	
#define EMU_SRC_DOCK_ADC3_RIGHT4	0x011f	
#define EMU_SRC_HAMOA_ADC_LEFT1	0x0200	
#define EMU_SRC_HAMOA_ADC_LEFT2	0x0202	
#define EMU_SRC_HAMOA_ADC_LEFT3	0x0204	
#define EMU_SRC_HAMOA_ADC_LEFT4	0x0206	
#define EMU_SRC_HAMOA_ADC_RIGHT1	0x0201	
#define EMU_SRC_HAMOA_ADC_RIGHT2	0x0203	
#define EMU_SRC_HAMOA_ADC_RIGHT3	0x0205	
#define EMU_SRC_HAMOA_ADC_RIGHT4	0x0207	
#define EMU_SRC_ALICE_EMU32A		0x0300	
#define EMU_SRC_ALICE_EMU32B		0x0310	
// In S/MUX mode, the samples of one channel are adjacent.
#define EMU_SRC_HANA_ADAT	0x0400	
#define EMU_SRC_HANA_SPDIF_LEFT1	0x0500	
#define EMU_SRC_HANA_SPDIF_LEFT2	0x0502	
#define EMU_SRC_HANA_SPDIF_LEFT3	0x0504	
#define EMU_SRC_HANA_SPDIF_LEFT4	0x0506	
#define EMU_SRC_HANA_SPDIF_RIGHT1	0x0501	
#define EMU_SRC_HANA_SPDIF_RIGHT2	0x0503	
#define EMU_SRC_HANA_SPDIF_RIGHT3	0x0505	
#define EMU_SRC_HANA_SPDIF_RIGHT4	0x0507	



#define EMU_SRC_MDOCK_SPDIF_LEFT1	0x0112	
#define EMU_SRC_MDOCK_SPDIF_LEFT2	0x0113	
#define EMU_SRC_MDOCK_SPDIF_RIGHT1	0x0116	
#define EMU_SRC_MDOCK_SPDIF_RIGHT2	0x0117	
#define EMU_SRC_MDOCK_ADAT		0x0118	






extern const char * const snd_emu10k1_fxbus[32];
extern const char * const snd_emu10k1_sblive_ins[16];
extern const char * const snd_emu10k1_audigy_ins[16];
extern const char * const snd_emu10k1_sblive_outs[32];
extern const char * const snd_emu10k1_audigy_outs[32];
extern const s8 snd_emu10k1_sblive51_fxbus2_map[16];



enum {
	EMU10K1_UNUSED,  // This must be zero
	EMU10K1_EFX,
	EMU10K1_EFX_IRQ,
	EMU10K1_PCM,
	EMU10K1_PCM_IRQ,
	EMU10K1_SYNTH,
	EMU10K1_NUM_TYPES
};

struct snd_emu10k1;

struct snd_emu10k1_voice {
	unsigned char number;
	unsigned char use;
	unsigned char dirty;
	unsigned char last;
	void (*interrupt)(struct snd_emu10k1 *emu, struct snd_emu10k1_voice *pvoice);

	struct snd_emu10k1_pcm *epcm;
};

enum {
	PLAYBACK_EMUVOICE,
	PLAYBACK_EFX,
	CAPTURE_AC97ADC,
	CAPTURE_AC97MIC,
	CAPTURE_EFX
};

struct snd_emu10k1_pcm {
	struct snd_emu10k1 *emu;
	int type;
	struct snd_pcm_substream *substream;
	struct snd_emu10k1_voice *voices[NUM_EFX_PLAYBACK];
	struct snd_emu10k1_voice *extra;
	unsigned short running;
	unsigned short first_ptr;
	snd_pcm_uframes_t resume_pos;
	struct snd_util_memblk *memblk;
	unsigned int pitch_target;
	unsigned int start_addr;
	unsigned int ccca_start_addr;
	unsigned int capture_ipr;	
	unsigned int capture_inte;	
	unsigned int capture_ba_reg;	
	unsigned int capture_bs_reg;	
	unsigned int capture_idx_reg;	
	unsigned int capture_cr_val;	
	unsigned int capture_cr_val2;	
	unsigned int capture_bs_val;	
	unsigned int capture_bufsize;	
};

struct snd_emu10k1_pcm_mixer {
	
	unsigned char send_routing[3][8];
	unsigned char send_volume[3][8];
	// 0x8000 is neutral. The mixer code rescales it to 0xffff to maintain
	// backwards compatibility with user space.
	unsigned short attn[3];
	struct snd_emu10k1_pcm *epcm;
};

#define snd_emu10k1_compose_send_routing(route) \
((route[0] | (route[1] << 4) | (route[2] << 8) | (route[3] << 12)) << 16)

#define snd_emu10k1_compose_audigy_fxrt1(route) \
((unsigned int)route[0] | ((unsigned int)route[1] << 8) | ((unsigned int)route[2] << 16) | ((unsigned int)route[3] << 24) | 0x80808080)

#define snd_emu10k1_compose_audigy_fxrt2(route) \
((unsigned int)route[4] | ((unsigned int)route[5] << 8) | ((unsigned int)route[6] << 16) | ((unsigned int)route[7] << 24) | 0x80808080)

#define snd_emu10k1_compose_audigy_sendamounts(vol) \
(((unsigned int)vol[4] << 24) | ((unsigned int)vol[5] << 16) | ((unsigned int)vol[6] << 8) | (unsigned int)vol[7])

struct snd_emu10k1_memblk {
	struct snd_util_memblk mem;
	
	int first_page, last_page, pages, mapped_page;
	unsigned int map_locked;
	struct list_head mapped_link;
	struct list_head mapped_order_link;
};

#define snd_emu10k1_memblk_offset(blk)	(((blk)->mapped_page << PAGE_SHIFT) | ((blk)->mem.offset & (PAGE_SIZE - 1)))

#define EMU10K1_MAX_TRAM_BLOCKS_PER_CODE	16

struct snd_emu10k1_fx8010_ctl {
	struct list_head list;		
	unsigned int vcount;
	unsigned int count;		
	unsigned short gpr[32];		
	int value[32];
	int min;			
	int max;			
	unsigned int translation;	
	struct snd_kcontrol *kcontrol;
};

typedef void (snd_fx8010_irq_handler_t)(struct snd_emu10k1 *emu, void *private_data);

struct snd_emu10k1_fx8010_irq {
	struct snd_emu10k1_fx8010_irq *next;
	snd_fx8010_irq_handler_t *handler;
	unsigned short gpr_running;
	void *private_data;
};

struct snd_emu10k1_fx8010_pcm {
	unsigned int valid: 1,
		     opened: 1,
		     active: 1;
	unsigned int channels;		
	unsigned int tram_start;	
	unsigned int buffer_size;	
	unsigned short gpr_size;		
	unsigned short gpr_ptr;		
	unsigned short gpr_count;	
	unsigned short gpr_tmpcount;	
	unsigned short gpr_trigger;	
	unsigned short gpr_running;	
	unsigned char etram[32];	
	struct snd_pcm_indirect pcm_rec;
	unsigned int tram_pos;
	unsigned int tram_shift;
	struct snd_emu10k1_fx8010_irq irq;
};

struct snd_emu10k1_fx8010 {
	unsigned short extin_mask;	
	unsigned short extout_mask;	
	unsigned int itram_size;	
	struct snd_dma_buffer etram_pages; 
	unsigned int dbg;		
	unsigned char name[128];
	int gpr_size;			
	int gpr_count;			
	struct list_head gpr_ctl;	
	struct mutex lock;
	struct snd_emu10k1_fx8010_pcm pcm[8];
	spinlock_t irq_lock;
	struct snd_emu10k1_fx8010_irq *irq_handlers;
};

struct snd_emu10k1_midi {
	struct snd_emu10k1 *emu;
	struct snd_rawmidi *rmidi;
	struct snd_rawmidi_substream *substream_input;
	struct snd_rawmidi_substream *substream_output;
	unsigned int midi_mode;
	spinlock_t input_lock;
	spinlock_t output_lock;
	spinlock_t open_lock;
	int tx_enable, rx_enable;
	int port;
	int ipr_tx, ipr_rx;
	void (*interrupt)(struct snd_emu10k1 *emu, unsigned int status);
};

enum {
	EMU_MODEL_SB,
	EMU_MODEL_EMU1010,
	EMU_MODEL_EMU1010B,
	EMU_MODEL_EMU1616,
	EMU_MODEL_EMU0404,
};

// Chip-o-logy:
// - All SB Live! cards use EMU10K1 chips
// - All SB Audigy cards use CA* chips, termed "emu10k2" by the driver
// - Original Audigy uses CA0100 "Alice"
// - Audigy 2 uses CA0102/CA10200 "Alice2"
//   - Has an interface for CA0151 (P16V) "Alice3"
// - Audigy 2 Value uses CA0108/CA10300 "Tina"
//   - Approximately a CA0102 with an on-chip CA0151 (P17V)
// - Audigy 2 ZS NB uses CA0109 "Tina2"
//   - Cardbus version of CA0108
struct snd_emu_chip_details {
	u32 vendor;
	u32 device;
	u32 subsystem;
	unsigned char revision;
	unsigned char emu_model;	
	unsigned int emu10k1_chip:1;	
					
	unsigned int emu10k2_chip:1;	
	unsigned int ca0102_chip:1;	
					
	unsigned int ca0108_chip:1;	
	unsigned int ca_cardbus_chip:1;	
	unsigned int ca0151_chip:1;	
	unsigned int spk20:1;		
	unsigned int spk71:1;		
	unsigned int no_adat:1;		
	unsigned int sblive51:1;	
	unsigned int spdif_bug:1;	
	unsigned int ac97_chip:2;	
	unsigned int ecard:1;		
	unsigned int spi_dac:1;		
	unsigned int i2c_adc:1;		
	unsigned int adc_1361t:1;	
	unsigned int invert_shared_spdif:1;  
	const char *driver;
	const char *name;
	const char *id;		
};

#define NUM_OUTPUT_DESTS 28
#define NUM_INPUT_DESTS 22

struct snd_emu1010 {
	unsigned char output_source[NUM_OUTPUT_DESTS];
	unsigned char input_source[NUM_INPUT_DESTS];
	unsigned int adc_pads; 
	unsigned int dac_pads; 
	unsigned int wclock;  
	unsigned int word_clock;  
	unsigned int clock_source;
	unsigned int clock_fallback;
	unsigned int optical_in; 
	unsigned int optical_out; 
	struct work_struct work;
};

struct snd_emu10k1 {
	int irq;

	unsigned long port;			
	unsigned int tos_link: 1,		
		rear_ac97: 1,			
		enable_ir: 1;
	unsigned int support_tlv :1;
	
	const struct snd_emu_chip_details *card_capabilities;
	unsigned int audigy;			
	unsigned int revision;			
	unsigned int serial;			
	unsigned short model;			
	unsigned int ecard_ctrl;		
	unsigned int address_mode;		
	unsigned long dma_mask;			
	bool iommu_workaround;			
	int max_cache_pages;			
	struct snd_dma_buffer silent_page;	
	struct snd_dma_buffer ptb_pages;	
	struct snd_dma_device p16v_dma_dev;
	struct snd_dma_buffer *p16v_buffer;

	struct snd_util_memhdr *memhdr;		

	struct list_head mapped_link_head;
	struct list_head mapped_order_link_head;
	void **page_ptr_table;
	unsigned long *page_addr_table;
	spinlock_t memblk_lock;

	unsigned int spdif_bits[3];		
	unsigned int i2c_capture_source;
	u8 i2c_capture_volume[4][2];

	struct snd_emu10k1_fx8010 fx8010;		
	int gpr_base;
	
	struct snd_ac97 *ac97;

	struct pci_dev *pci;
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct snd_pcm *pcm_mic;
	struct snd_pcm *pcm_efx;
	struct snd_pcm *pcm_multi;
	struct snd_pcm *pcm_p16v;

	spinlock_t synth_lock;
	void *synth;
	int (*get_synth_voice)(struct snd_emu10k1 *emu);

	spinlock_t reg_lock;  // high-level driver lock
	spinlock_t emu_lock;  // low-level i/o lock
	spinlock_t voice_lock;  // voice allocator lock
	spinlock_t spi_lock; 
	spinlock_t i2c_lock; 

	struct snd_emu10k1_voice voices[NUM_G];
	int p16v_device_offset;
	u32 p16v_capture_source;
	u32 p16v_capture_channel;
        struct snd_emu1010 emu1010;
	struct snd_emu10k1_pcm_mixer pcm_mixer[32];
	struct snd_emu10k1_pcm_mixer efx_pcm_mixer[NUM_EFX_PLAYBACK];
	struct snd_kcontrol *ctl_send_routing;
	struct snd_kcontrol *ctl_send_volume;
	struct snd_kcontrol *ctl_attn;
	struct snd_kcontrol *ctl_efx_send_routing;
	struct snd_kcontrol *ctl_efx_send_volume;
	struct snd_kcontrol *ctl_efx_attn;
	struct snd_kcontrol *ctl_clock_source;

	void (*hwvol_interrupt)(struct snd_emu10k1 *emu, unsigned int status);
	void (*capture_interrupt)(struct snd_emu10k1 *emu, unsigned int status);
	void (*capture_mic_interrupt)(struct snd_emu10k1 *emu, unsigned int status);
	void (*capture_efx_interrupt)(struct snd_emu10k1 *emu, unsigned int status);
	void (*spdif_interrupt)(struct snd_emu10k1 *emu, unsigned int status);
	void (*dsp_interrupt)(struct snd_emu10k1 *emu);
	void (*gpio_interrupt)(struct snd_emu10k1 *emu);
	void (*p16v_interrupt)(struct snd_emu10k1 *emu);

	struct snd_pcm_substream *pcm_capture_substream;
	struct snd_pcm_substream *pcm_capture_mic_substream;
	struct snd_pcm_substream *pcm_capture_efx_substream;

	struct snd_timer *timer;

	struct snd_emu10k1_midi midi;
	struct snd_emu10k1_midi midi2; 

	unsigned int efx_voices_mask[2];
	unsigned int next_free_voice;

	const struct firmware *firmware;
	const struct firmware *dock_fw;

#ifdef CONFIG_PM_SLEEP
	unsigned int *saved_ptr;
	unsigned int *saved_gpr;
	unsigned int *tram_val_saved;
	unsigned int *tram_addr_saved;
	unsigned int *saved_icode;
	unsigned int *p16v_saved;
	unsigned int saved_a_iocfg, saved_hcfg;
	bool suspend;
#endif

};

int snd_emu10k1_create(struct snd_card *card,
		       struct pci_dev *pci,
		       unsigned short extin_mask,
		       unsigned short extout_mask,
		       long max_cache_bytes,
		       int enable_ir,
		       uint subsystem);

int snd_emu10k1_pcm(struct snd_emu10k1 *emu, int device);
int snd_emu10k1_pcm_mic(struct snd_emu10k1 *emu, int device);
int snd_emu10k1_pcm_efx(struct snd_emu10k1 *emu, int device);
int snd_p16v_pcm(struct snd_emu10k1 *emu, int device);
int snd_p16v_mixer(struct snd_emu10k1 * emu);
int snd_emu10k1_pcm_multi(struct snd_emu10k1 *emu, int device);
int snd_emu10k1_fx8010_pcm(struct snd_emu10k1 *emu, int device);
int snd_emu10k1_mixer(struct snd_emu10k1 * emu, int pcm_device, int multi_device);
int snd_emu10k1_timer(struct snd_emu10k1 * emu, int device);
int snd_emu10k1_fx8010_new(struct snd_emu10k1 *emu, int device);

irqreturn_t snd_emu10k1_interrupt(int irq, void *dev_id);

void snd_emu10k1_voice_init(struct snd_emu10k1 * emu, int voice);
int snd_emu10k1_init_efx(struct snd_emu10k1 *emu);
void snd_emu10k1_free_efx(struct snd_emu10k1 *emu);
int snd_emu10k1_fx8010_tram_setup(struct snd_emu10k1 *emu, u32 size);
int snd_emu10k1_done(struct snd_emu10k1 * emu);


unsigned int snd_emu10k1_ptr_read(struct snd_emu10k1 * emu, unsigned int reg, unsigned int chn);
void snd_emu10k1_ptr_write(struct snd_emu10k1 *emu, unsigned int reg, unsigned int chn, unsigned int data);
void snd_emu10k1_ptr_write_multiple(struct snd_emu10k1 *emu, unsigned int chn, ...);
unsigned int snd_emu10k1_ptr20_read(struct snd_emu10k1 * emu, unsigned int reg, unsigned int chn);
void snd_emu10k1_ptr20_write(struct snd_emu10k1 *emu, unsigned int reg, unsigned int chn, unsigned int data);
int snd_emu10k1_spi_write(struct snd_emu10k1 * emu, unsigned int data);
int snd_emu10k1_i2c_write(struct snd_emu10k1 *emu, u32 reg, u32 value);
void snd_emu1010_fpga_write(struct snd_emu10k1 *emu, u32 reg, u32 value);
void snd_emu1010_fpga_read(struct snd_emu10k1 *emu, u32 reg, u32 *value);
void snd_emu1010_fpga_link_dst_src_write(struct snd_emu10k1 *emu, u32 dst, u32 src);
u32 snd_emu1010_fpga_link_dst_src_read(struct snd_emu10k1 *emu, u32 dst);
int snd_emu1010_get_raw_rate(struct snd_emu10k1 *emu, u8 src);
void snd_emu1010_update_clock(struct snd_emu10k1 *emu);
unsigned int snd_emu10k1_efx_read(struct snd_emu10k1 *emu, unsigned int pc);
void snd_emu10k1_intr_enable(struct snd_emu10k1 *emu, unsigned int intrenb);
void snd_emu10k1_intr_disable(struct snd_emu10k1 *emu, unsigned int intrenb);
void snd_emu10k1_voice_intr_enable(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_intr_disable(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_intr_ack(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_half_loop_intr_enable(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_half_loop_intr_disable(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_half_loop_intr_ack(struct snd_emu10k1 *emu, unsigned int voicenum);
#if 0
void snd_emu10k1_voice_set_loop_stop(struct snd_emu10k1 *emu, unsigned int voicenum);
void snd_emu10k1_voice_clear_loop_stop(struct snd_emu10k1 *emu, unsigned int voicenum);
#endif
void snd_emu10k1_voice_set_loop_stop_multiple(struct snd_emu10k1 *emu, u64 voices);
void snd_emu10k1_voice_clear_loop_stop_multiple(struct snd_emu10k1 *emu, u64 voices);
int snd_emu10k1_voice_clear_loop_stop_multiple_atomic(struct snd_emu10k1 *emu, u64 voices);
void snd_emu10k1_wait(struct snd_emu10k1 *emu, unsigned int wait);
static inline unsigned int snd_emu10k1_wc(struct snd_emu10k1 *emu) { return (inl(emu->port + WC) >> 6) & 0xfffff; }
unsigned short snd_emu10k1_ac97_read(struct snd_ac97 *ac97, unsigned short reg);
void snd_emu10k1_ac97_write(struct snd_ac97 *ac97, unsigned short reg, unsigned short data);

#ifdef CONFIG_PM_SLEEP
void snd_emu10k1_suspend_regs(struct snd_emu10k1 *emu);
void snd_emu10k1_resume_init(struct snd_emu10k1 *emu);
void snd_emu10k1_resume_regs(struct snd_emu10k1 *emu);
int snd_emu10k1_efx_alloc_pm_buffer(struct snd_emu10k1 *emu);
void snd_emu10k1_efx_free_pm_buffer(struct snd_emu10k1 *emu);
void snd_emu10k1_efx_suspend(struct snd_emu10k1 *emu);
void snd_emu10k1_efx_resume(struct snd_emu10k1 *emu);
int snd_p16v_alloc_pm_buffer(struct snd_emu10k1 *emu);
void snd_p16v_free_pm_buffer(struct snd_emu10k1 *emu);
void snd_p16v_suspend(struct snd_emu10k1 *emu);
void snd_p16v_resume(struct snd_emu10k1 *emu);
#endif


struct snd_util_memblk *snd_emu10k1_alloc_pages(struct snd_emu10k1 *emu, struct snd_pcm_substream *substream);
int snd_emu10k1_free_pages(struct snd_emu10k1 *emu, struct snd_util_memblk *blk);
int snd_emu10k1_alloc_pages_maybe_wider(struct snd_emu10k1 *emu, size_t size,
					struct snd_dma_buffer *dmab);
struct snd_util_memblk *snd_emu10k1_synth_alloc(struct snd_emu10k1 *emu, unsigned int size);
int snd_emu10k1_synth_free(struct snd_emu10k1 *emu, struct snd_util_memblk *blk);
int snd_emu10k1_synth_bzero(struct snd_emu10k1 *emu, struct snd_util_memblk *blk, int offset, int size);
int snd_emu10k1_synth_copy_from_user(struct snd_emu10k1 *emu, struct snd_util_memblk *blk, int offset, const char __user *data, int size);
int snd_emu10k1_memblk_map(struct snd_emu10k1 *emu, struct snd_emu10k1_memblk *blk);


int snd_emu10k1_voice_alloc(struct snd_emu10k1 *emu, int type, int count, int channels,
			    struct snd_emu10k1_pcm *epcm, struct snd_emu10k1_voice **rvoice);
int snd_emu10k1_voice_free(struct snd_emu10k1 *emu, struct snd_emu10k1_voice *pvoice);


int snd_emu10k1_midi(struct snd_emu10k1 * emu);
int snd_emu10k1_audigy_midi(struct snd_emu10k1 * emu);


int snd_emu10k1_proc_init(struct snd_emu10k1 * emu);


int snd_emu10k1_fx8010_register_irq_handler(struct snd_emu10k1 *emu,
					    snd_fx8010_irq_handler_t *handler,
					    unsigned char gpr_running,
					    void *private_data,
					    struct snd_emu10k1_fx8010_irq *irq);
int snd_emu10k1_fx8010_unregister_irq_handler(struct snd_emu10k1 *emu,
					      struct snd_emu10k1_fx8010_irq *irq);

#endif	
