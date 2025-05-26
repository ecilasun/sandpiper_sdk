/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __SOUND_HDAUDIO_H
#define __SOUND_HDAUDIO_H

#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/iopoll.h>
#include <linux/pci.h>
#include <linux/pm_runtime.h>
#include <linux/timecounter.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/memalloc.h>
#include <sound/hda_verbs.h>
#include <drm/i915_component.h>


typedef u16 hda_nid_t;

struct hdac_bus;
struct hdac_stream;
struct hdac_device;
struct hdac_driver;
struct hdac_widget_tree;
struct hda_device_id;


extern struct bus_type snd_hda_bus_type;


struct snd_array {
	unsigned int used;
	unsigned int alloced;
	unsigned int elem_size;
	unsigned int alloc_align;
	void *list;
};


struct hdac_device {
	struct device dev;
	int type;
	struct hdac_bus *bus;
	unsigned int addr;		
	struct list_head list;		

	hda_nid_t afg;			
	hda_nid_t mfg;			

	
	unsigned int vendor_id;
	unsigned int subsystem_id;
	unsigned int revision_id;
	unsigned int afg_function_id;
	unsigned int mfg_function_id;
	unsigned int afg_unsol:1;
	unsigned int mfg_unsol:1;

	unsigned int power_caps;	

	const char *vendor_name;	
	const char *chip_name;		

	
	int (*exec_verb)(struct hdac_device *dev, unsigned int cmd,
			 unsigned int flags, unsigned int *res);

	
	unsigned int num_nodes;
	hda_nid_t start_nid, end_nid;

	
	atomic_t in_pm;		

	
	struct mutex widget_lock;
	struct hdac_widget_tree *widgets;

	
	struct regmap *regmap;
	struct mutex regmap_lock;
	struct snd_array vendor_verbs;
	bool lazy_cache:1;	
	bool caps_overwriting:1; 
	bool cache_coef:1;	
	unsigned int registered:1; 
};


enum {
	HDA_DEV_CORE,
	HDA_DEV_LEGACY,
	HDA_DEV_ASOC,
};

enum {
	SND_SKL_PCI_BIND_AUTO,	
	SND_SKL_PCI_BIND_LEGACY,
	SND_SKL_PCI_BIND_ASOC	
};


enum {
	HDA_INPUT, HDA_OUTPUT
};

#define dev_to_hdac_dev(_dev)	container_of(_dev, struct hdac_device, dev)

int snd_hdac_device_init(struct hdac_device *dev, struct hdac_bus *bus,
			 const char *name, unsigned int addr);
void snd_hdac_device_exit(struct hdac_device *dev);
int snd_hdac_device_register(struct hdac_device *codec);
void snd_hdac_device_unregister(struct hdac_device *codec);
int snd_hdac_device_set_chip_name(struct hdac_device *codec, const char *name);
int snd_hdac_codec_modalias(const struct hdac_device *hdac, char *buf, size_t size);

int snd_hdac_refresh_widgets(struct hdac_device *codec);

int snd_hdac_read(struct hdac_device *codec, hda_nid_t nid,
		  unsigned int verb, unsigned int parm, unsigned int *res);
int _snd_hdac_read_parm(struct hdac_device *codec, hda_nid_t nid, int parm,
			unsigned int *res);
int snd_hdac_read_parm_uncached(struct hdac_device *codec, hda_nid_t nid,
				int parm);
int snd_hdac_override_parm(struct hdac_device *codec, hda_nid_t nid,
			   unsigned int parm, unsigned int val);
int snd_hdac_get_connections(struct hdac_device *codec, hda_nid_t nid,
			     hda_nid_t *conn_list, int max_conns);
int snd_hdac_get_sub_nodes(struct hdac_device *codec, hda_nid_t nid,
			   hda_nid_t *start_id);
unsigned int snd_hdac_calc_stream_format(unsigned int rate,
					 unsigned int channels,
					 snd_pcm_format_t format,
					 unsigned int maxbps,
					 unsigned short spdif_ctls);
int snd_hdac_query_supported_pcm(struct hdac_device *codec, hda_nid_t nid,
				u32 *ratesp, u64 *formatsp, unsigned int *bpsp);
bool snd_hdac_is_supported_format(struct hdac_device *codec, hda_nid_t nid,
				  unsigned int format);

