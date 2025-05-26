

#ifndef _DVBDEV_H_
#define _DVBDEV_H_

#include <linux/types.h>
#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <media/media-device.h>

#define DVB_MAJOR 212

#if defined(CONFIG_DVB_MAX_ADAPTERS) && CONFIG_DVB_MAX_ADAPTERS > 0
  #define DVB_MAX_ADAPTERS CONFIG_DVB_MAX_ADAPTERS
#else
  #define DVB_MAX_ADAPTERS 16
#endif

#define DVB_UNSET (-1)




enum dvb_device_type {
	DVB_DEVICE_SEC,
	DVB_DEVICE_FRONTEND,
	DVB_DEVICE_DEMUX,
	DVB_DEVICE_DVR,
	DVB_DEVICE_CA,
	DVB_DEVICE_NET,

	DVB_DEVICE_VIDEO,
	DVB_DEVICE_AUDIO,
	DVB_DEVICE_OSD,
};

#define DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr) \
	static short adapter_nr[] = \
		{[0 ... (DVB_MAX_ADAPTERS - 1)] = DVB_UNSET }; \
	module_param_array(adapter_nr, short, NULL, 0444); \
	MODULE_PARM_DESC(adapter_nr, "DVB adapter numbers")

struct dvb_frontend;


struct dvb_adapter {
	int num;
	struct list_head list_head;
	struct list_head device_list;
	const char *name;
	u8 proposed_mac [6];
	void* priv;

	struct device *device;

	struct module *module;

	int mfe_shared;			
	struct dvb_device *mfe_dvbdev;	
	struct mutex mfe_lock;		

#if defined(CONFIG_MEDIA_CONTROLLER_DVB)
	struct mutex mdev_lock;
	struct media_device *mdev;
	struct media_entity *conn;
	struct media_pad *conn_pads;
#endif
};


struct dvb_device {
	struct list_head list_head;
	struct kref ref;
	const struct file_operations *fops;
	struct dvb_adapter *adapter;
	enum dvb_device_type type;
	int minor;
	u32 id;

	
	int readers;
	int writers;
	int users;

	wait_queue_head_t	  wait_queue;
	
	int (*kernel_ioctl)(struct file *file, unsigned int cmd, void *arg);

	
#if defined(CONFIG_MEDIA_CONTROLLER_DVB)
	const char *name;

	
	struct media_intf_devnode *intf_devnode;

	unsigned tsout_num_entities;
	struct media_entity *entity, *tsout_entity;
	struct media_pad *pads, *tsout_pads;
#endif

	void *priv;
};


struct dvbdevfops_node {
	struct file_operations *fops;
	enum dvb_device_type type;
	const struct dvb_device *template;
	struct list_head list_head;
};


struct dvb_device *dvb_device_get(struct dvb_device *dvbdev);


void dvb_device_put(struct dvb_device *dvbdev);


int dvb_register_adapter(struct dvb_adapter *adap, const char *name,
			 struct module *module, struct device *device,
			 short *adapter_nums);


int dvb_unregister_adapter(struct dvb_adapter *adap);


int dvb_register_device(struct dvb_adapter *adap,
			struct dvb_device **pdvbdev,
			const struct dvb_device *template,
			void *priv,
			enum dvb_device_type type,
			int demux_sink_pads);


void dvb_remove_device(struct dvb_device *dvbdev);



void dvb_unregister_device(struct dvb_device *dvbdev);

#ifdef CONFIG_MEDIA_CONTROLLER_DVB

__must_check int dvb_create_media_graph(struct dvb_adapter *adap,
					bool create_rf_connector);


static inline void dvb_register_media_controller(struct dvb_adapter *adap,
						 struct media_device *mdev)
{
	adap->mdev = mdev;
}


static inline struct media_device *
dvb_get_media_controller(struct dvb_adapter *adap)
{
	return adap->mdev;
}
#else
static inline
int dvb_create_media_graph(struct dvb_adapter *adap,
			   bool create_rf_connector)
{
	return 0;
};
#define dvb_register_media_controller(a, b) {}
#define dvb_get_media_controller(a) NULL
#endif


int dvb_generic_open(struct inode *inode, struct file *file);


int dvb_generic_release(struct inode *inode, struct file *file);


long dvb_generic_ioctl(struct file *file,
		       unsigned int cmd, unsigned long arg);


int dvb_usercopy(struct file *file, unsigned int cmd, unsigned long arg,
		 int (*func)(struct file *file, unsigned int cmd, void *arg));

#if IS_ENABLED(CONFIG_I2C)

struct i2c_adapter;
struct i2c_client;

struct i2c_client *dvb_module_probe(const char *module_name,
				    const char *name,
				    struct i2c_adapter *adap,
				    unsigned char addr,
				    void *platform_data);


void dvb_module_release(struct i2c_client *client);

#endif 


#ifdef CONFIG_MEDIA_ATTACH


#define dvb_attach(FUNCTION, ARGS...) ({ \
	void *__r = NULL; \
	typeof(&FUNCTION) __a = symbol_request(FUNCTION); \
	if (__a) { \
		__r = (void *) __a(ARGS); \
		if (__r == NULL) \
			symbol_put(FUNCTION); \
	} else { \
		printk(KERN_ERR "DVB: Unable to find symbol "#FUNCTION"()\n"); \
	} \
	__r; \
})



#define dvb_detach(FUNC)	symbol_put_addr(FUNC)

#else
#define dvb_attach(FUNCTION, ARGS...) ({ \
	FUNCTION(ARGS); \
})

#define dvb_detach(FUNC)	{}

#endif	

#endif 
