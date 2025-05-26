/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CS_DSP_H
#define __CS_DSP_H

#include <linux/bits.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/list.h>
#include <linux/regmap.h>

#define CS_ADSP2_REGION_0 BIT(0)
#define CS_ADSP2_REGION_1 BIT(1)
#define CS_ADSP2_REGION_2 BIT(2)
#define CS_ADSP2_REGION_3 BIT(3)
#define CS_ADSP2_REGION_4 BIT(4)
#define CS_ADSP2_REGION_5 BIT(5)
#define CS_ADSP2_REGION_6 BIT(6)
#define CS_ADSP2_REGION_7 BIT(7)
#define CS_ADSP2_REGION_8 BIT(8)
#define CS_ADSP2_REGION_9 BIT(9)
#define CS_ADSP2_REGION_1_9 (CS_ADSP2_REGION_1 | \
		CS_ADSP2_REGION_2 | CS_ADSP2_REGION_3 | \
		CS_ADSP2_REGION_4 | CS_ADSP2_REGION_5 | \
		CS_ADSP2_REGION_6 | CS_ADSP2_REGION_7 | \
		CS_ADSP2_REGION_8 | CS_ADSP2_REGION_9)
#define CS_ADSP2_REGION_ALL (CS_ADSP2_REGION_0 | CS_ADSP2_REGION_1_9)

#define CS_DSP_DATA_WORD_SIZE                3
#define CS_DSP_DATA_WORD_BITS                (3 * BITS_PER_BYTE)

#define CS_DSP_ACKED_CTL_TIMEOUT_MS          100
#define CS_DSP_ACKED_CTL_N_QUICKPOLLS        10
#define CS_DSP_ACKED_CTL_MIN_VALUE           0
#define CS_DSP_ACKED_CTL_MAX_VALUE           0xFFFFFF


struct cs_dsp_region {
	int type;
	unsigned int base;
};


struct cs_dsp_alg_region {
	struct list_head list;
	unsigned int alg;
	unsigned int ver;
	int type;
	unsigned int base;
};


struct cs_dsp_coeff_ctl {
	struct list_head list;
	struct cs_dsp *dsp;
	void *cache;
	const char *fw_name;
	
	const char *subname;
	unsigned int subname_len;
	unsigned int offset;
	size_t len;
	unsigned int type;
	unsigned int flags;
	unsigned int set:1;
	unsigned int enabled:1;
	struct cs_dsp_alg_region alg_region;

	void *priv;
};

struct cs_dsp_ops;
struct cs_dsp_client_ops;


struct cs_dsp {
	const char *name;
	int rev;
	int num;
	int type;
	struct device *dev;
	struct regmap *regmap;

	const struct cs_dsp_ops *ops;
	const struct cs_dsp_client_ops *client_ops;

	unsigned int base;
	unsigned int base_sysinfo;
	unsigned int sysclk_reg;
	unsigned int sysclk_mask;
	unsigned int sysclk_shift;
	bool no_core_startstop;

	struct list_head alg_regions;

	const char *fw_name;
	unsigned int fw_id;
	unsigned int fw_id_version;
	unsigned int fw_vendor_id;

	const struct cs_dsp_region *mem;
	int num_mems;

	int fw_ver;

	bool booted;
	bool running;

	struct list_head ctl_list;

	struct mutex pwr_lock;

	unsigned int lock_regions;

#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_root;
	char *wmfw_file_name;
	char *bin_file_name;
#endif
};


struct cs_dsp_client_ops {
	int (*control_add)(struct cs_dsp_coeff_ctl *ctl);
	void (*control_remove)(struct cs_dsp_coeff_ctl *ctl);
	int (*pre_run)(struct cs_dsp *dsp);
	int (*post_run)(struct cs_dsp *dsp);
	void (*pre_stop)(struct cs_dsp *dsp);
	void (*post_stop)(struct cs_dsp *dsp);
	void (*watchdog_expired)(struct cs_dsp *dsp);
};