int snd_hdac_codec_read(struct hdac_device *hdac, hda_nid_t nid,
			int flags, unsigned int verb, unsigned int parm);
int snd_hdac_codec_write(struct hdac_device *hdac, hda_nid_t nid,
			int flags, unsigned int verb, unsigned int parm);
bool snd_hdac_check_power_state(struct hdac_device *hdac,
		hda_nid_t nid, unsigned int target_state);
unsigned int snd_hdac_sync_power_state(struct hdac_device *hdac,
		      hda_nid_t nid, unsigned int target_state);

static inline int snd_hdac_read_parm(struct hdac_device *codec, hda_nid_t nid,
				     int parm)
{
	unsigned int val;

	return _snd_hdac_read_parm(codec, nid, parm, &val) < 0 ? -1 : val;
}

#ifdef CONFIG_PM
int snd_hdac_power_up(struct hdac_device *codec);
int snd_hdac_power_down(struct hdac_device *codec);
int snd_hdac_power_up_pm(struct hdac_device *codec);
int snd_hdac_power_down_pm(struct hdac_device *codec);
int snd_hdac_keep_power_up(struct hdac_device *codec);


static inline void snd_hdac_enter_pm(struct hdac_device *codec)
{
	atomic_inc(&codec->in_pm);
}


static inline void snd_hdac_leave_pm(struct hdac_device *codec)
{
	atomic_dec(&codec->in_pm);
}

static inline bool snd_hdac_is_in_pm(struct hdac_device *codec)
{
	return atomic_read(&codec->in_pm);
}

static inline bool snd_hdac_is_power_on(struct hdac_device *codec)
{
	return !pm_runtime_suspended(&codec->dev);
}
#else
static inline int snd_hdac_power_up(struct hdac_device *codec) { return 0; }
static inline int snd_hdac_power_down(struct hdac_device *codec) { return 0; }
static inline int snd_hdac_power_up_pm(struct hdac_device *codec) { return 0; }
static inline int snd_hdac_power_down_pm(struct hdac_device *codec) { return 0; }
static inline int snd_hdac_keep_power_up(struct hdac_device *codec) { return 0; }
static inline void snd_hdac_enter_pm(struct hdac_device *codec) {}
static inline void snd_hdac_leave_pm(struct hdac_device *codec) {}
static inline bool snd_hdac_is_in_pm(struct hdac_device *codec) { return false; }
static inline bool snd_hdac_is_power_on(struct hdac_device *codec) { return true; }
#endif


struct hdac_driver {
	struct device_driver driver;
	int type;
	const struct hda_device_id *id_table;
	int (*match)(struct hdac_device *dev, struct hdac_driver *drv);
	void (*unsol_event)(struct hdac_device *dev, unsigned int event);

	
	int (*probe)(struct hdac_device *dev);
	int (*remove)(struct hdac_device *dev);
	void (*shutdown)(struct hdac_device *dev);
};

#define drv_to_hdac_driver(_drv) container_of(_drv, struct hdac_driver, driver)

const struct hda_device_id *
hdac_get_device_id(struct hdac_device *hdev, struct hdac_driver *drv);


struct hdac_bus_ops {
	
	int (*command)(struct hdac_bus *bus, unsigned int cmd);
	
	int (*get_response)(struct hdac_bus *bus, unsigned int addr,
			    unsigned int *res);
	
	void (*link_power)(struct hdac_device *hdev, bool enable);
};


struct hdac_ext_bus_ops {
	int (*hdev_attach)(struct hdac_device *hdev);
	int (*hdev_detach)(struct hdac_device *hdev);
};

#define HDA_UNSOL_QUEUE_SIZE	64
#define HDA_MAX_CODECS		8	


struct hdac_rb {
	__le32 *buf;		
	dma_addr_t addr;	
	unsigned short rp, wp;	
	int cmds[HDA_MAX_CODECS];	
	u32 res[HDA_MAX_CODECS];	
};


struct hdac_bus {
	struct device *dev;
	const struct hdac_bus_ops *ops;
	const struct hdac_ext_bus_ops *ext_ops;

	
	unsigned long addr;
	void __iomem *remap_addr;
	int irq;

