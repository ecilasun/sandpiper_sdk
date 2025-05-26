/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __SOUND_HDA_CODEC_H
#define __SOUND_HDA_CODEC_H

#include <linux/refcount.h>
#include <linux/mod_devicetable.h>
#include <sound/info.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/hwdep.h>
#include <sound/hdaudio.h>
#include <sound/hda_verbs.h>
#include <sound/hda_regmap.h>



struct hda_bus;
struct hda_beep;
struct hda_codec;
struct hda_pcm;
struct hda_pcm_stream;


struct hda_bus {
	struct hdac_bus core;

	struct snd_card *card;

	struct pci_dev *pci;
	const char *modelname;

	struct mutex prepare_mutex;

	
	DECLARE_BITMAP(pcm_dev_bits, SNDRV_PCM_DEVICES);

	
	unsigned int allow_bus_reset:1;	
	
	unsigned int shutdown :1;	
	unsigned int response_reset:1;	
	unsigned int in_reset:1;	
	unsigned int no_response_fallback:1; 
	unsigned int bus_probing :1;	
	unsigned int keep_power:1;	
	unsigned int jackpoll_in_suspend:1; 

	int primary_dig_out_type;	
	unsigned int mixer_assigned;	
};


#define to_hda_bus(bus)		container_of(bus, struct hda_bus, core)


typedef int (*hda_codec_patch_t)(struct hda_codec *);
	
#define HDA_CODEC_ID_SKIP_PROBE		0x00000001
#define HDA_CODEC_ID_GENERIC_HDMI	0x00000101
#define HDA_CODEC_ID_GENERIC		0x00000201

#define HDA_CODEC_REV_ENTRY(_vid, _rev, _name, _patch) \
	{ .vendor_id = (_vid), .rev_id = (_rev), .name = (_name), \
	  .api_version = HDA_DEV_LEGACY, \
	  .driver_data = (unsigned long)(_patch) }
#define HDA_CODEC_ENTRY(_vid, _name, _patch) \
	HDA_CODEC_REV_ENTRY(_vid, 0, _name, _patch)

struct hda_codec_driver {
	struct hdac_driver core;
	const struct hda_device_id *id;
};

int __hda_codec_driver_register(struct hda_codec_driver *drv, const char *name,
			       struct module *owner);
#define hda_codec_driver_register(drv) \
	__hda_codec_driver_register(drv, KBUILD_MODNAME, THIS_MODULE)
void hda_codec_driver_unregister(struct hda_codec_driver *drv);
#define module_hda_codec_driver(drv) \
	module_driver(drv, hda_codec_driver_register, \
		      hda_codec_driver_unregister)


struct hda_codec_ops {
	int (*build_controls)(struct hda_codec *codec);
	int (*build_pcms)(struct hda_codec *codec);
	int (*init)(struct hda_codec *codec);
	void (*free)(struct hda_codec *codec);
	void (*unsol_event)(struct hda_codec *codec, unsigned int res);
	void (*set_power_state)(struct hda_codec *codec, hda_nid_t fg,
				unsigned int power_state);
#ifdef CONFIG_PM
	int (*suspend)(struct hda_codec *codec);
	int (*resume)(struct hda_codec *codec);
	int (*check_power_status)(struct hda_codec *codec, hda_nid_t nid);
#endif
	void (*stream_pm)(struct hda_codec *codec, hda_nid_t nid, bool on);
};


struct hda_pcm_ops {
	int (*open)(struct hda_pcm_stream *info, struct hda_codec *codec,
		    struct snd_pcm_substream *substream);
	int (*close)(struct hda_pcm_stream *info, struct hda_codec *codec,
		     struct snd_pcm_substream *substream);
	int (*prepare)(struct hda_pcm_stream *info, struct hda_codec *codec,
		       unsigned int stream_tag, unsigned int format,
		       struct snd_pcm_substream *substream);
	int (*cleanup)(struct hda_pcm_stream *info, struct hda_codec *codec,
		       struct snd_pcm_substream *substream);
	unsigned int (*get_delay)(struct hda_pcm_stream *info,
				  struct hda_codec *codec,
				  struct snd_pcm_substream *substream);
};


struct hda_pcm_stream {
	unsigned int substreams;	
	unsigned int channels_min;	
	unsigned int channels_max;	
	hda_nid_t nid;	
	u32 rates;	
	u64 formats;	
	unsigned int maxbps;	
	const struct snd_pcm_chmap_elem *chmap; 
	struct hda_pcm_ops ops;
};


