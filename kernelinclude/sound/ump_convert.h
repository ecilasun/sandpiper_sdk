// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef __SOUND_UMP_CONVERT_H
#define __SOUND_UMP_CONVERT_H

#include <sound/ump_msg.h>


struct ump_cvt_to_ump_bank {
	bool rpn_set;
	bool nrpn_set;
	bool bank_set;
	unsigned char cc_rpn_msb, cc_rpn_lsb;
	unsigned char cc_nrpn_msb, cc_nrpn_lsb;
	unsigned char cc_data_msb, cc_data_lsb;
	unsigned char cc_bank_msb, cc_bank_lsb;
};


struct ump_cvt_to_ump {
	
	unsigned char buf[4];
	int len;
	int cmd_bytes;

	
	u32 ump[4];
	int ump_bytes;

	
	unsigned int in_sysex;
	struct ump_cvt_to_ump_bank bank[16];	
};

int snd_ump_convert_from_ump(const u32 *data, unsigned char *dst,
			     unsigned char *group_ret);
void snd_ump_convert_to_ump(struct ump_cvt_to_ump *cvt, unsigned char group,
			    unsigned int protocol, unsigned char c);


static inline void snd_ump_convert_reset(struct ump_cvt_to_ump *ctx)
{
	memset(ctx, 0, sizeof(*ctx));

}

#endif 