	void __iomem *ppcap;
	void __iomem *spbcap;
	void __iomem *mlcap;
	void __iomem *gtscap;
	void __iomem *drsmcap;

	
	struct list_head codec_list;
	unsigned int num_codecs;

	
	struct hdac_device *caddr_tbl[HDA_MAX_CODEC_ADDRESS + 1];

	
	u32 unsol_queue[HDA_UNSOL_QUEUE_SIZE * 2]; 
	unsigned int unsol_rp, unsol_wp;
	struct work_struct unsol_work;

	
	unsigned long codec_mask;

	
	unsigned long codec_powered;

	
	struct hdac_rb corb;
	struct hdac_rb rirb;
	unsigned int last_cmd[HDA_MAX_CODECS];	
	wait_queue_head_t rirb_wq;

	
	struct snd_dma_buffer rb;
	struct snd_dma_buffer posbuf;
	int dma_type;			

	
	struct list_head stream_list;

	
	bool chip_init:1;		

	
	bool aligned_mmio:1;		
	bool sync_write:1;		
	bool use_posbuf:1;		
	bool snoop:1;			
	bool align_bdle_4k:1;		
	bool reverse_assign:1;		
	bool corbrp_self_clear:1;	
	bool polling_mode:1;
	bool needs_damn_long_delay:1;
	bool not_use_interrupts:1;	
	bool access_sdnctl_in_dword:1;	

	int poll_count;

	int bdl_pos_adj;		

	
	unsigned int dma_stop_delay;

	
	spinlock_t reg_lock;
	struct mutex cmd_mutex;
	struct mutex lock;

	
	struct drm_audio_component *audio_component;
	long display_power_status;
	unsigned long display_power_active;

	
	int num_streams;
	int idx;

	
	struct list_head hlink_list;
	bool cmd_dma_state;

	
	unsigned int sdo_limit;
};

int snd_hdac_bus_init(struct hdac_bus *bus, struct device *dev,
		      const struct hdac_bus_ops *ops);
void snd_hdac_bus_exit(struct hdac_bus *bus);
int snd_hdac_bus_exec_verb_unlocked(struct hdac_bus *bus, unsigned int addr,
				    unsigned int cmd, unsigned int *res);

void snd_hdac_codec_link_up(struct hdac_device *codec);
void snd_hdac_codec_link_down(struct hdac_device *codec);

int snd_hdac_bus_send_cmd(struct hdac_bus *bus, unsigned int val);
int snd_hdac_bus_get_response(struct hdac_bus *bus, unsigned int addr,
			      unsigned int *res);
int snd_hdac_bus_parse_capabilities(struct hdac_bus *bus);

bool snd_hdac_bus_init_chip(struct hdac_bus *bus, bool full_reset);
void snd_hdac_bus_stop_chip(struct hdac_bus *bus);
void snd_hdac_bus_init_cmd_io(struct hdac_bus *bus);
void snd_hdac_bus_stop_cmd_io(struct hdac_bus *bus);
void snd_hdac_bus_enter_link_reset(struct hdac_bus *bus);
void snd_hdac_bus_exit_link_reset(struct hdac_bus *bus);
int snd_hdac_bus_reset_link(struct hdac_bus *bus, bool full_reset);
void snd_hdac_bus_link_power(struct hdac_device *hdev, bool enable);

void snd_hdac_bus_update_rirb(struct hdac_bus *bus);
int snd_hdac_bus_handle_stream_irq(struct hdac_bus *bus, unsigned int status,
				    void (*ack)(struct hdac_bus *,
						struct hdac_stream *));

int snd_hdac_bus_alloc_stream_pages(struct hdac_bus *bus);
void snd_hdac_bus_free_stream_pages(struct hdac_bus *bus);

#ifdef CONFIG_SND_HDA_ALIGNED_MMIO
unsigned int snd_hdac_aligned_read(void __iomem *addr, unsigned int mask);
void snd_hdac_aligned_write(unsigned int val, void __iomem *addr,
			    unsigned int mask);
#define snd_hdac_aligned_mmio(bus)	(bus)->aligned_mmio
#else
#define snd_hdac_aligned_mmio(bus)	false
#define snd_hdac_aligned_read(addr, mask)	0
#define snd_hdac_aligned_write(val, addr, mask) do {} while (0)
#endif