enum {
	HDA_PCM_TYPE_AUDIO,
	HDA_PCM_TYPE_SPDIF,
	HDA_PCM_TYPE_HDMI,
	HDA_PCM_TYPE_MODEM,
	HDA_PCM_NTYPES
};

#define SNDRV_PCM_INVALID_DEVICE	(-1)

struct hda_pcm {
	char *name;
	struct hda_pcm_stream stream[2];
	unsigned int pcm_type;	
	int device;		
	struct snd_pcm *pcm;	
	bool own_chmap;		
	
	struct hda_codec *codec;
	struct list_head list;
	unsigned int disconnected:1;
};


struct hda_codec {
	struct hdac_device core;
	struct hda_bus *bus;
	struct snd_card *card;
	unsigned int addr;	
	u32 probe_id; 

	
	const struct hda_device_id *preset;
	const char *modelname;	

	
	struct hda_codec_ops patch_ops;

	
	struct list_head pcm_list_head;
	refcount_t pcm_ref;
	wait_queue_head_t remove_sleep;

	
	void *spec;

	
	struct hda_beep *beep;
	unsigned int beep_mode;

	
	u32 *wcaps;

	struct snd_array mixers;	
	struct snd_array nids;		

	struct list_head conn_list;	

	struct mutex spdif_mutex;
	struct mutex control_mutex;
	struct snd_array spdif_out;
	unsigned int spdif_in_enable;	
	const hda_nid_t *follower_dig_outs; 
	struct snd_array init_pins;	
	struct snd_array driver_pins;	
	struct snd_array cvt_setups;	

	struct mutex user_mutex;
#ifdef CONFIG_SND_HDA_RECONFIG
	struct snd_array init_verbs;	
	struct snd_array hints;		
	struct snd_array user_pins;	
#endif

#ifdef CONFIG_SND_HDA_HWDEP
	struct snd_hwdep *hwdep;	
#endif

	
	unsigned int configured:1; 
	unsigned int in_freeing:1; 
	unsigned int display_power_control:1; 
	unsigned int spdif_status_reset :1; 
	unsigned int pin_amp_workaround:1; 
	unsigned int single_adc_amp:1; 
	unsigned int no_sticky_stream:1; 
	unsigned int pins_shutup:1;	
	unsigned int no_trigger_sense:1; 
	unsigned int no_jack_detect:1;	
	unsigned int inv_eapd:1; 
	unsigned int inv_jack_detect:1;	
	unsigned int pcm_format_first:1; 
	unsigned int cached_write:1;	
	unsigned int dp_mst:1; 
	unsigned int dump_coef:1; 
	unsigned int power_save_node:1; 
	unsigned int auto_runtime_pm:1; 
	unsigned int force_pin_prefix:1; 
	unsigned int link_down_at_suspend:1; 
	unsigned int relaxed_resume:1;	
	unsigned int forced_resume:1; 
	unsigned int no_stream_clean_at_suspend:1; 
	unsigned int ctl_dev_id:1; 

#ifdef CONFIG_PM
	unsigned long power_on_acct;
	unsigned long power_off_acct;
	unsigned long power_jiffies;
#endif

	
	unsigned int (*power_filter)(struct hda_codec *codec, hda_nid_t nid,
				     unsigned int power_state);

	
	void (*proc_widget_hook)(struct snd_info_buffer *buffer,
				 struct hda_codec *codec, hda_nid_t nid);

	
	struct snd_array jacktbl;
	unsigned long jackpoll_interval; 
	struct delayed_work jackpoll_work;

	int depop_delay; 

	
	int fixup_id;
	const struct hda_fixup *fixup_list;
	const char *fixup_name;

	
	struct snd_array verbs;
};

#define dev_to_hda_codec(_dev)	container_of(_dev, struct hda_codec, core.dev)
#define hda_codec_dev(_dev)	(&(_dev)->core.dev)

#define hdac_to_hda_codec(_hdac) container_of(_hdac, struct hda_codec, core)

#define list_for_each_codec(c, bus) \
	list_for_each_entry(c, &(bus)->core.codec_list, core.list)
#define list_for_each_codec_safe(c, n, bus)				\
	list_for_each_entry_safe(c, n, &(bus)->core.codec_list, core.list)


#define HDA_RW_NO_RESPONSE_FALLBACK	(1 << 0)


__printf(3, 4) struct hda_codec *
snd_hda_codec_device_init(struct hda_bus *bus, unsigned int codec_addr,
			  const char *fmt, ...);
int snd_hda_codec_new(struct hda_bus *bus, struct snd_card *card,
		      unsigned int codec_addr, struct hda_codec **codecp);
int snd_hda_codec_device_new(struct hda_bus *bus, struct snd_card *card,
		      unsigned int codec_addr, struct hda_codec *codec,
		      bool snddev_managed);