int cs_dsp_adsp1_init(struct cs_dsp *dsp);
int cs_dsp_adsp2_init(struct cs_dsp *dsp);
int cs_dsp_halo_init(struct cs_dsp *dsp);

int cs_dsp_adsp1_power_up(struct cs_dsp *dsp,
			  const struct firmware *wmfw_firmware, char *wmfw_filename,
			  const struct firmware *coeff_firmware, char *coeff_filename,
			  const char *fw_name);
void cs_dsp_adsp1_power_down(struct cs_dsp *dsp);
int cs_dsp_power_up(struct cs_dsp *dsp,
		    const struct firmware *wmfw_firmware, char *wmfw_filename,
		    const struct firmware *coeff_firmware, char *coeff_filename,
		    const char *fw_name);
void cs_dsp_power_down(struct cs_dsp *dsp);
int cs_dsp_run(struct cs_dsp *dsp);
void cs_dsp_stop(struct cs_dsp *dsp);

void cs_dsp_remove(struct cs_dsp *dsp);

int cs_dsp_set_dspclk(struct cs_dsp *dsp, unsigned int freq);
void cs_dsp_adsp2_bus_error(struct cs_dsp *dsp);
void cs_dsp_halo_bus_error(struct cs_dsp *dsp);
void cs_dsp_halo_wdt_expire(struct cs_dsp *dsp);

void cs_dsp_init_debugfs(struct cs_dsp *dsp, struct dentry *debugfs_root);
void cs_dsp_cleanup_debugfs(struct cs_dsp *dsp);

int cs_dsp_coeff_write_acked_control(struct cs_dsp_coeff_ctl *ctl, unsigned int event_id);
int cs_dsp_coeff_write_ctrl(struct cs_dsp_coeff_ctl *ctl, unsigned int off,
			    const void *buf, size_t len);
int cs_dsp_coeff_read_ctrl(struct cs_dsp_coeff_ctl *ctl, unsigned int off,
			   void *buf, size_t len);
struct cs_dsp_coeff_ctl *cs_dsp_get_ctl(struct cs_dsp *dsp, const char *name, int type,
					unsigned int alg);

int cs_dsp_read_raw_data_block(struct cs_dsp *dsp, int mem_type, unsigned int mem_addr,
			       unsigned int num_words, __be32 *data);
int cs_dsp_read_data_word(struct cs_dsp *dsp, int mem_type, unsigned int mem_addr, u32 *data);
int cs_dsp_write_data_word(struct cs_dsp *dsp, int mem_type, unsigned int mem_addr, u32 data);
void cs_dsp_remove_padding(u32 *buf, int nwords);

struct cs_dsp_alg_region *cs_dsp_find_alg_region(struct cs_dsp *dsp,
						 int type, unsigned int id);

const char *cs_dsp_mem_region_name(unsigned int type);


struct cs_dsp_chunk {
	u8 *data;
	u8 *max;
	int bytes;

	u32 cache;
	int cachebits;
};


static inline struct cs_dsp_chunk cs_dsp_chunk(void *data, int size)
{
	struct cs_dsp_chunk ch = {
		.data = data,
		.max = data + size,
	};

	return ch;
}


static inline bool cs_dsp_chunk_end(struct cs_dsp_chunk *ch)
{
	return ch->data == ch->max;
}


static inline int cs_dsp_chunk_bytes(struct cs_dsp_chunk *ch)
{
	return ch->bytes;
}


static inline bool cs_dsp_chunk_valid_addr(struct cs_dsp_chunk *ch, void *addr)
{
	return (u8 *)addr >= ch->data && (u8 *)addr < ch->max;
}

int cs_dsp_chunk_write(struct cs_dsp_chunk *ch, int nbits, u32 val);
int cs_dsp_chunk_flush(struct cs_dsp_chunk *ch);
int cs_dsp_chunk_read(struct cs_dsp_chunk *ch, int nbits);

#endif