static inline void snd_hdac_reg_writeb(struct hdac_bus *bus, void __iomem *addr,
				       u8 val)
{
	if (snd_hdac_aligned_mmio(bus))
		snd_hdac_aligned_write(val, addr, 0xff);
	else
		writeb(val, addr);
}

static inline void snd_hdac_reg_writew(struct hdac_bus *bus, void __iomem *addr,
				       u16 val)
{
	if (snd_hdac_aligned_mmio(bus))
		snd_hdac_aligned_write(val, addr, 0xffff);
	else
		writew(val, addr);
}

static inline u8 snd_hdac_reg_readb(struct hdac_bus *bus, void __iomem *addr)
{
	return snd_hdac_aligned_mmio(bus) ?
		snd_hdac_aligned_read(addr, 0xff) : readb(addr);
}

static inline u16 snd_hdac_reg_readw(struct hdac_bus *bus, void __iomem *addr)
{
	return snd_hdac_aligned_mmio(bus) ?
		snd_hdac_aligned_read(addr, 0xffff) : readw(addr);
}

#define snd_hdac_reg_writel(bus, addr, val)	writel(val, addr)
#define snd_hdac_reg_readl(bus, addr)	readl(addr)
#define snd_hdac_reg_writeq(bus, addr, val)	writeq(val, addr)
#define snd_hdac_reg_readq(bus, addr)		readq(addr)


#define _snd_hdac_chip_writeb(chip, reg, value) \
	snd_hdac_reg_writeb(chip, (chip)->remap_addr + (reg), value)
#define _snd_hdac_chip_readb(chip, reg) \
	snd_hdac_reg_readb(chip, (chip)->remap_addr + (reg))
#define _snd_hdac_chip_writew(chip, reg, value) \
	snd_hdac_reg_writew(chip, (chip)->remap_addr + (reg), value)
#define _snd_hdac_chip_readw(chip, reg) \
	snd_hdac_reg_readw(chip, (chip)->remap_addr + (reg))
#define _snd_hdac_chip_writel(chip, reg, value) \
	snd_hdac_reg_writel(chip, (chip)->remap_addr + (reg), value)
#define _snd_hdac_chip_readl(chip, reg) \
	snd_hdac_reg_readl(chip, (chip)->remap_addr + (reg))