int snd_hda_codec_configure(struct hda_codec *codec);
int snd_hda_codec_update_widgets(struct hda_codec *codec);
void snd_hda_codec_register(struct hda_codec *codec);
void snd_hda_codec_unregister(struct hda_codec *codec);
void snd_hda_codec_cleanup_for_unbind(struct hda_codec *codec);


static inline unsigned int
snd_hda_codec_read(struct hda_codec *codec, hda_nid_t nid,
				int flags,
				unsigned int verb, unsigned int parm)
{
	return snd_hdac_codec_read(&codec->core, nid, flags, verb, parm);
}

static inline int
snd_hda_codec_write(struct hda_codec *codec, hda_nid_t nid, int flags,
			unsigned int verb, unsigned int parm)
{
	return snd_hdac_codec_write(&codec->core, nid, flags, verb, parm);
}

#define snd_hda_param_read(codec, nid, param) \
	snd_hdac_read_parm(&(codec)->core, nid, param)
#define snd_hda_get_sub_nodes(codec, nid, start_nid) \
	snd_hdac_get_sub_nodes(&(codec)->core, nid, start_nid)
int snd_hda_get_connections(struct hda_codec *codec, hda_nid_t nid,
			    hda_nid_t *conn_list, int max_conns);
static inline int
snd_hda_get_num_conns(struct hda_codec *codec, hda_nid_t nid)
{
	return snd_hda_get_connections(codec, nid, NULL, 0);
}

#define snd_hda_get_raw_connections(codec, nid, list, max_conns) \
	snd_hdac_get_connections(&(codec)->core, nid, list, max_conns)
#define snd_hda_get_num_raw_conns(codec, nid) \
	snd_hdac_get_connections(&(codec)->core, nid, NULL, 0)

int snd_hda_get_conn_list(struct hda_codec *codec, hda_nid_t nid,
			  const hda_nid_t **listp);
int snd_hda_override_conn_list(struct hda_codec *codec, hda_nid_t nid, int nums,
			  const hda_nid_t *list);
int snd_hda_get_conn_index(struct hda_codec *codec, hda_nid_t mux,
			   hda_nid_t nid, int recursive);
unsigned int snd_hda_get_num_devices(struct hda_codec *codec, hda_nid_t nid);
int snd_hda_get_devices(struct hda_codec *codec, hda_nid_t nid,
			u8 *dev_list, int max_devices);
int snd_hda_get_dev_select(struct hda_codec *codec, hda_nid_t nid);
int snd_hda_set_dev_select(struct hda_codec *codec, hda_nid_t nid, int dev_id);

struct hda_verb {
	hda_nid_t nid;
	u32 verb;
	u32 param;
};

void snd_hda_sequence_write(struct hda_codec *codec,
			    const struct hda_verb *seq);


static inline int
snd_hda_codec_write_cache(struct hda_codec *codec, hda_nid_t nid,
			  int flags, unsigned int verb, unsigned int parm)
{
	return snd_hdac_regmap_write(&codec->core, nid, verb, parm);
}


struct hda_pincfg {
	hda_nid_t nid;
	unsigned char ctrl;	
	unsigned char target;	
	unsigned int cfg;	
};

unsigned int snd_hda_codec_get_pincfg(struct hda_codec *codec, hda_nid_t nid);
int snd_hda_codec_set_pincfg(struct hda_codec *codec, hda_nid_t nid,
			     unsigned int cfg);
int snd_hda_add_pincfg(struct hda_codec *codec, struct snd_array *list,
		       hda_nid_t nid, unsigned int cfg); 
void snd_hda_shutup_pins(struct hda_codec *codec);


struct hda_spdif_out {
	hda_nid_t nid;		
	unsigned int status;	
	unsigned short ctls;	
};
struct hda_spdif_out *snd_hda_spdif_out_of_nid(struct hda_codec *codec,
					       hda_nid_t nid);
void snd_hda_spdif_ctls_unassign(struct hda_codec *codec, int idx);
void snd_hda_spdif_ctls_assign(struct hda_codec *codec, int idx, hda_nid_t nid);


int snd_hda_codec_build_controls(struct hda_codec *codec);


int snd_hda_codec_parse_pcms(struct hda_codec *codec);
int snd_hda_codec_build_pcms(struct hda_codec *codec);

__printf(2, 3)
struct hda_pcm *snd_hda_codec_pcm_new(struct hda_codec *codec,
				      const char *fmt, ...);

void snd_hda_codec_cleanup_for_unbind(struct hda_codec *codec);

