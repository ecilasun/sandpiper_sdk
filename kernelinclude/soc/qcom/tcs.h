/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __SOC_QCOM_TCS_H__
#define __SOC_QCOM_TCS_H__

#define MAX_RPMH_PAYLOAD	16


enum rpmh_state {
	RPMH_SLEEP_STATE,
	RPMH_WAKE_ONLY_STATE,
	RPMH_ACTIVE_ONLY_STATE,
};


struct tcs_cmd {
	u32 addr;
	u32 data;
	u32 wait;
};


struct tcs_request {
	enum rpmh_state state;
	u32 wait_for_compl;
	u32 num_cmds;
	struct tcs_cmd *cmds;
};

#define BCM_TCS_CMD_COMMIT_SHFT		30
#define BCM_TCS_CMD_COMMIT_MASK		0x40000000
#define BCM_TCS_CMD_VALID_SHFT		29
#define BCM_TCS_CMD_VALID_MASK		0x20000000
#define BCM_TCS_CMD_VOTE_X_SHFT		14
#define BCM_TCS_CMD_VOTE_MASK		0x3fff
#define BCM_TCS_CMD_VOTE_Y_SHFT		0
#define BCM_TCS_CMD_VOTE_Y_MASK		0xfffc000


#define BCM_TCS_CMD(commit, valid, vote_x, vote_y)		\
	(((commit) << BCM_TCS_CMD_COMMIT_SHFT) |		\
	((valid) << BCM_TCS_CMD_VALID_SHFT) |			\
	((cpu_to_le32(vote_x) &					\
	BCM_TCS_CMD_VOTE_MASK) << BCM_TCS_CMD_VOTE_X_SHFT) |	\
	((cpu_to_le32(vote_y) &					\
	BCM_TCS_CMD_VOTE_MASK) << BCM_TCS_CMD_VOTE_Y_SHFT))

#endif 