#define snd_hdac_chip_writel(chip, reg, value) \
	_snd_hdac_chip_writel(chip, AZX_REG_ ## reg, value)
#define snd_hdac_chip_writew(chip, reg, value) \
	_snd_hdac_chip_writew(chip, AZX_REG_ ## reg, value)
#define snd_hdac_chip_writeb(chip, reg, value) \
	_snd_hdac_chip_writeb(chip, AZX_REG_ ## reg, value)
#define snd_hdac_chip_readl(chip, reg) \
	_snd_hdac_chip_readl(chip, AZX_REG_ ## reg)
#define snd_hdac_chip_readw(chip, reg) \
	_snd_hdac_chip_readw(chip, AZX_REG_ ## reg)
#define snd_hdac_chip_readb(chip, reg) \
	_snd_hdac_chip_readb(chip, AZX_REG_ ## reg)


#define snd_hdac_chip_updatel(chip, reg, mask, val) \
	snd_hdac_chip_writel(chip, reg, \
			     (snd_hdac_chip_readl(chip, reg) & ~(mask)) | (val))
#define snd_hdac_chip_updatew(chip, reg, mask, val) \
	snd_hdac_chip_writew(chip, reg, \
			     (snd_hdac_chip_readw(chip, reg) & ~(mask)) | (val))
#define snd_hdac_chip_updateb(chip, reg, mask, val) \
	snd_hdac_chip_writeb(chip, reg, \
			     (snd_hdac_chip_readb(chip, reg) & ~(mask)) | (val))


#define snd_hdac_updatel(addr, reg, mask, val)		\
	writel(((readl(addr + reg) & ~(mask)) | (val)), addr + reg)

#define snd_hdac_updatew(addr, reg, mask, val)		\
	writew(((readw(addr + reg) & ~(mask)) | (val)), addr + reg)


struct hdac_stream {
	struct hdac_bus *bus;
	struct snd_dma_buffer bdl; 
	__le32 *posbuf;		
	int direction;		

	unsigned int bufsize;	
	unsigned int period_bytes; 
	unsigned int frags;	
	unsigned int fifo_size;	

	void __iomem *sd_addr;	

	void __iomem *spib_addr; 
	void __iomem *fifo_addr; 

	void __iomem *dpibr_addr; 
	u32 dpib;		
	u32 lpib;		

	u32 sd_int_sta_mask;	

	
	struct snd_pcm_substream *substream;	
	struct snd_compr_stream *cstream;
	unsigned int format_val;	
	unsigned char stream_tag;	
	unsigned char index;		
	int assigned_key;		

	bool opened:1;
	bool running:1;
	bool prepared:1;
	bool no_period_wakeup:1;
	bool locked:1;
	bool stripe:1;			

	u64 curr_pos;
	
	unsigned long start_wallclk;	
	unsigned long period_wallclk;	
	struct timecounter  tc;
	struct cyclecounter cc;
	int delay_negative_threshold;

	struct list_head list;
#ifdef CONFIG_SND_HDA_DSP_LOADER
	
	struct mutex dsp_mutex;
#endif
};

void snd_hdac_stream_init(struct hdac_bus *bus, struct hdac_stream *azx_dev,
			  int idx, int direction, int tag);
struct hdac_stream *snd_hdac_stream_assign(struct hdac_bus *bus,
					   struct snd_pcm_substream *substream);
void snd_hdac_stream_release_locked(struct hdac_stream *azx_dev);
void snd_hdac_stream_release(struct hdac_stream *azx_dev);
struct hdac_stream *snd_hdac_get_stream(struct hdac_bus *bus,
					int dir, int stream_tag);

int snd_hdac_stream_setup(struct hdac_stream *azx_dev);
void snd_hdac_stream_cleanup(struct hdac_stream *azx_dev);
int snd_hdac_stream_setup_periods(struct hdac_stream *azx_dev);
int snd_hdac_stream_set_params(struct hdac_stream *azx_dev,
				unsigned int format_val);
void snd_hdac_stream_start(struct hdac_stream *azx_dev);
void snd_hdac_stream_stop(struct hdac_stream *azx_dev);
void snd_hdac_stop_streams(struct hdac_bus *bus);
void snd_hdac_stop_streams_and_chip(struct hdac_bus *bus);
void snd_hdac_stream_reset(struct hdac_stream *azx_dev);
void snd_hdac_stream_sync_trigger(struct hdac_stream *azx_dev, bool set,
				  unsigned int streams, unsigned int reg);
void snd_hdac_stream_sync(struct hdac_stream *azx_dev, bool start,
			  unsigned int streams);
void snd_hdac_stream_timecounter_init(struct hdac_stream *azx_dev,
				      unsigned int streams);
int snd_hdac_get_stream_stripe_ctl(struct hdac_bus *bus,
				struct snd_pcm_substream *substream);

void snd_hdac_stream_spbcap_enable(struct hdac_bus *chip,
				   bool enable, int index);
int snd_hdac_stream_set_spib(struct hdac_bus *bus,
			     struct hdac_stream *azx_dev, u32 value);
int snd_hdac_stream_get_spbmaxfifo(struct hdac_bus *bus,
				   struct hdac_stream *azx_dev);
void snd_hdac_stream_drsm_enable(struct hdac_bus *bus,
				 bool enable, int index);
int snd_hdac_stream_wait_drsm(struct hdac_stream *azx_dev);
int snd_hdac_stream_set_dpibr(struct hdac_bus *bus,
			      struct hdac_stream *azx_dev, u32 value);
int snd_hdac_stream_set_lpib(struct hdac_stream *azx_dev, u32 value);



#define snd_hdac_stream_writel(dev, reg, value) \
	snd_hdac_reg_writel((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg, value)
#define snd_hdac_stream_writew(dev, reg, value) \
	snd_hdac_reg_writew((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg, value)
#define snd_hdac_stream_writeb(dev, reg, value) \
	snd_hdac_reg_writeb((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg, value)
#define snd_hdac_stream_readl(dev, reg) \
	snd_hdac_reg_readl((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg)
#define snd_hdac_stream_readw(dev, reg) \
	snd_hdac_reg_readw((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg)
#define snd_hdac_stream_readb(dev, reg) \
	snd_hdac_reg_readb((dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg)
#define snd_hdac_stream_readb_poll(dev, reg, val, cond, delay_us, timeout_us) \
	read_poll_timeout_atomic(snd_hdac_reg_readb, val, cond, delay_us, timeout_us, \
				 false, (dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg)
#define snd_hdac_stream_readl_poll(dev, reg, val, cond, delay_us, timeout_us) \
	read_poll_timeout_atomic(snd_hdac_reg_readl, val, cond, delay_us, timeout_us, \
				 false, (dev)->bus, (dev)->sd_addr + AZX_REG_ ## reg)


#define snd_hdac_stream_updatel(dev, reg, mask, val) \
	snd_hdac_stream_writel(dev, reg, \
			       (snd_hdac_stream_readl(dev, reg) & \
				~(mask)) | (val))
#define snd_hdac_stream_updatew(dev, reg, mask, val) \
	snd_hdac_stream_writew(dev, reg, \
			       (snd_hdac_stream_readw(dev, reg) & \
				~(mask)) | (val))
#define snd_hdac_stream_updateb(dev, reg, mask, val) \
	snd_hdac_stream_writeb(dev, reg, \
			       (snd_hdac_stream_readb(dev, reg) & \
				~(mask)) | (val))

#ifdef CONFIG_SND_HDA_DSP_LOADER

#define snd_hdac_dsp_lock_init(dev)	mutex_init(&(dev)->dsp_mutex)
#define snd_hdac_dsp_lock(dev)		mutex_lock(&(dev)->dsp_mutex)
#define snd_hdac_dsp_unlock(dev)	mutex_unlock(&(dev)->dsp_mutex)
#define snd_hdac_stream_is_locked(dev)	((dev)->locked)

int snd_hdac_dsp_prepare(struct hdac_stream *azx_dev, unsigned int format,
			 unsigned int byte_size, struct snd_dma_buffer *bufp);
void snd_hdac_dsp_trigger(struct hdac_stream *azx_dev, bool start);
void snd_hdac_dsp_cleanup(struct hdac_stream *azx_dev,
			  struct snd_dma_buffer *dmab);
#else 
#define snd_hdac_dsp_lock_init(dev)	do {} while (0)
#define snd_hdac_dsp_lock(dev)		do {} while (0)
#define snd_hdac_dsp_unlock(dev)	do {} while (0)
#define snd_hdac_stream_is_locked(dev)	0

static inline int
snd_hdac_dsp_prepare(struct hdac_stream *azx_dev, unsigned int format,
		     unsigned int byte_size, struct snd_dma_buffer *bufp)
{
	return 0;
}

static inline void snd_hdac_dsp_trigger(struct hdac_stream *azx_dev, bool start)
{
}

static inline void snd_hdac_dsp_cleanup(struct hdac_stream *azx_dev,
					struct snd_dma_buffer *dmab)
{
}
#endif 



void *snd_array_new(struct snd_array *array);
void snd_array_free(struct snd_array *array);
static inline void snd_array_init(struct snd_array *array, unsigned int size,
				  unsigned int align)
{
	array->elem_size = size;
	array->alloc_align = align;
}

static inline void *snd_array_elem(struct snd_array *array, unsigned int idx)
{
	return array->list + idx * array->elem_size;
}

static inline unsigned int snd_array_index(struct snd_array *array, void *ptr)
{
	return (unsigned long)(ptr - array->list) / array->elem_size;
}


#define snd_array_for_each(array, idx, ptr) \
	for ((idx) = 0, (ptr) = (array)->list; (idx) < (array)->used; \
	     (ptr) = snd_array_elem(array, ++(idx)))



#define HDA_CONTROLLER_IS_HSW(pci) (pci_match_id((struct pci_device_id []){ \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_HSW_0) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_HSW_2) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_HSW_3) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_BDW) }, \
			{ } \
		}, pci))

#define HDA_CONTROLLER_IS_APL(pci) (pci_match_id((struct pci_device_id []){ \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_APL) }, \
			{ } \
		}, pci))

#define HDA_CONTROLLER_IN_GPU(pci) (pci_match_id((struct pci_device_id []){ \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_DG1) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_DG2_0) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_DG2_1) }, \
			{ PCI_DEVICE(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HDA_DG2_2) }, \
			{ } \
		}, pci) || HDA_CONTROLLER_IS_HSW(pci))

#endif 