static inline void snd_hda_codec_pcm_get(struct hda_pcm *pcm)
{
	refcount_inc(&pcm->codec->pcm_ref);
}
void snd_hda_codec_pcm_put(struct hda_pcm *pcm);

int snd_hda_codec_prepare(struct hda_codec *codec,
			  struct hda_pcm_stream *hinfo,
			  unsigned int stream,
			  unsigned int format,
			  struct snd_pcm_substream *substream);
void snd_hda_codec_cleanup(struct hda_codec *codec,
			   struct hda_pcm_stream *hinfo,
			   struct snd_pcm_substream *substream);

void snd_hda_codec_setup_stream(struct hda_codec *codec, hda_nid_t nid,
				u32 stream_tag,
				int channel_id, int format);
void __snd_hda_codec_cleanup_stream(struct hda_codec *codec, hda_nid_t nid,
				    int do_now);
#define snd_hda_codec_cleanup_stream(codec, nid) \
	__snd_hda_codec_cleanup_stream(codec, nid, 0)

#define snd_hda_query_supported_pcm(codec, nid, ratesp, fmtsp, bpsp) \
	snd_hdac_query_supported_pcm(&(codec)->core, nid, ratesp, fmtsp, bpsp)
#define snd_hda_is_supported_format(codec, nid, fmt) \
	snd_hdac_is_supported_format(&(codec)->core, nid, fmt)

extern const struct snd_pcm_chmap_elem snd_pcm_2_1_chmaps[];

int snd_hda_attach_pcm_stream(struct hda_bus *_bus, struct hda_codec *codec,
			      struct hda_pcm *cpcm);


void snd_hda_get_codec_name(struct hda_codec *codec, char *name, int namelen);
void snd_hda_codec_set_power_to_all(struct hda_codec *codec, hda_nid_t fg,
				    unsigned int power_state);

int snd_hda_lock_devices(struct hda_bus *bus);
void snd_hda_unlock_devices(struct hda_bus *bus);
void snd_hda_bus_reset(struct hda_bus *bus);
void snd_hda_bus_reset_codecs(struct hda_bus *bus);

int snd_hda_codec_set_name(struct hda_codec *codec, const char *name);


extern const struct dev_pm_ops hda_codec_driver_pm;

static inline
int hda_call_check_power_status(struct hda_codec *codec, hda_nid_t nid)
{
#ifdef CONFIG_PM
	if (codec->patch_ops.check_power_status)
		return codec->patch_ops.check_power_status(codec, nid);
#endif
	return 0;
}


#define snd_hda_power_up(codec)		snd_hdac_power_up(&(codec)->core)
#define snd_hda_power_up_pm(codec)	snd_hdac_power_up_pm(&(codec)->core)
#define snd_hda_power_down(codec)	snd_hdac_power_down(&(codec)->core)
#define snd_hda_power_down_pm(codec)	snd_hdac_power_down_pm(&(codec)->core)
#ifdef CONFIG_PM
void snd_hda_codec_set_power_save(struct hda_codec *codec, int delay);
void snd_hda_set_power_save(struct hda_bus *bus, int delay);
void snd_hda_update_power_acct(struct hda_codec *codec);
#else
static inline void snd_hda_codec_set_power_save(struct hda_codec *codec, int delay) {}
static inline void snd_hda_set_power_save(struct hda_bus *bus, int delay) {}
#endif

static inline bool hda_codec_need_resume(struct hda_codec *codec)
{
	return !codec->relaxed_resume && codec->jacktbl.used;
}

#ifdef CONFIG_SND_HDA_PATCH_LOADER

int snd_hda_load_patch(struct hda_bus *bus, size_t size, const void *buf);
#endif

#ifdef CONFIG_SND_HDA_DSP_LOADER
int snd_hda_codec_load_dsp_prepare(struct hda_codec *codec, unsigned int format,
				   unsigned int size,
				   struct snd_dma_buffer *bufp);
void snd_hda_codec_load_dsp_trigger(struct hda_codec *codec, bool start);
void snd_hda_codec_load_dsp_cleanup(struct hda_codec *codec,
				    struct snd_dma_buffer *dmab);
#else
static inline int
snd_hda_codec_load_dsp_prepare(struct hda_codec *codec, unsigned int format,
				unsigned int size,
				struct snd_dma_buffer *bufp)
{
	return -ENOSYS;
}
static inline void
snd_hda_codec_load_dsp_trigger(struct hda_codec *codec, bool start) {}
static inline void
snd_hda_codec_load_dsp_cleanup(struct hda_codec *codec,
				struct snd_dma_buffer *dmab) {}
#endif

#endif 
