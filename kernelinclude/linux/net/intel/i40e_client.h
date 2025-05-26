/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _I40E_CLIENT_H_
#define _I40E_CLIENT_H_

#include <linux/auxiliary_bus.h>

#define I40E_CLIENT_STR_LENGTH 10


#define I40E_CLIENT_VERSION_MAJOR 0
#define I40E_CLIENT_VERSION_MINOR 01
#define I40E_CLIENT_VERSION_BUILD 00
#define I40E_CLIENT_VERSION_STR     \
	__stringify(I40E_CLIENT_VERSION_MAJOR) "." \
	__stringify(I40E_CLIENT_VERSION_MINOR) "." \
	__stringify(I40E_CLIENT_VERSION_BUILD)

struct i40e_client_version {
	u8 major;
	u8 minor;
	u8 build;
	u8 rsvd;
};

enum i40e_client_instance_state {
	__I40E_CLIENT_INSTANCE_NONE,
	__I40E_CLIENT_INSTANCE_OPENED,
};

struct i40e_ops;
struct i40e_client;

#define I40E_QUEUE_INVALID_IDX	0xFFFF

struct i40e_qv_info {
	u32 v_idx; 
	u16 ceq_idx;
	u16 aeq_idx;
	u8 itr_idx;
};

struct i40e_qvlist_info {
	u32 num_vectors;
	struct i40e_qv_info qv_info[];
};





struct i40e_prio_qos_params {
	u16 qs_handle; 
	u8 tc; 
	u8 reserved;
};

#define I40E_CLIENT_MAX_USER_PRIORITY        8

struct i40e_qos_params {
	struct i40e_prio_qos_params prio_qos[I40E_CLIENT_MAX_USER_PRIORITY];
};

struct i40e_params {
	struct i40e_qos_params qos;
	u16 mtu;
};


struct i40e_info {
	struct i40e_client_version version;
	u8 lanmac[6];
	struct net_device *netdev;
	struct pci_dev *pcidev;
	struct auxiliary_device *aux_dev;
	u8 __iomem *hw_addr;
	u8 fid;	
#define I40E_CLIENT_FTYPE_PF 0
	u8 ftype; 
	void *pf;

	
	struct i40e_qvlist_info *qvlist_info;
	struct i40e_params params;
	struct i40e_ops *ops;

	u16 msix_count;	 
	
	struct msix_entry *msix_entries;
	u16 itr_index; 
	u16 fw_maj_ver;                 
	u16 fw_min_ver;                 
	u32 fw_build;                   
};

struct i40e_auxiliary_device {
	struct auxiliary_device aux_dev;
	struct i40e_info *ldev;
};

#define I40E_CLIENT_RESET_LEVEL_PF   1
#define I40E_CLIENT_RESET_LEVEL_CORE 2
#define I40E_CLIENT_VSI_FLAG_TCP_ENABLE  BIT(1)

struct i40e_ops {
	
	int (*setup_qvlist)(struct i40e_info *ldev, struct i40e_client *client,
			    struct i40e_qvlist_info *qv_info);

	int (*virtchnl_send)(struct i40e_info *ldev, struct i40e_client *client,
			     u32 vf_id, u8 *msg, u16 len);

	
	void (*request_reset)(struct i40e_info *ldev,
			      struct i40e_client *client, u32 level);

	
	int (*update_vsi_ctxt)(struct i40e_info *ldev,
			       struct i40e_client *client,
			       bool is_vf, u32 vf_id,
			       u32 flag, u32 valid_flag);
};

struct i40e_client_ops {
	
	int (*open)(struct i40e_info *ldev, struct i40e_client *client);

	
	void (*close)(struct i40e_info *ldev, struct i40e_client *client,
		      bool reset);

	
	void (*l2_param_change)(struct i40e_info *ldev,
				struct i40e_client *client,
				struct i40e_params *params);

	int (*virtchnl_receive)(struct i40e_info *ldev,
				struct i40e_client *client, u32 vf_id,
				u8 *msg, u16 len);

	
	void (*vf_reset)(struct i40e_info *ldev,
			 struct i40e_client *client, u32 vf_id);

	
	void (*vf_enable)(struct i40e_info *ldev,
			  struct i40e_client *client, u32 num_vfs);

	
	int (*vf_capable)(struct i40e_info *ldev,
			  struct i40e_client *client, u32 vf_id);
};


struct i40e_client_instance {
	struct list_head list;
	struct i40e_info lan_info;
	struct i40e_client *client;
	unsigned long  state;
};

struct i40e_client {
	struct list_head list;		
	char name[I40E_CLIENT_STR_LENGTH];
	struct i40e_client_version version;
	unsigned long state;		
	atomic_t ref_cnt;  
	u32 flags;
	u8 type;
#define I40E_CLIENT_IWARP 0
	const struct i40e_client_ops *ops; 
};

void i40e_client_device_register(struct i40e_info *ldev, struct i40e_client *client);
void i40e_client_device_unregister(struct i40e_info *ldev);

#endif 
