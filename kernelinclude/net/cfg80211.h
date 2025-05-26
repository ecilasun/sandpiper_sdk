/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __NET_CFG80211_H
#define __NET_CFG80211_H


#include <linux/ethtool.h>
#include <uapi/linux/rfkill.h>
#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <linux/list.h>
#include <linux/bug.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/nl80211.h>
#include <linux/if_ether.h>
#include <linux/ieee80211.h>
#include <linux/net.h>
#include <linux/rfkill.h>
#include <net/regulatory.h>






struct wiphy;




enum ieee80211_channel_flags {
	IEEE80211_CHAN_DISABLED		= 1<<0,
	IEEE80211_CHAN_NO_IR		= 1<<1,
	
	IEEE80211_CHAN_RADAR		= 1<<3,
	IEEE80211_CHAN_NO_HT40PLUS	= 1<<4,
	IEEE80211_CHAN_NO_HT40MINUS	= 1<<5,
	IEEE80211_CHAN_NO_OFDM		= 1<<6,
	IEEE80211_CHAN_NO_80MHZ		= 1<<7,
	IEEE80211_CHAN_NO_160MHZ	= 1<<8,
	IEEE80211_CHAN_INDOOR_ONLY	= 1<<9,
	IEEE80211_CHAN_IR_CONCURRENT	= 1<<10,
	IEEE80211_CHAN_NO_20MHZ		= 1<<11,
	IEEE80211_CHAN_NO_10MHZ		= 1<<12,
	IEEE80211_CHAN_NO_HE		= 1<<13,
	IEEE80211_CHAN_1MHZ		= 1<<14,
	IEEE80211_CHAN_2MHZ		= 1<<15,
	IEEE80211_CHAN_4MHZ		= 1<<16,
	IEEE80211_CHAN_8MHZ		= 1<<17,
	IEEE80211_CHAN_16MHZ		= 1<<18,
	IEEE80211_CHAN_NO_320MHZ	= 1<<19,
	IEEE80211_CHAN_NO_EHT		= 1<<20,
};

#define IEEE80211_CHAN_NO_HT40 \
	(IEEE80211_CHAN_NO_HT40PLUS | IEEE80211_CHAN_NO_HT40MINUS)

#define IEEE80211_DFS_MIN_CAC_TIME_MS		60000
#define IEEE80211_DFS_MIN_NOP_TIME_MS		(30 * 60 * 1000)


struct ieee80211_channel {
	enum nl80211_band band;
	u32 center_freq;
	u16 freq_offset;
	u16 hw_value;
	u32 flags;
	int max_antenna_gain;
	int max_power;
	int max_reg_power;
	bool beacon_found;
	u32 orig_flags;
	int orig_mag, orig_mpwr;
	enum nl80211_dfs_state dfs_state;
	unsigned long dfs_state_entered;
	unsigned int dfs_cac_ms;
};


enum ieee80211_rate_flags {
	IEEE80211_RATE_SHORT_PREAMBLE	= 1<<0,
	IEEE80211_RATE_MANDATORY_A	= 1<<1,
	IEEE80211_RATE_MANDATORY_B	= 1<<2,
	IEEE80211_RATE_MANDATORY_G	= 1<<3,
	IEEE80211_RATE_ERP_G		= 1<<4,
	IEEE80211_RATE_SUPPORTS_5MHZ	= 1<<5,
	IEEE80211_RATE_SUPPORTS_10MHZ	= 1<<6,
};


enum ieee80211_bss_type {
	IEEE80211_BSS_TYPE_ESS,
	IEEE80211_BSS_TYPE_PBSS,
	IEEE80211_BSS_TYPE_IBSS,
	IEEE80211_BSS_TYPE_MBSS,
	IEEE80211_BSS_TYPE_ANY
};


enum ieee80211_privacy {
	IEEE80211_PRIVACY_ON,
	IEEE80211_PRIVACY_OFF,
	IEEE80211_PRIVACY_ANY
};

#define IEEE80211_PRIVACY(x)	\
	((x) ? IEEE80211_PRIVACY_ON : IEEE80211_PRIVACY_OFF)


struct ieee80211_rate {
	u32 flags;
	u16 bitrate;
	u16 hw_value, hw_value_short;
};


struct ieee80211_he_obss_pd {
	bool enable;
	u8 sr_ctrl;
	u8 non_srg_max_offset;
	u8 min_offset;
	u8 max_offset;
	u8 bss_color_bitmap[8];
	u8 partial_bssid_bitmap[8];
};


struct cfg80211_he_bss_color {
	u8 color;
	bool enabled;
	bool partial;
};


struct ieee80211_sta_ht_cap {
	u16 cap; 
	bool ht_supported;
	u8 ampdu_factor;
	u8 ampdu_density;
	struct ieee80211_mcs_info mcs;
};


struct ieee80211_sta_vht_cap {
	bool vht_supported;
	u32 cap; 
	struct ieee80211_vht_mcs_info vht_mcs;
};

#define IEEE80211_HE_PPE_THRES_MAX_LEN		25


struct ieee80211_sta_he_cap {
	bool has_he;
	struct ieee80211_he_cap_elem he_cap_elem;
	struct ieee80211_he_mcs_nss_supp he_mcs_nss_supp;
	u8 ppe_thres[IEEE80211_HE_PPE_THRES_MAX_LEN];
};


struct ieee80211_eht_mcs_nss_supp {
	union {
		struct ieee80211_eht_mcs_nss_supp_20mhz_only only_20mhz;
		struct {
			struct ieee80211_eht_mcs_nss_supp_bw _80;
			struct ieee80211_eht_mcs_nss_supp_bw _160;
			struct ieee80211_eht_mcs_nss_supp_bw _320;
		} __packed bw;
	} __packed;
} __packed;

#define IEEE80211_EHT_PPE_THRES_MAX_LEN		32


struct ieee80211_sta_eht_cap {
	bool has_eht;
	struct ieee80211_eht_cap_elem_fixed eht_cap_elem;
	struct ieee80211_eht_mcs_nss_supp eht_mcs_nss_supp;
	u8 eht_ppe_thres[IEEE80211_EHT_PPE_THRES_MAX_LEN];
};


struct ieee80211_sband_iftype_data {
	u16 types_mask;
	struct ieee80211_sta_he_cap he_cap;
	struct ieee80211_he_6ghz_capa he_6ghz_capa;
	struct ieee80211_sta_eht_cap eht_cap;
	struct {
		const u8 *data;
		unsigned int len;
	} vendor_elems;
};


enum ieee80211_edmg_bw_config {
	IEEE80211_EDMG_BW_CONFIG_4	= 4,
	IEEE80211_EDMG_BW_CONFIG_5	= 5,
	IEEE80211_EDMG_BW_CONFIG_6	= 6,
	IEEE80211_EDMG_BW_CONFIG_7	= 7,
	IEEE80211_EDMG_BW_CONFIG_8	= 8,
	IEEE80211_EDMG_BW_CONFIG_9	= 9,
	IEEE80211_EDMG_BW_CONFIG_10	= 10,
	IEEE80211_EDMG_BW_CONFIG_11	= 11,
	IEEE80211_EDMG_BW_CONFIG_12	= 12,
	IEEE80211_EDMG_BW_CONFIG_13	= 13,
	IEEE80211_EDMG_BW_CONFIG_14	= 14,
	IEEE80211_EDMG_BW_CONFIG_15	= 15,
};


struct ieee80211_edmg {
	u8 channels;
	enum ieee80211_edmg_bw_config bw_config;
};


struct ieee80211_sta_s1g_cap {
	bool s1g;
	u8 cap[10]; 
	u8 nss_mcs[5];
};


struct ieee80211_supported_band {
	struct ieee80211_channel *channels;
	struct ieee80211_rate *bitrates;
	enum nl80211_band band;
	int n_channels;
	int n_bitrates;
	struct ieee80211_sta_ht_cap ht_cap;
	struct ieee80211_sta_vht_cap vht_cap;
	struct ieee80211_sta_s1g_cap s1g_cap;
	struct ieee80211_edmg edmg_cap;
	u16 n_iftype_data;
	const struct ieee80211_sband_iftype_data *iftype_data;
};


static inline const struct ieee80211_sband_iftype_data *
ieee80211_get_sband_iftype_data(const struct ieee80211_supported_band *sband,
				u8 iftype)
{
	int i;

	if (WARN_ON(iftype >= NL80211_IFTYPE_MAX))
		return NULL;

	if (iftype == NL80211_IFTYPE_AP_VLAN)
		iftype = NL80211_IFTYPE_AP;

	for (i = 0; i < sband->n_iftype_data; i++)  {
		const struct ieee80211_sband_iftype_data *data =
			&sband->iftype_data[i];

		if (data->types_mask & BIT(iftype))
			return data;
	}

	return NULL;
}


static inline const struct ieee80211_sta_he_cap *
ieee80211_get_he_iftype_cap(const struct ieee80211_supported_band *sband,
			    u8 iftype)
{
	const struct ieee80211_sband_iftype_data *data =
		ieee80211_get_sband_iftype_data(sband, iftype);

	if (data && data->he_cap.has_he)
		return &data->he_cap;

	return NULL;
}


static inline __le16
ieee80211_get_he_6ghz_capa(const struct ieee80211_supported_band *sband,
			   enum nl80211_iftype iftype)
{
	const struct ieee80211_sband_iftype_data *data =
		ieee80211_get_sband_iftype_data(sband, iftype);

	if (WARN_ON(!data || !data->he_cap.has_he))
		return 0;

	return data->he_6ghz_capa.capa;
}


static inline const struct ieee80211_sta_eht_cap *
ieee80211_get_eht_iftype_cap(const struct ieee80211_supported_band *sband,
			     enum nl80211_iftype iftype)
{
	const struct ieee80211_sband_iftype_data *data =
		ieee80211_get_sband_iftype_data(sband, iftype);

	if (data && data->eht_cap.has_eht)
		return &data->eht_cap;

	return NULL;
}


#ifdef CONFIG_OF
void wiphy_read_of_freq_limits(struct wiphy *wiphy);
#else 
static inline void wiphy_read_of_freq_limits(struct wiphy *wiphy)
{
}
#endif 






#define VHT_MUMIMO_GROUPS_DATA_LEN (WLAN_MEMBERSHIP_LEN +\
				    WLAN_USER_POSITION_LEN)


struct vif_params {
	u32 flags;
	int use_4addr;
	u8 macaddr[ETH_ALEN];
	const u8 *vht_mumimo_groups;
	const u8 *vht_mumimo_follow_addr;
};


struct key_params {
	const u8 *key;
	const u8 *seq;
	int key_len;
	int seq_len;
	u16 vlan_id;
	u32 cipher;
	enum nl80211_key_mode mode;
};


struct cfg80211_chan_def {
	struct ieee80211_channel *chan;
	enum nl80211_chan_width width;
	u32 center_freq1;
	u32 center_freq2;
	struct ieee80211_edmg edmg;
	u16 freq1_offset;
};


struct cfg80211_bitrate_mask {
	struct {
		u32 legacy;
		u8 ht_mcs[IEEE80211_HT_MCS_MASK_LEN];
		u16 vht_mcs[NL80211_VHT_NSS_MAX];
		u16 he_mcs[NL80211_HE_NSS_MAX];
		enum nl80211_txrate_gi gi;
		enum nl80211_he_gi he_gi;
		enum nl80211_he_ltf he_ltf;
	} control[NUM_NL80211_BANDS];
};



struct cfg80211_tid_cfg {
	bool config_override;
	u8 tids;
	u64 mask;
	enum nl80211_tid_config noack;
	u8 retry_long, retry_short;
	enum nl80211_tid_config ampdu;
	enum nl80211_tid_config rtscts;
	enum nl80211_tid_config amsdu;
	enum nl80211_tx_rate_setting txrate_type;
	struct cfg80211_bitrate_mask txrate_mask;
};


struct cfg80211_tid_config {
	const u8 *peer;
	u32 n_tid_conf;
	struct cfg80211_tid_cfg tid_conf[] __counted_by(n_tid_conf);
};


struct cfg80211_fils_aad {
	const u8 *macaddr;
	const u8 *kek;
	u8 kek_len;
	const u8 *snonce;
	const u8 *anonce;
};


struct cfg80211_set_hw_timestamp {
	const u8 *macaddr;
	bool enable;
};


static inline enum nl80211_channel_type
cfg80211_get_chandef_type(const struct cfg80211_chan_def *chandef)
{
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_20_NOHT:
		return NL80211_CHAN_NO_HT;
	case NL80211_CHAN_WIDTH_20:
		return NL80211_CHAN_HT20;
	case NL80211_CHAN_WIDTH_40:
		if (chandef->center_freq1 > chandef->chan->center_freq)
			return NL80211_CHAN_HT40PLUS;
		return NL80211_CHAN_HT40MINUS;
	default:
		WARN_ON(1);
		return NL80211_CHAN_NO_HT;
	}
}


void cfg80211_chandef_create(struct cfg80211_chan_def *chandef,
			     struct ieee80211_channel *channel,
			     enum nl80211_channel_type chantype);


static inline bool
cfg80211_chandef_identical(const struct cfg80211_chan_def *chandef1,
			   const struct cfg80211_chan_def *chandef2)
{
	return (chandef1->chan == chandef2->chan &&
		chandef1->width == chandef2->width &&
		chandef1->center_freq1 == chandef2->center_freq1 &&
		chandef1->freq1_offset == chandef2->freq1_offset &&
		chandef1->center_freq2 == chandef2->center_freq2);
}


static inline bool
cfg80211_chandef_is_edmg(const struct cfg80211_chan_def *chandef)
{
	return chandef->edmg.channels || chandef->edmg.bw_config;
}


const struct cfg80211_chan_def *
cfg80211_chandef_compatible(const struct cfg80211_chan_def *chandef1,
			    const struct cfg80211_chan_def *chandef2);


bool cfg80211_chandef_valid(const struct cfg80211_chan_def *chandef);


bool cfg80211_chandef_usable(struct wiphy *wiphy,
			     const struct cfg80211_chan_def *chandef,
			     u32 prohibited_flags);


int cfg80211_chandef_dfs_required(struct wiphy *wiphy,
				  const struct cfg80211_chan_def *chandef,
				  enum nl80211_iftype iftype);


int nl80211_send_chandef(struct sk_buff *msg, const struct cfg80211_chan_def *chandef);


static inline enum ieee80211_rate_flags
ieee80211_chanwidth_rate_flags(enum nl80211_chan_width width)
{
	switch (width) {
	case NL80211_CHAN_WIDTH_5:
		return IEEE80211_RATE_SUPPORTS_5MHZ;
	case NL80211_CHAN_WIDTH_10:
		return IEEE80211_RATE_SUPPORTS_10MHZ;
	default:
		break;
	}
	return 0;
}


static inline enum ieee80211_rate_flags
ieee80211_chandef_rate_flags(struct cfg80211_chan_def *chandef)
{
	return ieee80211_chanwidth_rate_flags(chandef->width);
}


static inline int
ieee80211_chandef_max_power(struct cfg80211_chan_def *chandef)
{
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_5:
		return min(chandef->chan->max_reg_power - 6,
			   chandef->chan->max_power);
	case NL80211_CHAN_WIDTH_10:
		return min(chandef->chan->max_reg_power - 3,
			   chandef->chan->max_power);
	default:
		break;
	}
	return chandef->chan->max_power;
}


bool cfg80211_any_usable_channels(struct wiphy *wiphy,
				  unsigned long band_mask,
				  u32 prohibited_flags);


enum survey_info_flags {
	SURVEY_INFO_NOISE_DBM		= BIT(0),
	SURVEY_INFO_IN_USE		= BIT(1),
	SURVEY_INFO_TIME		= BIT(2),
	SURVEY_INFO_TIME_BUSY		= BIT(3),
	SURVEY_INFO_TIME_EXT_BUSY	= BIT(4),
	SURVEY_INFO_TIME_RX		= BIT(5),
	SURVEY_INFO_TIME_TX		= BIT(6),
	SURVEY_INFO_TIME_SCAN		= BIT(7),
	SURVEY_INFO_TIME_BSS_RX		= BIT(8),
};


struct survey_info {
	struct ieee80211_channel *channel;
	u64 time;
	u64 time_busy;
	u64 time_ext_busy;
	u64 time_rx;
	u64 time_tx;
	u64 time_scan;
	u64 time_bss_rx;
	u32 filled;
	s8 noise;
};

#define CFG80211_MAX_NUM_AKM_SUITES	10


struct cfg80211_crypto_settings {
	u32 wpa_versions;
	u32 cipher_group;
	int n_ciphers_pairwise;
	u32 ciphers_pairwise[NL80211_MAX_NR_CIPHER_SUITES];
	int n_akm_suites;
	u32 akm_suites[CFG80211_MAX_NUM_AKM_SUITES];
	bool control_port;
	__be16 control_port_ethertype;
	bool control_port_no_encrypt;
	bool control_port_over_nl80211;
	bool control_port_no_preauth;
	const u8 *psk;
	const u8 *sae_pwd;
	u8 sae_pwd_len;
	enum nl80211_sae_pwe_mechanism sae_pwe;
};


struct cfg80211_mbssid_config {
	struct wireless_dev *tx_wdev;
	u8 index;
	bool ema;
};


struct cfg80211_mbssid_elems {
	u8 cnt;
	struct {
		const u8 *data;
		size_t len;
	} elem[] __counted_by(cnt);
};


struct cfg80211_rnr_elems {
	u8 cnt;
	struct {
		const u8 *data;
		size_t len;
	} elem[] __counted_by(cnt);
};


struct cfg80211_beacon_data {
	unsigned int link_id;

	const u8 *head, *tail;
	const u8 *beacon_ies;
	const u8 *proberesp_ies;
	const u8 *assocresp_ies;
	const u8 *probe_resp;
	const u8 *lci;
	const u8 *civicloc;
	struct cfg80211_mbssid_elems *mbssid_ies;
	struct cfg80211_rnr_elems *rnr_ies;
	s8 ftm_responder;

	size_t head_len, tail_len;
	size_t beacon_ies_len;
	size_t proberesp_ies_len;
	size_t assocresp_ies_len;
	size_t probe_resp_len;
	size_t lci_len;
	size_t civicloc_len;
	struct cfg80211_he_bss_color he_bss_color;
	bool he_bss_color_valid;
};

struct mac_address {
	u8 addr[ETH_ALEN];
};


struct cfg80211_acl_data {
	enum nl80211_acl_policy acl_policy;
	int n_acl_entries;

	
	struct mac_address mac_addrs[] __counted_by(n_acl_entries);
};


struct cfg80211_fils_discovery {
	u32 min_interval;
	u32 max_interval;
	size_t tmpl_len;
	const u8 *tmpl;
};


struct cfg80211_unsol_bcast_probe_resp {
	u32 interval;
	size_t tmpl_len;
	const u8 *tmpl;
};


struct cfg80211_ap_settings {
	struct cfg80211_chan_def chandef;

	struct cfg80211_beacon_data beacon;

	int beacon_interval, dtim_period;
	const u8 *ssid;
	size_t ssid_len;
	enum nl80211_hidden_ssid hidden_ssid;
	struct cfg80211_crypto_settings crypto;
	bool privacy;
	enum nl80211_auth_type auth_type;
	enum nl80211_smps_mode smps_mode;
	int inactivity_timeout;
	u8 p2p_ctwindow;
	bool p2p_opp_ps;
	const struct cfg80211_acl_data *acl;
	bool pbss;
	struct cfg80211_bitrate_mask beacon_rate;

	const struct ieee80211_ht_cap *ht_cap;
	const struct ieee80211_vht_cap *vht_cap;
	const struct ieee80211_he_cap_elem *he_cap;
	const struct ieee80211_he_operation *he_oper;
	const struct ieee80211_eht_cap_elem *eht_cap;
	const struct ieee80211_eht_operation *eht_oper;
	bool ht_required, vht_required, he_required, sae_h2e_required;
	bool twt_responder;
	u32 flags;
	struct ieee80211_he_obss_pd he_obss_pd;
	struct cfg80211_fils_discovery fils_discovery;
	struct cfg80211_unsol_bcast_probe_resp unsol_bcast_probe_resp;
	struct cfg80211_mbssid_config mbssid_config;
	u16 punct_bitmap;
};


struct cfg80211_csa_settings {
	struct cfg80211_chan_def chandef;
	struct cfg80211_beacon_data beacon_csa;
	const u16 *counter_offsets_beacon;
	const u16 *counter_offsets_presp;
	unsigned int n_counter_offsets_beacon;
	unsigned int n_counter_offsets_presp;
	struct cfg80211_beacon_data beacon_after;
	bool radar_required;
	bool block_tx;
	u8 count;
	u16 punct_bitmap;
};


struct cfg80211_color_change_settings {
	struct cfg80211_beacon_data beacon_color_change;
	u16 counter_offset_beacon;
	u16 counter_offset_presp;
	struct cfg80211_beacon_data beacon_next;
	u8 count;
	u8 color;
};


struct iface_combination_params {
	int num_different_channels;
	u8 radar_detect;
	int iftype_num[NUM_NL80211_IFTYPES];
	u32 new_beacon_int;
};


enum station_parameters_apply_mask {
	STATION_PARAM_APPLY_UAPSD = BIT(0),
	STATION_PARAM_APPLY_CAPABILITY = BIT(1),
	STATION_PARAM_APPLY_PLINK_STATE = BIT(2),
};


struct sta_txpwr {
	s16 power;
	enum nl80211_tx_power_setting type;
};


struct link_station_parameters {
	const u8 *mld_mac;
	int link_id;
	const u8 *link_mac;
	const u8 *supported_rates;
	u8 supported_rates_len;
	const struct ieee80211_ht_cap *ht_capa;
	const struct ieee80211_vht_cap *vht_capa;
	u8 opmode_notif;
	bool opmode_notif_used;
	const struct ieee80211_he_cap_elem *he_capa;
	u8 he_capa_len;
	struct sta_txpwr txpwr;
	bool txpwr_set;
	const struct ieee80211_he_6ghz_capa *he_6ghz_capa;
	const struct ieee80211_eht_cap_elem *eht_capa;
	u8 eht_capa_len;
};


struct link_station_del_parameters {
	const u8 *mld_mac;
	u32 link_id;
};


struct station_parameters {
	struct net_device *vlan;
	u32 sta_flags_mask, sta_flags_set;
	u32 sta_modify_mask;
	int listen_interval;
	u16 aid;
	u16 vlan_id;
	u16 peer_aid;
	u8 plink_action;
	u8 plink_state;
	u8 uapsd_queues;
	u8 max_sp;
	enum nl80211_mesh_power_mode local_pm;
	u16 capability;
	const u8 *ext_capab;
	u8 ext_capab_len;
	const u8 *supported_channels;
	u8 supported_channels_len;
	const u8 *supported_oper_classes;
	u8 supported_oper_classes_len;
	int support_p2p_ps;
	u16 airtime_weight;
	struct link_station_parameters link_sta_params;
};


struct station_del_parameters {
	const u8 *mac;
	u8 subtype;
	u16 reason_code;
};


enum cfg80211_station_type {
	CFG80211_STA_AP_CLIENT,
	CFG80211_STA_AP_CLIENT_UNASSOC,
	CFG80211_STA_AP_MLME_CLIENT,
	CFG80211_STA_AP_STA,
	CFG80211_STA_IBSS,
	CFG80211_STA_TDLS_PEER_SETUP,
	CFG80211_STA_TDLS_PEER_ACTIVE,
	CFG80211_STA_MESH_PEER_KERNEL,
	CFG80211_STA_MESH_PEER_USER,
};


int cfg80211_check_station_change(struct wiphy *wiphy,
				  struct station_parameters *params,
				  enum cfg80211_station_type statype);


enum rate_info_flags {
	RATE_INFO_FLAGS_MCS			= BIT(0),
	RATE_INFO_FLAGS_VHT_MCS			= BIT(1),
	RATE_INFO_FLAGS_SHORT_GI		= BIT(2),
	RATE_INFO_FLAGS_DMG			= BIT(3),
	RATE_INFO_FLAGS_HE_MCS			= BIT(4),
	RATE_INFO_FLAGS_EDMG			= BIT(5),
	RATE_INFO_FLAGS_EXTENDED_SC_DMG		= BIT(6),
	RATE_INFO_FLAGS_EHT_MCS			= BIT(7),
	RATE_INFO_FLAGS_S1G_MCS			= BIT(8),
};


enum rate_info_bw {
	RATE_INFO_BW_20 = 0,
	RATE_INFO_BW_5,
	RATE_INFO_BW_10,
	RATE_INFO_BW_40,
	RATE_INFO_BW_80,
	RATE_INFO_BW_160,
	RATE_INFO_BW_HE_RU,
	RATE_INFO_BW_320,
	RATE_INFO_BW_EHT_RU,
	RATE_INFO_BW_1,
	RATE_INFO_BW_2,
	RATE_INFO_BW_4,
	RATE_INFO_BW_8,
	RATE_INFO_BW_16,
};


struct rate_info {
	u16 flags;
	u16 legacy;
	u8 mcs;
	u8 nss;
	u8 bw;
	u8 he_gi;
	u8 he_dcm;
	u8 he_ru_alloc;
	u8 n_bonded_ch;
	u8 eht_gi;
	u8 eht_ru_alloc;
};


enum bss_param_flags {
	BSS_PARAM_FLAGS_CTS_PROT	= 1<<0,
	BSS_PARAM_FLAGS_SHORT_PREAMBLE	= 1<<1,
	BSS_PARAM_FLAGS_SHORT_SLOT_TIME	= 1<<2,
};


struct sta_bss_parameters {
	u8 flags;
	u8 dtim_period;
	u16 beacon_interval;
};


struct cfg80211_txq_stats {
	u32 filled;
	u32 backlog_bytes;
	u32 backlog_packets;
	u32 flows;
	u32 drops;
	u32 ecn_marks;
	u32 overlimit;
	u32 overmemory;
	u32 collisions;
	u32 tx_bytes;
	u32 tx_packets;
	u32 max_flows;
};


struct cfg80211_tid_stats {
	u32 filled;
	u64 rx_msdu;
	u64 tx_msdu;
	u64 tx_msdu_retries;
	u64 tx_msdu_failed;
	struct cfg80211_txq_stats txq_stats;
};

#define IEEE80211_MAX_CHAINS	4


struct station_info {
	u64 filled;
	u32 connected_time;
	u32 inactive_time;
	u64 assoc_at;
	u64 rx_bytes;
	u64 tx_bytes;
	u16 llid;
	u16 plid;
	u8 plink_state;
	s8 signal;
	s8 signal_avg;

	u8 chains;
	s8 chain_signal[IEEE80211_MAX_CHAINS];
	s8 chain_signal_avg[IEEE80211_MAX_CHAINS];

	struct rate_info txrate;
	struct rate_info rxrate;
	u32 rx_packets;
	u32 tx_packets;
	u32 tx_retries;
	u32 tx_failed;
	u32 rx_dropped_misc;
	struct sta_bss_parameters bss_param;
	struct nl80211_sta_flag_update sta_flags;

	int generation;

	const u8 *assoc_req_ies;
	size_t assoc_req_ies_len;

	u32 beacon_loss_count;
	s64 t_offset;
	enum nl80211_mesh_power_mode local_pm;
	enum nl80211_mesh_power_mode peer_pm;
	enum nl80211_mesh_power_mode nonpeer_pm;

	u32 expected_throughput;

	u64 tx_duration;
	u64 rx_duration;
	u64 rx_beacon;
	u8 rx_beacon_signal_avg;
	u8 connected_to_gate;

	struct cfg80211_tid_stats *pertid;
	s8 ack_signal;
	s8 avg_ack_signal;

	u16 airtime_weight;

	u32 rx_mpdu_count;
	u32 fcs_err_count;

	u32 airtime_link_metric;

	u8 connected_to_as;

	bool mlo_params_valid;
	u8 assoc_link_id;
	u8 mld_addr[ETH_ALEN] __aligned(2);
	const u8 *assoc_resp_ies;
	size_t assoc_resp_ies_len;
};


struct cfg80211_sar_sub_specs {
	s32 power;
	u32 freq_range_index;
};


struct cfg80211_sar_specs {
	enum nl80211_sar_type type;
	u32 num_sub_specs;
	struct cfg80211_sar_sub_specs sub_specs[];
};



struct cfg80211_sar_freq_ranges {
	u32 start_freq;
	u32 end_freq;
};


struct cfg80211_sar_capa {
	enum nl80211_sar_type type;
	u32 num_freq_ranges;
	const struct cfg80211_sar_freq_ranges *freq_ranges;
};

#if IS_ENABLED(CONFIG_CFG80211)

int cfg80211_get_station(struct net_device *dev, const u8 *mac_addr,
			 struct station_info *sinfo);
#else
static inline int cfg80211_get_station(struct net_device *dev,
				       const u8 *mac_addr,
				       struct station_info *sinfo)
{
	return -ENOENT;
}
#endif


enum monitor_flags {
	MONITOR_FLAG_CHANGED		= 1<<__NL80211_MNTR_FLAG_INVALID,
	MONITOR_FLAG_FCSFAIL		= 1<<NL80211_MNTR_FLAG_FCSFAIL,
	MONITOR_FLAG_PLCPFAIL		= 1<<NL80211_MNTR_FLAG_PLCPFAIL,
	MONITOR_FLAG_CONTROL		= 1<<NL80211_MNTR_FLAG_CONTROL,
	MONITOR_FLAG_OTHER_BSS		= 1<<NL80211_MNTR_FLAG_OTHER_BSS,
	MONITOR_FLAG_COOK_FRAMES	= 1<<NL80211_MNTR_FLAG_COOK_FRAMES,
	MONITOR_FLAG_ACTIVE		= 1<<NL80211_MNTR_FLAG_ACTIVE,
};


enum mpath_info_flags {
	MPATH_INFO_FRAME_QLEN		= BIT(0),
	MPATH_INFO_SN			= BIT(1),
	MPATH_INFO_METRIC		= BIT(2),
	MPATH_INFO_EXPTIME		= BIT(3),
	MPATH_INFO_DISCOVERY_TIMEOUT	= BIT(4),
	MPATH_INFO_DISCOVERY_RETRIES	= BIT(5),
	MPATH_INFO_FLAGS		= BIT(6),
	MPATH_INFO_HOP_COUNT		= BIT(7),
	MPATH_INFO_PATH_CHANGE		= BIT(8),
};


struct mpath_info {
	u32 filled;
	u32 frame_qlen;
	u32 sn;
	u32 metric;
	u32 exptime;
	u32 discovery_timeout;
	u8 discovery_retries;
	u8 flags;
	u8 hop_count;
	u32 path_change_count;

	int generation;
};


struct bss_parameters {
	int link_id;
	int use_cts_prot;
	int use_short_preamble;
	int use_short_slot_time;
	const u8 *basic_rates;
	u8 basic_rates_len;
	int ap_isolate;
	int ht_opmode;
	s8 p2p_ctwindow, p2p_opp_ps;
};


struct mesh_config {
	u16 dot11MeshRetryTimeout;
	u16 dot11MeshConfirmTimeout;
	u16 dot11MeshHoldingTimeout;
	u16 dot11MeshMaxPeerLinks;
	u8 dot11MeshMaxRetries;
	u8 dot11MeshTTL;
	u8 element_ttl;
	bool auto_open_plinks;
	u32 dot11MeshNbrOffsetMaxNeighbor;
	u8 dot11MeshHWMPmaxPREQretries;
	u32 path_refresh_time;
	u16 min_discovery_timeout;
	u32 dot11MeshHWMPactivePathTimeout;
	u16 dot11MeshHWMPpreqMinInterval;
	u16 dot11MeshHWMPperrMinInterval;
	u16 dot11MeshHWMPnetDiameterTraversalTime;
	u8 dot11MeshHWMPRootMode;
	bool dot11MeshConnectedToMeshGate;
	bool dot11MeshConnectedToAuthServer;
	u16 dot11MeshHWMPRannInterval;
	bool dot11MeshGateAnnouncementProtocol;
	bool dot11MeshForwarding;
	s32 rssi_threshold;
	u16 ht_opmode;
	u32 dot11MeshHWMPactivePathToRootTimeout;
	u16 dot11MeshHWMProotInterval;
	u16 dot11MeshHWMPconfirmationInterval;
	enum nl80211_mesh_power_mode power_mode;
	u16 dot11MeshAwakeWindowDuration;
	u32 plink_timeout;
	bool dot11MeshNolearn;
};


struct mesh_setup {
	struct cfg80211_chan_def chandef;
	const u8 *mesh_id;
	u8 mesh_id_len;
	u8 sync_method;
	u8 path_sel_proto;
	u8 path_metric;
	u8 auth_id;
	const u8 *ie;
	u8 ie_len;
	bool is_authenticated;
	bool is_secure;
	bool user_mpm;
	u8 dtim_period;
	u16 beacon_interval;
	int mcast_rate[NUM_NL80211_BANDS];
	u32 basic_rates;
	struct cfg80211_bitrate_mask beacon_rate;
	bool userspace_handles_dfs;
	bool control_port_over_nl80211;
};


struct ocb_setup {
	struct cfg80211_chan_def chandef;
};


struct ieee80211_txq_params {
	enum nl80211_ac ac;
	u16 txop;
	u16 cwmin;
	u16 cwmax;
	u8 aifs;
	int link_id;
};




struct cfg80211_ssid {
	u8 ssid[IEEE80211_MAX_SSID_LEN];
	u8 ssid_len;
};


struct cfg80211_scan_info {
	u64 scan_start_tsf;
	u8 tsf_bssid[ETH_ALEN] __aligned(2);
	bool aborted;
};


struct cfg80211_scan_6ghz_params {
	u32 short_ssid;
	u32 channel_idx;
	u8 bssid[ETH_ALEN];
	bool unsolicited_probe;
	bool short_ssid_valid;
	bool psc_no_listen;
	s8 psd_20;
};


struct cfg80211_scan_request {
	struct cfg80211_ssid *ssids;
	int n_ssids;
	u32 n_channels;
	enum nl80211_bss_scan_width scan_width;
	const u8 *ie;
	size_t ie_len;
	u16 duration;
	bool duration_mandatory;
	u32 flags;

	u32 rates[NUM_NL80211_BANDS];

	struct wireless_dev *wdev;

	u8 mac_addr[ETH_ALEN] __aligned(2);
	u8 mac_addr_mask[ETH_ALEN] __aligned(2);
	u8 bssid[ETH_ALEN] __aligned(2);

	
	struct wiphy *wiphy;
	unsigned long scan_start;
	struct cfg80211_scan_info info;
	bool notified;
	bool no_cck;
	bool scan_6ghz;
	u32 n_6ghz_params;
	struct cfg80211_scan_6ghz_params *scan_6ghz_params;

	
	struct ieee80211_channel *channels[] __counted_by(n_channels);
};

static inline void get_random_mask_addr(u8 *buf, const u8 *addr, const u8 *mask)
{
	int i;

	get_random_bytes(buf, ETH_ALEN);
	for (i = 0; i < ETH_ALEN; i++) {
		buf[i] &= ~mask[i];
		buf[i] |= addr[i] & mask[i];
	}
}


struct cfg80211_match_set {
	struct cfg80211_ssid ssid;
	u8 bssid[ETH_ALEN];
	s32 rssi_thold;
	s32 per_band_rssi_thold[NUM_NL80211_BANDS];
};


struct cfg80211_sched_scan_plan {
	u32 interval;
	u32 iterations;
};


struct cfg80211_bss_select_adjust {
	enum nl80211_band band;
	s8 delta;
};


struct cfg80211_sched_scan_request {
	u64 reqid;
	struct cfg80211_ssid *ssids;
	int n_ssids;
	u32 n_channels;
	enum nl80211_bss_scan_width scan_width;
	const u8 *ie;
	size_t ie_len;
	u32 flags;
	struct cfg80211_match_set *match_sets;
	int n_match_sets;
	s32 min_rssi_thold;
	u32 delay;
	struct cfg80211_sched_scan_plan *scan_plans;
	int n_scan_plans;

	u8 mac_addr[ETH_ALEN] __aligned(2);
	u8 mac_addr_mask[ETH_ALEN] __aligned(2);

	bool relative_rssi_set;
	s8 relative_rssi;
	struct cfg80211_bss_select_adjust rssi_adjust;

	
	struct wiphy *wiphy;
	struct net_device *dev;
	unsigned long scan_start;
	bool report_results;
	struct rcu_head rcu_head;
	u32 owner_nlportid;
	bool nl_owner_dead;
	struct list_head list;

	
	struct ieee80211_channel *channels[];
};


enum cfg80211_signal_type {
	CFG80211_SIGNAL_TYPE_NONE,
	CFG80211_SIGNAL_TYPE_MBM,
	CFG80211_SIGNAL_TYPE_UNSPEC,
};


struct cfg80211_inform_bss {
	struct ieee80211_channel *chan;
	enum nl80211_bss_scan_width scan_width;
	s32 signal;
	u64 boottime_ns;
	u64 parent_tsf;
	u8 parent_bssid[ETH_ALEN] __aligned(2);
	u8 chains;
	s8 chain_signal[IEEE80211_MAX_CHAINS];

	void *drv_data;
};


struct cfg80211_bss_ies {
	u64 tsf;
	struct rcu_head rcu_head;
	int len;
	bool from_beacon;
	u8 data[];
};


struct cfg80211_bss {
	struct ieee80211_channel *channel;
	enum nl80211_bss_scan_width scan_width;

	const struct cfg80211_bss_ies __rcu *ies;
	const struct cfg80211_bss_ies __rcu *beacon_ies;
	const struct cfg80211_bss_ies __rcu *proberesp_ies;

	struct cfg80211_bss *hidden_beacon_bss;
	struct cfg80211_bss *transmitted_bss;
	struct list_head nontrans_list;

	s32 signal;

	u16 beacon_interval;
	u16 capability;

	u8 bssid[ETH_ALEN];
	u8 chains;
	s8 chain_signal[IEEE80211_MAX_CHAINS];

	u8 bssid_index;
	u8 max_bssid_indicator;

	u8 priv[] __aligned(sizeof(void *));
};


const struct element *ieee80211_bss_get_elem(struct cfg80211_bss *bss, u8 id);


static inline const u8 *ieee80211_bss_get_ie(struct cfg80211_bss *bss, u8 id)
{
	return (const void *)ieee80211_bss_get_elem(bss, id);
}



struct cfg80211_auth_request {
	struct cfg80211_bss *bss;
	const u8 *ie;
	size_t ie_len;
	enum nl80211_auth_type auth_type;
	const u8 *key;
	u8 key_len;
	s8 key_idx;
	const u8 *auth_data;
	size_t auth_data_len;
	s8 link_id;
	const u8 *ap_mld_addr;
};


struct cfg80211_assoc_link {
	struct cfg80211_bss *bss;
	const u8 *elems;
	size_t elems_len;
	bool disabled;
};


enum cfg80211_assoc_req_flags {
	ASSOC_REQ_DISABLE_HT			= BIT(0),
	ASSOC_REQ_DISABLE_VHT			= BIT(1),
	ASSOC_REQ_USE_RRM			= BIT(2),
	CONNECT_REQ_EXTERNAL_AUTH_SUPPORT	= BIT(3),
	ASSOC_REQ_DISABLE_HE			= BIT(4),
	ASSOC_REQ_DISABLE_EHT			= BIT(5),
	CONNECT_REQ_MLO_SUPPORT			= BIT(6),
};


struct cfg80211_assoc_request {
	struct cfg80211_bss *bss;
	const u8 *ie, *prev_bssid;
	size_t ie_len;
	struct cfg80211_crypto_settings crypto;
	bool use_mfp;
	u32 flags;
	struct ieee80211_ht_cap ht_capa;
	struct ieee80211_ht_cap ht_capa_mask;
	struct ieee80211_vht_cap vht_capa, vht_capa_mask;
	const u8 *fils_kek;
	size_t fils_kek_len;
	const u8 *fils_nonces;
	struct ieee80211_s1g_cap s1g_capa, s1g_capa_mask;
	struct cfg80211_assoc_link links[IEEE80211_MLD_MAX_NUM_LINKS];
	const u8 *ap_mld_addr;
	s8 link_id;
};


struct cfg80211_deauth_request {
	const u8 *bssid;
	const u8 *ie;
	size_t ie_len;
	u16 reason_code;
	bool local_state_change;
};


struct cfg80211_disassoc_request {
	const u8 *ap_addr;
	const u8 *ie;
	size_t ie_len;
	u16 reason_code;
	bool local_state_change;
};


struct cfg80211_ibss_params {
	const u8 *ssid;
	const u8 *bssid;
	struct cfg80211_chan_def chandef;
	const u8 *ie;
	u8 ssid_len, ie_len;
	u16 beacon_interval;
	u32 basic_rates;
	bool channel_fixed;
	bool privacy;
	bool control_port;
	bool control_port_over_nl80211;
	bool userspace_handles_dfs;
	int mcast_rate[NUM_NL80211_BANDS];
	struct ieee80211_ht_cap ht_capa;
	struct ieee80211_ht_cap ht_capa_mask;
	struct key_params *wep_keys;
	int wep_tx_key;
};


struct cfg80211_bss_selection {
	enum nl80211_bss_select_attr behaviour;
	union {
		enum nl80211_band band_pref;
		struct cfg80211_bss_select_adjust adjust;
	} param;
};


struct cfg80211_connect_params {
	struct ieee80211_channel *channel;
	struct ieee80211_channel *channel_hint;
	const u8 *bssid;
	const u8 *bssid_hint;
	const u8 *ssid;
	size_t ssid_len;
	enum nl80211_auth_type auth_type;
	const u8 *ie;
	size_t ie_len;
	bool privacy;
	enum nl80211_mfp mfp;
	struct cfg80211_crypto_settings crypto;
	const u8 *key;
	u8 key_len, key_idx;
	u32 flags;
	int bg_scan_period;
	struct ieee80211_ht_cap ht_capa;
	struct ieee80211_ht_cap ht_capa_mask;
	struct ieee80211_vht_cap vht_capa;
	struct ieee80211_vht_cap vht_capa_mask;
	bool pbss;
	struct cfg80211_bss_selection bss_select;
	const u8 *prev_bssid;
	const u8 *fils_erp_username;
	size_t fils_erp_username_len;
	const u8 *fils_erp_realm;
	size_t fils_erp_realm_len;
	u16 fils_erp_next_seq_num;
	const u8 *fils_erp_rrk;
	size_t fils_erp_rrk_len;
	bool want_1x;
	struct ieee80211_edmg edmg;
};


enum cfg80211_connect_params_changed {
	UPDATE_ASSOC_IES		= BIT(0),
	UPDATE_FILS_ERP_INFO		= BIT(1),
	UPDATE_AUTH_TYPE		= BIT(2),
};


enum wiphy_params_flags {
	WIPHY_PARAM_RETRY_SHORT		= 1 << 0,
	WIPHY_PARAM_RETRY_LONG		= 1 << 1,
	WIPHY_PARAM_FRAG_THRESHOLD	= 1 << 2,
	WIPHY_PARAM_RTS_THRESHOLD	= 1 << 3,
	WIPHY_PARAM_COVERAGE_CLASS	= 1 << 4,
	WIPHY_PARAM_DYN_ACK		= 1 << 5,
	WIPHY_PARAM_TXQ_LIMIT		= 1 << 6,
	WIPHY_PARAM_TXQ_MEMORY_LIMIT	= 1 << 7,
	WIPHY_PARAM_TXQ_QUANTUM		= 1 << 8,
};

#define IEEE80211_DEFAULT_AIRTIME_WEIGHT	256


#define IEEE80211_DEFAULT_AQL_TXQ_LIMIT_L	5000
#define IEEE80211_DEFAULT_AQL_TXQ_LIMIT_H	12000


#define IEEE80211_AQL_THRESHOLD			24000


struct cfg80211_pmksa {
	const u8 *bssid;
	const u8 *pmkid;
	const u8 *pmk;
	size_t pmk_len;
	const u8 *ssid;
	size_t ssid_len;
	const u8 *cache_id;
	u32 pmk_lifetime;
	u8 pmk_reauth_threshold;
};


struct cfg80211_pkt_pattern {
	const u8 *mask, *pattern;
	int pattern_len;
	int pkt_offset;
};


struct cfg80211_wowlan_tcp {
	struct socket *sock;
	__be32 src, dst;
	u16 src_port, dst_port;
	u8 dst_mac[ETH_ALEN];
	int payload_len;
	const u8 *payload;
	struct nl80211_wowlan_tcp_data_seq payload_seq;
	u32 data_interval;
	u32 wake_len;
	const u8 *wake_data, *wake_mask;
	u32 tokens_size;
	
	struct nl80211_wowlan_tcp_data_token payload_tok;
};


struct cfg80211_wowlan {
	bool any, disconnect, magic_pkt, gtk_rekey_failure,
	     eap_identity_req, four_way_handshake,
	     rfkill_release;
	struct cfg80211_pkt_pattern *patterns;
	struct cfg80211_wowlan_tcp *tcp;
	int n_patterns;
	struct cfg80211_sched_scan_request *nd_config;
};


struct cfg80211_coalesce_rules {
	int delay;
	enum nl80211_coalesce_condition condition;
	struct cfg80211_pkt_pattern *patterns;
	int n_patterns;
};


struct cfg80211_coalesce {
	struct cfg80211_coalesce_rules *rules;
	int n_rules;
};


struct cfg80211_wowlan_nd_match {
	struct cfg80211_ssid ssid;
	int n_channels;
	u32 channels[];
};


struct cfg80211_wowlan_nd_info {
	int n_matches;
	struct cfg80211_wowlan_nd_match *matches[];
};


struct cfg80211_wowlan_wakeup {
	bool disconnect, magic_pkt, gtk_rekey_failure,
	     eap_identity_req, four_way_handshake,
	     rfkill_release, packet_80211,
	     tcp_match, tcp_connlost, tcp_nomoretokens;
	s32 pattern_idx;
	u32 packet_present_len, packet_len;
	const void *packet;
	struct cfg80211_wowlan_nd_info *net_detect;
};


struct cfg80211_gtk_rekey_data {
	const u8 *kek, *kck, *replay_ctr;
	u32 akm;
	u8 kek_len, kck_len;
};


struct cfg80211_update_ft_ies_params {
	u16 md;
	const u8 *ie;
	size_t ie_len;
};


struct cfg80211_mgmt_tx_params {
	struct ieee80211_channel *chan;
	bool offchan;
	unsigned int wait;
	const u8 *buf;
	size_t len;
	bool no_cck;
	bool dont_wait_for_ack;
	int n_csa_offsets;
	const u16 *csa_offsets;
	int link_id;
};


struct cfg80211_dscp_exception {
	u8 dscp;
	u8 up;
};


struct cfg80211_dscp_range {
	u8 low;
	u8 high;
};


#define IEEE80211_QOS_MAP_MAX_EX	21
#define IEEE80211_QOS_MAP_LEN_MIN	16
#define IEEE80211_QOS_MAP_LEN_MAX \
	(IEEE80211_QOS_MAP_LEN_MIN + 2 * IEEE80211_QOS_MAP_MAX_EX)


struct cfg80211_qos_map {
	u8 num_des;
	struct cfg80211_dscp_exception dscp_exception[IEEE80211_QOS_MAP_MAX_EX];
	struct cfg80211_dscp_range up[8];
};


struct cfg80211_nan_conf {
	u8 master_pref;
	u8 bands;
};


enum cfg80211_nan_conf_changes {
	CFG80211_NAN_CONF_CHANGED_PREF = BIT(0),
	CFG80211_NAN_CONF_CHANGED_BANDS = BIT(1),
};


struct cfg80211_nan_func_filter {
	const u8 *filter;
	u8 len;
};


struct cfg80211_nan_func {
	enum nl80211_nan_function_type type;
	u8 service_id[NL80211_NAN_FUNC_SERVICE_ID_LEN];
	u8 publish_type;
	bool close_range;
	bool publish_bcast;
	bool subscribe_active;
	u8 followup_id;
	u8 followup_reqid;
	struct mac_address followup_dest;
	u32 ttl;
	const u8 *serv_spec_info;
	u8 serv_spec_info_len;
	bool srf_include;
	const u8 *srf_bf;
	u8 srf_bf_len;
	u8 srf_bf_idx;
	struct mac_address *srf_macs;
	int srf_num_macs;
	struct cfg80211_nan_func_filter *rx_filters;
	struct cfg80211_nan_func_filter *tx_filters;
	u8 num_tx_filters;
	u8 num_rx_filters;
	u8 instance_id;
	u64 cookie;
};


struct cfg80211_pmk_conf {
	const u8 *aa;
	u8 pmk_len;
	const u8 *pmk;
	const u8 *pmk_r0_name;
};


struct cfg80211_external_auth_params {
	enum nl80211_external_auth_action action;
	u8 bssid[ETH_ALEN] __aligned(2);
	struct cfg80211_ssid ssid;
	unsigned int key_mgmt_suite;
	u16 status;
	const u8 *pmkid;
	u8 mld_addr[ETH_ALEN] __aligned(2);
};


struct cfg80211_ftm_responder_stats {
	u32 filled;
	u32 success_num;
	u32 partial_num;
	u32 failed_num;
	u32 asap_num;
	u32 non_asap_num;
	u64 total_duration_ms;
	u32 unknown_triggers_num;
	u32 reschedule_requests_num;
	u32 out_of_window_triggers_num;
};


struct cfg80211_pmsr_ftm_result {
	const u8 *lci;
	const u8 *civicloc;
	unsigned int lci_len;
	unsigned int civicloc_len;
	enum nl80211_peer_measurement_ftm_failure_reasons failure_reason;
	u32 num_ftmr_attempts, num_ftmr_successes;
	s16 burst_index;
	u8 busy_retry_time;
	u8 num_bursts_exp;
	u8 burst_duration;
	u8 ftms_per_burst;
	s32 rssi_avg;
	s32 rssi_spread;
	struct rate_info tx_rate, rx_rate;
	s64 rtt_avg;
	s64 rtt_variance;
	s64 rtt_spread;
	s64 dist_avg;
	s64 dist_variance;
	s64 dist_spread;

	u16 num_ftmr_attempts_valid:1,
	    num_ftmr_successes_valid:1,
	    rssi_avg_valid:1,
	    rssi_spread_valid:1,
	    tx_rate_valid:1,
	    rx_rate_valid:1,
	    rtt_avg_valid:1,
	    rtt_variance_valid:1,
	    rtt_spread_valid:1,
	    dist_avg_valid:1,
	    dist_variance_valid:1,
	    dist_spread_valid:1;
};


struct cfg80211_pmsr_result {
	u64 host_time, ap_tsf;
	enum nl80211_peer_measurement_status status;

	u8 addr[ETH_ALEN];

	u8 final:1,
	   ap_tsf_valid:1;

	enum nl80211_peer_measurement_type type;

	union {
		struct cfg80211_pmsr_ftm_result ftm;
	};
};


struct cfg80211_pmsr_ftm_request_peer {
	enum nl80211_preamble preamble;
	u16 burst_period;
	u8 requested:1,
	   asap:1,
	   request_lci:1,
	   request_civicloc:1,
	   trigger_based:1,
	   non_trigger_based:1,
	   lmr_feedback:1;
	u8 num_bursts_exp;
	u8 burst_duration;
	u8 ftms_per_burst;
	u8 ftmr_retries;
	u8 bss_color;
};


struct cfg80211_pmsr_request_peer {
	u8 addr[ETH_ALEN];
	struct cfg80211_chan_def chandef;
	u8 report_ap_tsf:1;
	struct cfg80211_pmsr_ftm_request_peer ftm;
};


struct cfg80211_pmsr_request {
	u64 cookie;
	void *drv_data;
	u32 n_peers;
	u32 nl_portid;

	u32 timeout;

	u8 mac_addr[ETH_ALEN] __aligned(2);
	u8 mac_addr_mask[ETH_ALEN] __aligned(2);

	struct list_head list;

	struct cfg80211_pmsr_request_peer peers[] __counted_by(n_peers);
};


struct cfg80211_update_owe_info {
	u8 peer[ETH_ALEN] __aligned(2);
	u16 status;
	const u8 *ie;
	size_t ie_len;
	int assoc_link_id;
	u8 peer_mld_addr[ETH_ALEN] __aligned(2);
};


struct mgmt_frame_regs {
	u32 global_stypes, interface_stypes;
	u32 global_mcast_stypes, interface_mcast_stypes;
};


struct cfg80211_ops {
	int	(*suspend)(struct wiphy *wiphy, struct cfg80211_wowlan *wow);
	int	(*resume)(struct wiphy *wiphy);
	void	(*set_wakeup)(struct wiphy *wiphy, bool enabled);

	struct wireless_dev * (*add_virtual_intf)(struct wiphy *wiphy,
						  const char *name,
						  unsigned char name_assign_type,
						  enum nl80211_iftype type,
						  struct vif_params *params);
	int	(*del_virtual_intf)(struct wiphy *wiphy,
				    struct wireless_dev *wdev);
	int	(*change_virtual_intf)(struct wiphy *wiphy,
				       struct net_device *dev,
				       enum nl80211_iftype type,
				       struct vif_params *params);

	int	(*add_intf_link)(struct wiphy *wiphy,
				 struct wireless_dev *wdev,
				 unsigned int link_id);
	void	(*del_intf_link)(struct wiphy *wiphy,
				 struct wireless_dev *wdev,
				 unsigned int link_id);

	int	(*add_key)(struct wiphy *wiphy, struct net_device *netdev,
			   int link_id, u8 key_index, bool pairwise,
			   const u8 *mac_addr, struct key_params *params);
	int	(*get_key)(struct wiphy *wiphy, struct net_device *netdev,
			   int link_id, u8 key_index, bool pairwise,
			   const u8 *mac_addr, void *cookie,
			   void (*callback)(void *cookie, struct key_params*));
	int	(*del_key)(struct wiphy *wiphy, struct net_device *netdev,
			   int link_id, u8 key_index, bool pairwise,
			   const u8 *mac_addr);
	int	(*set_default_key)(struct wiphy *wiphy,
				   struct net_device *netdev, int link_id,
				   u8 key_index, bool unicast, bool multicast);
	int	(*set_default_mgmt_key)(struct wiphy *wiphy,
					struct net_device *netdev, int link_id,
					u8 key_index);
	int	(*set_default_beacon_key)(struct wiphy *wiphy,
					  struct net_device *netdev,
					  int link_id,
					  u8 key_index);

	int	(*start_ap)(struct wiphy *wiphy, struct net_device *dev,
			    struct cfg80211_ap_settings *settings);
	int	(*change_beacon)(struct wiphy *wiphy, struct net_device *dev,
				 struct cfg80211_beacon_data *info);
	int	(*stop_ap)(struct wiphy *wiphy, struct net_device *dev,
			   unsigned int link_id);


	int	(*add_station)(struct wiphy *wiphy, struct net_device *dev,
			       const u8 *mac,
			       struct station_parameters *params);
	int	(*del_station)(struct wiphy *wiphy, struct net_device *dev,
			       struct station_del_parameters *params);
	int	(*change_station)(struct wiphy *wiphy, struct net_device *dev,
				  const u8 *mac,
				  struct station_parameters *params);
	int	(*get_station)(struct wiphy *wiphy, struct net_device *dev,
			       const u8 *mac, struct station_info *sinfo);
	int	(*dump_station)(struct wiphy *wiphy, struct net_device *dev,
				int idx, u8 *mac, struct station_info *sinfo);

	int	(*add_mpath)(struct wiphy *wiphy, struct net_device *dev,
			       const u8 *dst, const u8 *next_hop);
	int	(*del_mpath)(struct wiphy *wiphy, struct net_device *dev,
			       const u8 *dst);
	int	(*change_mpath)(struct wiphy *wiphy, struct net_device *dev,
				  const u8 *dst, const u8 *next_hop);
	int	(*get_mpath)(struct wiphy *wiphy, struct net_device *dev,
			     u8 *dst, u8 *next_hop, struct mpath_info *pinfo);
	int	(*dump_mpath)(struct wiphy *wiphy, struct net_device *dev,
			      int idx, u8 *dst, u8 *next_hop,
			      struct mpath_info *pinfo);
	int	(*get_mpp)(struct wiphy *wiphy, struct net_device *dev,
			   u8 *dst, u8 *mpp, struct mpath_info *pinfo);
	int	(*dump_mpp)(struct wiphy *wiphy, struct net_device *dev,
			    int idx, u8 *dst, u8 *mpp,
			    struct mpath_info *pinfo);
	int	(*get_mesh_config)(struct wiphy *wiphy,
				struct net_device *dev,
				struct mesh_config *conf);
	int	(*update_mesh_config)(struct wiphy *wiphy,
				      struct net_device *dev, u32 mask,
				      const struct mesh_config *nconf);
	int	(*join_mesh)(struct wiphy *wiphy, struct net_device *dev,
			     const struct mesh_config *conf,
			     const struct mesh_setup *setup);
	int	(*leave_mesh)(struct wiphy *wiphy, struct net_device *dev);

	int	(*join_ocb)(struct wiphy *wiphy, struct net_device *dev,
			    struct ocb_setup *setup);
	int	(*leave_ocb)(struct wiphy *wiphy, struct net_device *dev);

	int	(*change_bss)(struct wiphy *wiphy, struct net_device *dev,
			      struct bss_parameters *params);

	void	(*inform_bss)(struct wiphy *wiphy, struct cfg80211_bss *bss,
			      const struct cfg80211_bss_ies *ies, void *data);

	int	(*set_txq_params)(struct wiphy *wiphy, struct net_device *dev,
				  struct ieee80211_txq_params *params);

	int	(*libertas_set_mesh_channel)(struct wiphy *wiphy,
					     struct net_device *dev,
					     struct ieee80211_channel *chan);

	int	(*set_monitor_channel)(struct wiphy *wiphy,
				       struct cfg80211_chan_def *chandef);

	int	(*scan)(struct wiphy *wiphy,
			struct cfg80211_scan_request *request);
	void	(*abort_scan)(struct wiphy *wiphy, struct wireless_dev *wdev);

	int	(*auth)(struct wiphy *wiphy, struct net_device *dev,
			struct cfg80211_auth_request *req);
	int	(*assoc)(struct wiphy *wiphy, struct net_device *dev,
			 struct cfg80211_assoc_request *req);
	int	(*deauth)(struct wiphy *wiphy, struct net_device *dev,
			  struct cfg80211_deauth_request *req);
	int	(*disassoc)(struct wiphy *wiphy, struct net_device *dev,
			    struct cfg80211_disassoc_request *req);

	int	(*connect)(struct wiphy *wiphy, struct net_device *dev,
			   struct cfg80211_connect_params *sme);
	int	(*update_connect_params)(struct wiphy *wiphy,
					 struct net_device *dev,
					 struct cfg80211_connect_params *sme,
					 u32 changed);
	int	(*disconnect)(struct wiphy *wiphy, struct net_device *dev,
			      u16 reason_code);

	int	(*join_ibss)(struct wiphy *wiphy, struct net_device *dev,
			     struct cfg80211_ibss_params *params);
	int	(*leave_ibss)(struct wiphy *wiphy, struct net_device *dev);

	int	(*set_mcast_rate)(struct wiphy *wiphy, struct net_device *dev,
				  int rate[NUM_NL80211_BANDS]);

	int	(*set_wiphy_params)(struct wiphy *wiphy, u32 changed);

	int	(*set_tx_power)(struct wiphy *wiphy, struct wireless_dev *wdev,
				enum nl80211_tx_power_setting type, int mbm);
	int	(*get_tx_power)(struct wiphy *wiphy, struct wireless_dev *wdev,
				int *dbm);

	void	(*rfkill_poll)(struct wiphy *wiphy);

#ifdef CONFIG_NL80211_TESTMODE
	int	(*testmode_cmd)(struct wiphy *wiphy, struct wireless_dev *wdev,
				void *data, int len);
	int	(*testmode_dump)(struct wiphy *wiphy, struct sk_buff *skb,
				 struct netlink_callback *cb,
				 void *data, int len);
#endif

	int	(*set_bitrate_mask)(struct wiphy *wiphy,
				    struct net_device *dev,
				    unsigned int link_id,
				    const u8 *peer,
				    const struct cfg80211_bitrate_mask *mask);

	int	(*dump_survey)(struct wiphy *wiphy, struct net_device *netdev,
			int idx, struct survey_info *info);

	int	(*set_pmksa)(struct wiphy *wiphy, struct net_device *netdev,
			     struct cfg80211_pmksa *pmksa);
	int	(*del_pmksa)(struct wiphy *wiphy, struct net_device *netdev,
			     struct cfg80211_pmksa *pmksa);
	int	(*flush_pmksa)(struct wiphy *wiphy, struct net_device *netdev);

	int	(*remain_on_channel)(struct wiphy *wiphy,
				     struct wireless_dev *wdev,
				     struct ieee80211_channel *chan,
				     unsigned int duration,
				     u64 *cookie);
	int	(*cancel_remain_on_channel)(struct wiphy *wiphy,
					    struct wireless_dev *wdev,
					    u64 cookie);

	int	(*mgmt_tx)(struct wiphy *wiphy, struct wireless_dev *wdev,
			   struct cfg80211_mgmt_tx_params *params,
			   u64 *cookie);
	int	(*mgmt_tx_cancel_wait)(struct wiphy *wiphy,
				       struct wireless_dev *wdev,
				       u64 cookie);

	int	(*set_power_mgmt)(struct wiphy *wiphy, struct net_device *dev,
				  bool enabled, int timeout);

	int	(*set_cqm_rssi_config)(struct wiphy *wiphy,
				       struct net_device *dev,
				       s32 rssi_thold, u32 rssi_hyst);

	int	(*set_cqm_rssi_range_config)(struct wiphy *wiphy,
					     struct net_device *dev,
					     s32 rssi_low, s32 rssi_high);

	int	(*set_cqm_txe_config)(struct wiphy *wiphy,
				      struct net_device *dev,
				      u32 rate, u32 pkts, u32 intvl);

	void	(*update_mgmt_frame_registrations)(struct wiphy *wiphy,
						   struct wireless_dev *wdev,
						   struct mgmt_frame_regs *upd);

	int	(*set_antenna)(struct wiphy *wiphy, u32 tx_ant, u32 rx_ant);
	int	(*get_antenna)(struct wiphy *wiphy, u32 *tx_ant, u32 *rx_ant);

	int	(*sched_scan_start)(struct wiphy *wiphy,
				struct net_device *dev,
				struct cfg80211_sched_scan_request *request);
	int	(*sched_scan_stop)(struct wiphy *wiphy, struct net_device *dev,
				   u64 reqid);

	int	(*set_rekey_data)(struct wiphy *wiphy, struct net_device *dev,
				  struct cfg80211_gtk_rekey_data *data);

	int	(*tdls_mgmt)(struct wiphy *wiphy, struct net_device *dev,
			     const u8 *peer, int link_id,
			     u8 action_code, u8 dialog_token, u16 status_code,
			     u32 peer_capability, bool initiator,
			     const u8 *buf, size_t len);
	int	(*tdls_oper)(struct wiphy *wiphy, struct net_device *dev,
			     const u8 *peer, enum nl80211_tdls_operation oper);

	int	(*probe_client)(struct wiphy *wiphy, struct net_device *dev,
				const u8 *peer, u64 *cookie);

	int	(*set_noack_map)(struct wiphy *wiphy,
				  struct net_device *dev,
				  u16 noack_map);

	int	(*get_channel)(struct wiphy *wiphy,
			       struct wireless_dev *wdev,
			       unsigned int link_id,
			       struct cfg80211_chan_def *chandef);

	int	(*start_p2p_device)(struct wiphy *wiphy,
				    struct wireless_dev *wdev);
	void	(*stop_p2p_device)(struct wiphy *wiphy,
				   struct wireless_dev *wdev);

	int	(*set_mac_acl)(struct wiphy *wiphy, struct net_device *dev,
			       const struct cfg80211_acl_data *params);

	int	(*start_radar_detection)(struct wiphy *wiphy,
					 struct net_device *dev,
					 struct cfg80211_chan_def *chandef,
					 u32 cac_time_ms);
	void	(*end_cac)(struct wiphy *wiphy,
				struct net_device *dev);
	int	(*update_ft_ies)(struct wiphy *wiphy, struct net_device *dev,
				 struct cfg80211_update_ft_ies_params *ftie);
	int	(*crit_proto_start)(struct wiphy *wiphy,
				    struct wireless_dev *wdev,
				    enum nl80211_crit_proto_id protocol,
				    u16 duration);
	void	(*crit_proto_stop)(struct wiphy *wiphy,
				   struct wireless_dev *wdev);
	int	(*set_coalesce)(struct wiphy *wiphy,
				struct cfg80211_coalesce *coalesce);

	int	(*channel_switch)(struct wiphy *wiphy,
				  struct net_device *dev,
				  struct cfg80211_csa_settings *params);

	int     (*set_qos_map)(struct wiphy *wiphy,
			       struct net_device *dev,
			       struct cfg80211_qos_map *qos_map);

	int	(*set_ap_chanwidth)(struct wiphy *wiphy, struct net_device *dev,
				    unsigned int link_id,
				    struct cfg80211_chan_def *chandef);

	int	(*add_tx_ts)(struct wiphy *wiphy, struct net_device *dev,
			     u8 tsid, const u8 *peer, u8 user_prio,
			     u16 admitted_time);
	int	(*del_tx_ts)(struct wiphy *wiphy, struct net_device *dev,
			     u8 tsid, const u8 *peer);

	int	(*tdls_channel_switch)(struct wiphy *wiphy,
				       struct net_device *dev,
				       const u8 *addr, u8 oper_class,
				       struct cfg80211_chan_def *chandef);
	void	(*tdls_cancel_channel_switch)(struct wiphy *wiphy,
					      struct net_device *dev,
					      const u8 *addr);
	int	(*start_nan)(struct wiphy *wiphy, struct wireless_dev *wdev,
			     struct cfg80211_nan_conf *conf);
	void	(*stop_nan)(struct wiphy *wiphy, struct wireless_dev *wdev);
	int	(*add_nan_func)(struct wiphy *wiphy, struct wireless_dev *wdev,
				struct cfg80211_nan_func *nan_func);
	void	(*del_nan_func)(struct wiphy *wiphy, struct wireless_dev *wdev,
			       u64 cookie);
	int	(*nan_change_conf)(struct wiphy *wiphy,
				   struct wireless_dev *wdev,
				   struct cfg80211_nan_conf *conf,
				   u32 changes);

	int	(*set_multicast_to_unicast)(struct wiphy *wiphy,
					    struct net_device *dev,
					    const bool enabled);

	int	(*get_txq_stats)(struct wiphy *wiphy,
				 struct wireless_dev *wdev,
				 struct cfg80211_txq_stats *txqstats);

	int	(*set_pmk)(struct wiphy *wiphy, struct net_device *dev,
			   const struct cfg80211_pmk_conf *conf);
	int	(*del_pmk)(struct wiphy *wiphy, struct net_device *dev,
			   const u8 *aa);
	int     (*external_auth)(struct wiphy *wiphy, struct net_device *dev,
				 struct cfg80211_external_auth_params *params);

	int	(*tx_control_port)(struct wiphy *wiphy,
				   struct net_device *dev,
				   const u8 *buf, size_t len,
				   const u8 *dest, const __be16 proto,
				   const bool noencrypt, int link_id,
				   u64 *cookie);

	int	(*get_ftm_responder_stats)(struct wiphy *wiphy,
				struct net_device *dev,
				struct cfg80211_ftm_responder_stats *ftm_stats);

	int	(*start_pmsr)(struct wiphy *wiphy, struct wireless_dev *wdev,
			      struct cfg80211_pmsr_request *request);
	void	(*abort_pmsr)(struct wiphy *wiphy, struct wireless_dev *wdev,
			      struct cfg80211_pmsr_request *request);
	int	(*update_owe_info)(struct wiphy *wiphy, struct net_device *dev,
				   struct cfg80211_update_owe_info *owe_info);
	int	(*probe_mesh_link)(struct wiphy *wiphy, struct net_device *dev,
				   const u8 *buf, size_t len);
	int     (*set_tid_config)(struct wiphy *wiphy, struct net_device *dev,
				  struct cfg80211_tid_config *tid_conf);
	int	(*reset_tid_config)(struct wiphy *wiphy, struct net_device *dev,
				    const u8 *peer, u8 tids);
	int	(*set_sar_specs)(struct wiphy *wiphy,
				 struct cfg80211_sar_specs *sar);
	int	(*color_change)(struct wiphy *wiphy,
				struct net_device *dev,
				struct cfg80211_color_change_settings *params);
	int     (*set_fils_aad)(struct wiphy *wiphy, struct net_device *dev,
				struct cfg80211_fils_aad *fils_aad);
	int	(*set_radar_background)(struct wiphy *wiphy,
					struct cfg80211_chan_def *chandef);
	int	(*add_link_station)(struct wiphy *wiphy, struct net_device *dev,
				    struct link_station_parameters *params);
	int	(*mod_link_station)(struct wiphy *wiphy, struct net_device *dev,
				    struct link_station_parameters *params);
	int	(*del_link_station)(struct wiphy *wiphy, struct net_device *dev,
				    struct link_station_del_parameters *params);
	int	(*set_hw_timestamp)(struct wiphy *wiphy, struct net_device *dev,
				    struct cfg80211_set_hw_timestamp *hwts);
};




enum wiphy_flags {
	WIPHY_FLAG_SUPPORTS_EXT_KEK_KCK		= BIT(0),
	WIPHY_FLAG_SUPPORTS_MLO			= BIT(1),
	WIPHY_FLAG_SPLIT_SCAN_6GHZ		= BIT(2),
	WIPHY_FLAG_NETNS_OK			= BIT(3),
	WIPHY_FLAG_PS_ON_BY_DEFAULT		= BIT(4),
	WIPHY_FLAG_4ADDR_AP			= BIT(5),
	WIPHY_FLAG_4ADDR_STATION		= BIT(6),
	WIPHY_FLAG_CONTROL_PORT_PROTOCOL	= BIT(7),
	WIPHY_FLAG_IBSS_RSN			= BIT(8),
	WIPHY_FLAG_DISABLE_WEXT			= BIT(9),
	WIPHY_FLAG_MESH_AUTH			= BIT(10),
	WIPHY_FLAG_SUPPORTS_EXT_KCK_32          = BIT(11),
	
	WIPHY_FLAG_SUPPORTS_FW_ROAM		= BIT(13),
	WIPHY_FLAG_AP_UAPSD			= BIT(14),
	WIPHY_FLAG_SUPPORTS_TDLS		= BIT(15),
	WIPHY_FLAG_TDLS_EXTERNAL_SETUP		= BIT(16),
	WIPHY_FLAG_HAVE_AP_SME			= BIT(17),
	WIPHY_FLAG_REPORTS_OBSS			= BIT(18),
	WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD	= BIT(19),
	WIPHY_FLAG_OFFCHAN_TX			= BIT(20),
	WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL	= BIT(21),
	WIPHY_FLAG_SUPPORTS_5_10_MHZ		= BIT(22),
	WIPHY_FLAG_HAS_CHANNEL_SWITCH		= BIT(23),
	WIPHY_FLAG_NOTIFY_REGDOM_BY_DRIVER	= BIT(24),
};


struct ieee80211_iface_limit {
	u16 max;
	u16 types;
};


struct ieee80211_iface_combination {
	
	const struct ieee80211_iface_limit *limits;

	
	u32 num_different_channels;

	
	u16 max_interfaces;

	
	u8 n_limits;

	
	bool beacon_int_infra_match;

	
	u8 radar_detect_widths;

	
	u8 radar_detect_regions;

	
	u32 beacon_int_min_gcd;
};

struct ieee80211_txrx_stypes {
	u16 tx, rx;
};


enum wiphy_wowlan_support_flags {
	WIPHY_WOWLAN_ANY		= BIT(0),
	WIPHY_WOWLAN_MAGIC_PKT		= BIT(1),
	WIPHY_WOWLAN_DISCONNECT		= BIT(2),
	WIPHY_WOWLAN_SUPPORTS_GTK_REKEY	= BIT(3),
	WIPHY_WOWLAN_GTK_REKEY_FAILURE	= BIT(4),
	WIPHY_WOWLAN_EAP_IDENTITY_REQ	= BIT(5),
	WIPHY_WOWLAN_4WAY_HANDSHAKE	= BIT(6),
	WIPHY_WOWLAN_RFKILL_RELEASE	= BIT(7),
	WIPHY_WOWLAN_NET_DETECT		= BIT(8),
};

struct wiphy_wowlan_tcp_support {
	const struct nl80211_wowlan_tcp_data_token_feature *tok;
	u32 data_payload_max;
	u32 data_interval_max;
	u32 wake_payload_max;
	bool seq;
};


struct wiphy_wowlan_support {
	u32 flags;
	int n_patterns;
	int pattern_max_len;
	int pattern_min_len;
	int max_pkt_offset;
	int max_nd_match_sets;
	const struct wiphy_wowlan_tcp_support *tcp;
};


struct wiphy_coalesce_support {
	int n_rules;
	int max_delay;
	int n_patterns;
	int pattern_max_len;
	int pattern_min_len;
	int max_pkt_offset;
};


enum wiphy_vendor_command_flags {
	WIPHY_VENDOR_CMD_NEED_WDEV = BIT(0),
	WIPHY_VENDOR_CMD_NEED_NETDEV = BIT(1),
	WIPHY_VENDOR_CMD_NEED_RUNNING = BIT(2),
};


enum wiphy_opmode_flag {
	STA_OPMODE_MAX_BW_CHANGED	= BIT(0),
	STA_OPMODE_SMPS_MODE_CHANGED	= BIT(1),
	STA_OPMODE_N_SS_CHANGED		= BIT(2),
};



struct sta_opmode_info {
	u32 changed;
	enum nl80211_smps_mode smps_mode;
	enum nl80211_chan_width bw;
	u8 rx_nss;
};

#define VENDOR_CMD_RAW_DATA ((const struct nla_policy *)(long)(-ENODATA))


struct wiphy_vendor_command {
	struct nl80211_vendor_cmd_info info;
	u32 flags;
	int (*doit)(struct wiphy *wiphy, struct wireless_dev *wdev,
		    const void *data, int data_len);
	int (*dumpit)(struct wiphy *wiphy, struct wireless_dev *wdev,
		      struct sk_buff *skb, const void *data, int data_len,
		      unsigned long *storage);
	const struct nla_policy *policy;
	unsigned int maxattr;
};


struct wiphy_iftype_ext_capab {
	enum nl80211_iftype iftype;
	const u8 *extended_capabilities;
	const u8 *extended_capabilities_mask;
	u8 extended_capabilities_len;
	u16 eml_capabilities;
	u16 mld_capa_and_ops;
};


const struct wiphy_iftype_ext_capab *
cfg80211_get_iftype_ext_capa(struct wiphy *wiphy, enum nl80211_iftype type);


struct cfg80211_pmsr_capabilities {
	unsigned int max_peers;
	u8 report_ap_tsf:1,
	   randomize_mac_addr:1;

	struct {
		u32 preambles;
		u32 bandwidths;
		s8 max_bursts_exponent;
		u8 max_ftms_per_burst;
		u8 supported:1,
		   asap:1,
		   non_asap:1,
		   request_lci:1,
		   request_civicloc:1,
		   trigger_based:1,
		   non_trigger_based:1;
	} ftm;
};


struct wiphy_iftype_akm_suites {
	u16 iftypes_mask;
	const u32 *akm_suites;
	int n_akm_suites;
};

#define CFG80211_HW_TIMESTAMP_ALL_PEERS	0xffff


struct wiphy {
	struct mutex mtx;

	

	u8 perm_addr[ETH_ALEN];
	u8 addr_mask[ETH_ALEN];

	struct mac_address *addresses;

	const struct ieee80211_txrx_stypes *mgmt_stypes;

	const struct ieee80211_iface_combination *iface_combinations;
	int n_iface_combinations;
	u16 software_iftypes;

	u16 n_addresses;

	
	u16 interface_modes;

	u16 max_acl_mac_addrs;

	u32 flags, regulatory_flags, features;
	u8 ext_features[DIV_ROUND_UP(NUM_NL80211_EXT_FEATURES, 8)];

	u32 ap_sme_capa;

	enum cfg80211_signal_type signal_type;

	int bss_priv_size;
	u8 max_scan_ssids;
	u8 max_sched_scan_reqs;
	u8 max_sched_scan_ssids;
	u8 max_match_sets;
	u16 max_scan_ie_len;
	u16 max_sched_scan_ie_len;
	u32 max_sched_scan_plans;
	u32 max_sched_scan_plan_interval;
	u32 max_sched_scan_plan_iterations;

	int n_cipher_suites;
	const u32 *cipher_suites;

	int n_akm_suites;
	const u32 *akm_suites;

	const struct wiphy_iftype_akm_suites *iftype_akm_suites;
	unsigned int num_iftype_akm_suites;

	u8 retry_short;
	u8 retry_long;
	u32 frag_threshold;
	u32 rts_threshold;
	u8 coverage_class;

	char fw_version[ETHTOOL_FWVERS_LEN];
	u32 hw_version;

#ifdef CONFIG_PM
	const struct wiphy_wowlan_support *wowlan;
	struct cfg80211_wowlan *wowlan_config;
#endif

	u16 max_remain_on_channel_duration;

	u8 max_num_pmkids;

	u32 available_antennas_tx;
	u32 available_antennas_rx;

	u32 probe_resp_offload;

	const u8 *extended_capabilities, *extended_capabilities_mask;
	u8 extended_capabilities_len;

	const struct wiphy_iftype_ext_capab *iftype_ext_capab;
	unsigned int num_iftype_ext_capab;

	const void *privid;

	struct ieee80211_supported_band *bands[NUM_NL80211_BANDS];

	void (*reg_notifier)(struct wiphy *wiphy,
			     struct regulatory_request *request);

	

	const struct ieee80211_regdomain __rcu *regd;

	struct device dev;

	bool registered;

	struct dentry *debugfsdir;

	const struct ieee80211_ht_cap *ht_capa_mod_mask;
	const struct ieee80211_vht_cap *vht_capa_mod_mask;

	struct list_head wdev_list;

	possible_net_t _net;

#ifdef CONFIG_CFG80211_WEXT
	const struct iw_handler_def *wext;
#endif

	const struct wiphy_coalesce_support *coalesce;

	const struct wiphy_vendor_command *vendor_commands;
	const struct nl80211_vendor_cmd_info *vendor_events;
	int n_vendor_commands, n_vendor_events;

	u16 max_ap_assoc_sta;

	u8 max_num_csa_counters;

	u32 bss_select_support;

	u8 nan_supported_bands;

	u32 txq_limit;
	u32 txq_memory_limit;
	u32 txq_quantum;

	unsigned long tx_queue_len;

	u8 support_mbssid:1,
	   support_only_he_mbssid:1;

	const struct cfg80211_pmsr_capabilities *pmsr_capa;

	struct {
		u64 peer, vif;
		u8 max_retry;
	} tid_config_support;

	u8 max_data_retry_count;

	const struct cfg80211_sar_capa *sar_capa;

	struct rfkill *rfkill;

	u8 mbssid_max_interfaces;
	u8 ema_max_profile_periodicity;
	u16 max_num_akm_suites;

	u16 hw_timestamp_max_peers;

	char priv[] __aligned(NETDEV_ALIGN);
};

static inline struct net *wiphy_net(struct wiphy *wiphy)
{
	return read_pnet(&wiphy->_net);
}

static inline void wiphy_net_set(struct wiphy *wiphy, struct net *net)
{
	write_pnet(&wiphy->_net, net);
}


static inline void *wiphy_priv(struct wiphy *wiphy)
{
	BUG_ON(!wiphy);
	return &wiphy->priv;
}


static inline struct wiphy *priv_to_wiphy(void *priv)
{
	BUG_ON(!priv);
	return container_of(priv, struct wiphy, priv);
}


static inline void set_wiphy_dev(struct wiphy *wiphy, struct device *dev)
{
	wiphy->dev.parent = dev;
}


static inline struct device *wiphy_dev(struct wiphy *wiphy)
{
	return wiphy->dev.parent;
}


static inline const char *wiphy_name(const struct wiphy *wiphy)
{
	return dev_name(&wiphy->dev);
}


struct wiphy *wiphy_new_nm(const struct cfg80211_ops *ops, int sizeof_priv,
			   const char *requested_name);


static inline struct wiphy *wiphy_new(const struct cfg80211_ops *ops,
				      int sizeof_priv)
{
	return wiphy_new_nm(ops, sizeof_priv, NULL);
}


int wiphy_register(struct wiphy *wiphy);


#define lockdep_assert_wiphy(wiphy) lockdep_assert_held(&(wiphy)->mtx)


#define rcu_dereference_wiphy(wiphy, p)				\
        rcu_dereference_check(p, lockdep_is_held(&wiphy->mtx))


#define wiphy_dereference(wiphy, p)				\
        rcu_dereference_protected(p, lockdep_is_held(&wiphy->mtx))


const struct ieee80211_regdomain *get_wiphy_regdom(struct wiphy *wiphy);


void wiphy_unregister(struct wiphy *wiphy);


void wiphy_free(struct wiphy *wiphy);


struct cfg80211_conn;
struct cfg80211_internal_bss;
struct cfg80211_cached_keys;
struct cfg80211_cqm_config;


static inline void wiphy_lock(struct wiphy *wiphy)
	__acquires(&wiphy->mtx)
{
	mutex_lock(&wiphy->mtx);
	__acquire(&wiphy->mtx);
}


static inline void wiphy_unlock(struct wiphy *wiphy)
	__releases(&wiphy->mtx)
{
	__release(&wiphy->mtx);
	mutex_unlock(&wiphy->mtx);
}

struct wiphy_work;
typedef void (*wiphy_work_func_t)(struct wiphy *, struct wiphy_work *);

struct wiphy_work {
	struct list_head entry;
	wiphy_work_func_t func;
};

static inline void wiphy_work_init(struct wiphy_work *work,
				   wiphy_work_func_t func)
{
	INIT_LIST_HEAD(&work->entry);
	work->func = func;
}


void wiphy_work_queue(struct wiphy *wiphy, struct wiphy_work *work);


void wiphy_work_cancel(struct wiphy *wiphy, struct wiphy_work *work);


void wiphy_work_flush(struct wiphy *wiphy, struct wiphy_work *work);

struct wiphy_delayed_work {
	struct wiphy_work work;
	struct wiphy *wiphy;
	struct timer_list timer;
};

void wiphy_delayed_work_timer(struct timer_list *t);

static inline void wiphy_delayed_work_init(struct wiphy_delayed_work *dwork,
					   wiphy_work_func_t func)
{
	timer_setup(&dwork->timer, wiphy_delayed_work_timer, 0);
	wiphy_work_init(&dwork->work, func);
}


void wiphy_delayed_work_queue(struct wiphy *wiphy,
			      struct wiphy_delayed_work *dwork,
			      unsigned long delay);


void wiphy_delayed_work_cancel(struct wiphy *wiphy,
			       struct wiphy_delayed_work *dwork);


void wiphy_delayed_work_flush(struct wiphy *wiphy,
			      struct wiphy_delayed_work *dwork);


struct wireless_dev {
	struct wiphy *wiphy;
	enum nl80211_iftype iftype;

	
	struct list_head list;
	struct net_device *netdev;

	u32 identifier;

	struct list_head mgmt_registrations;
	u8 mgmt_registrations_need_update:1;

	struct mutex mtx;

	bool use_4addr, is_running, registered, registering;

	u8 address[ETH_ALEN] __aligned(sizeof(u16));

	
	struct cfg80211_conn *conn;
	struct cfg80211_cached_keys *connect_keys;
	enum ieee80211_bss_type conn_bss_type;
	u32 conn_owner_nlportid;

	struct work_struct disconnect_wk;
	u8 disconnect_bssid[ETH_ALEN];

	struct list_head event_list;
	spinlock_t event_lock;

	u8 connected:1;

	bool ps;
	int ps_timeout;

	u32 ap_unexpected_nlportid;

	u32 owner_nlportid;
	bool nl_owner_dead;

	
	bool cac_started;
	unsigned long cac_start_time;
	unsigned int cac_time_ms;

#ifdef CONFIG_CFG80211_WEXT
	
	struct {
		struct cfg80211_ibss_params ibss;
		struct cfg80211_connect_params connect;
		struct cfg80211_cached_keys *keys;
		const u8 *ie;
		size_t ie_len;
		u8 bssid[ETH_ALEN];
		u8 prev_bssid[ETH_ALEN];
		u8 ssid[IEEE80211_MAX_SSID_LEN];
		s8 default_key, default_mgmt_key;
		bool prev_bssid_valid;
	} wext;
#endif

	struct wiphy_work cqm_rssi_work;
	struct cfg80211_cqm_config __rcu *cqm_config;

	struct list_head pmsr_list;
	spinlock_t pmsr_lock;
	struct work_struct pmsr_free_wk;

	unsigned long unprot_beacon_reported;

	union {
		struct {
			u8 connected_addr[ETH_ALEN] __aligned(2);
			u8 ssid[IEEE80211_MAX_SSID_LEN];
			u8 ssid_len;
		} client;
		struct {
			int beacon_interval;
			struct cfg80211_chan_def preset_chandef;
			struct cfg80211_chan_def chandef;
			u8 id[IEEE80211_MAX_SSID_LEN];
			u8 id_len, id_up_len;
		} mesh;
		struct {
			struct cfg80211_chan_def preset_chandef;
			u8 ssid[IEEE80211_MAX_SSID_LEN];
			u8 ssid_len;
		} ap;
		struct {
			struct cfg80211_internal_bss *current_bss;
			struct cfg80211_chan_def chandef;
			int beacon_interval;
			u8 ssid[IEEE80211_MAX_SSID_LEN];
			u8 ssid_len;
		} ibss;
		struct {
			struct cfg80211_chan_def chandef;
		} ocb;
	} u;

	struct {
		u8 addr[ETH_ALEN] __aligned(2);
		union {
			struct {
				unsigned int beacon_interval;
				struct cfg80211_chan_def chandef;
			} ap;
			struct {
				struct cfg80211_internal_bss *current_bss;
			} client;
		};
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
	u16 valid_links;
};

static inline const u8 *wdev_address(struct wireless_dev *wdev)
{
	if (wdev->netdev)
		return wdev->netdev->dev_addr;
	return wdev->address;
}

static inline bool wdev_running(struct wireless_dev *wdev)
{
	if (wdev->netdev)
		return netif_running(wdev->netdev);
	return wdev->is_running;
}


static inline void *wdev_priv(struct wireless_dev *wdev)
{
	BUG_ON(!wdev);
	return wiphy_priv(wdev->wiphy);
}


struct cfg80211_chan_def *wdev_chandef(struct wireless_dev *wdev,
				       unsigned int link_id);

static inline void WARN_INVALID_LINK_ID(struct wireless_dev *wdev,
					unsigned int link_id)
{
	WARN_ON(link_id && !wdev->valid_links);
	WARN_ON(wdev->valid_links &&
		!(wdev->valid_links & BIT(link_id)));
}

#define for_each_valid_link(link_info, link_id)			\
	for (link_id = 0;					\
	     link_id < ((link_info)->valid_links ?		\
			ARRAY_SIZE((link_info)->links) : 1);	\
	     link_id++)						\
		if (!(link_info)->valid_links ||		\
		    ((link_info)->valid_links & BIT(link_id)))




static inline bool
ieee80211_channel_equal(struct ieee80211_channel *a,
			struct ieee80211_channel *b)
{
	return (a->center_freq == b->center_freq &&
		a->freq_offset == b->freq_offset);
}


static inline u32
ieee80211_channel_to_khz(const struct ieee80211_channel *chan)
{
	return MHZ_TO_KHZ(chan->center_freq) + chan->freq_offset;
}


enum nl80211_chan_width
ieee80211_s1g_channel_width(const struct ieee80211_channel *chan);


u32 ieee80211_channel_to_freq_khz(int chan, enum nl80211_band band);


static inline int
ieee80211_channel_to_frequency(int chan, enum nl80211_band band)
{
	return KHZ_TO_MHZ(ieee80211_channel_to_freq_khz(chan, band));
}


int ieee80211_freq_khz_to_channel(u32 freq);


static inline int
ieee80211_frequency_to_channel(int freq)
{
	return ieee80211_freq_khz_to_channel(MHZ_TO_KHZ(freq));
}


struct ieee80211_channel *
ieee80211_get_channel_khz(struct wiphy *wiphy, u32 freq);


static inline struct ieee80211_channel *
ieee80211_get_channel(struct wiphy *wiphy, int freq)
{
	return ieee80211_get_channel_khz(wiphy, MHZ_TO_KHZ(freq));
}


static inline bool cfg80211_channel_is_psc(struct ieee80211_channel *chan)
{
	if (chan->band != NL80211_BAND_6GHZ)
		return false;

	return ieee80211_frequency_to_channel(chan->center_freq) % 16 == 5;
}


const struct ieee80211_rate *
ieee80211_get_response_rate(struct ieee80211_supported_band *sband,
			    u32 basic_rates, int bitrate);


u32 ieee80211_mandatory_rates(struct ieee80211_supported_band *sband,
			      enum nl80211_bss_scan_width scan_width);



struct radiotap_align_size {
	uint8_t align:4, size:4;
};

struct ieee80211_radiotap_namespace {
	const struct radiotap_align_size *align_size;
	int n_bits;
	uint32_t oui;
	uint8_t subns;
};

struct ieee80211_radiotap_vendor_namespaces {
	const struct ieee80211_radiotap_namespace *ns;
	int n_ns;
};



struct ieee80211_radiotap_iterator {
	struct ieee80211_radiotap_header *_rtheader;
	const struct ieee80211_radiotap_vendor_namespaces *_vns;
	const struct ieee80211_radiotap_namespace *current_namespace;

	unsigned char *_arg, *_next_ns_data;
	__le32 *_next_bitmap;

	unsigned char *this_arg;
	int this_arg_index;
	int this_arg_size;

	int is_radiotap_ns;

	int _max_length;
	int _arg_index;
	uint32_t _bitmap_shifter;
	int _reset_on_ext;
};

int
ieee80211_radiotap_iterator_init(struct ieee80211_radiotap_iterator *iterator,
				 struct ieee80211_radiotap_header *radiotap_header,
				 int max_length,
				 const struct ieee80211_radiotap_vendor_namespaces *vns);

int
ieee80211_radiotap_iterator_next(struct ieee80211_radiotap_iterator *iterator);


extern const unsigned char rfc1042_header[6];
extern const unsigned char bridge_tunnel_header[6];


unsigned int ieee80211_get_hdrlen_from_skb(const struct sk_buff *skb);


unsigned int __attribute_const__ ieee80211_hdrlen(__le16 fc);


unsigned int ieee80211_get_mesh_hdrlen(struct ieee80211s_hdr *meshhdr);




int ieee80211_data_to_8023_exthdr(struct sk_buff *skb, struct ethhdr *ehdr,
				  const u8 *addr, enum nl80211_iftype iftype,
				  u8 data_offset, bool is_amsdu);


static inline int ieee80211_data_to_8023(struct sk_buff *skb, const u8 *addr,
					 enum nl80211_iftype iftype)
{
	return ieee80211_data_to_8023_exthdr(skb, NULL, addr, iftype, 0, false);
}


bool ieee80211_is_valid_amsdu(struct sk_buff *skb, u8 mesh_hdr);


void ieee80211_amsdu_to_8023s(struct sk_buff *skb, struct sk_buff_head *list,
			      const u8 *addr, enum nl80211_iftype iftype,
			      const unsigned int extra_headroom,
			      const u8 *check_da, const u8 *check_sa,
			      u8 mesh_control);


bool ieee80211_get_8023_tunnel_proto(const void *hdr, __be16 *proto);


int ieee80211_strip_8023_mesh_hdr(struct sk_buff *skb);


unsigned int cfg80211_classify8021d(struct sk_buff *skb,
				    struct cfg80211_qos_map *qos_map);


const struct element *
cfg80211_find_elem_match(u8 eid, const u8 *ies, unsigned int len,
			 const u8 *match, unsigned int match_len,
			 unsigned int match_offset);


static inline const u8 *
cfg80211_find_ie_match(u8 eid, const u8 *ies, unsigned int len,
		       const u8 *match, unsigned int match_len,
		       unsigned int match_offset)
{
	
	if (WARN_ON((match_len && match_offset < 2) ||
		    (!match_len && match_offset)))
		return NULL;

	return (const void *)cfg80211_find_elem_match(eid, ies, len,
						      match, match_len,
						      match_offset ?
							match_offset - 2 : 0);
}


static inline const struct element *
cfg80211_find_elem(u8 eid, const u8 *ies, int len)
{
	return cfg80211_find_elem_match(eid, ies, len, NULL, 0, 0);
}


static inline const u8 *cfg80211_find_ie(u8 eid, const u8 *ies, int len)
{
	return cfg80211_find_ie_match(eid, ies, len, NULL, 0, 0);
}


static inline const struct element *
cfg80211_find_ext_elem(u8 ext_eid, const u8 *ies, int len)
{
	return cfg80211_find_elem_match(WLAN_EID_EXTENSION, ies, len,
					&ext_eid, 1, 0);
}


static inline const u8 *cfg80211_find_ext_ie(u8 ext_eid, const u8 *ies, int len)
{
	return cfg80211_find_ie_match(WLAN_EID_EXTENSION, ies, len,
				      &ext_eid, 1, 2);
}


const struct element *cfg80211_find_vendor_elem(unsigned int oui, int oui_type,
						const u8 *ies,
						unsigned int len);


static inline const u8 *
cfg80211_find_vendor_ie(unsigned int oui, int oui_type,
			const u8 *ies, unsigned int len)
{
	return (const void *)cfg80211_find_vendor_elem(oui, oui_type, ies, len);
}


ssize_t cfg80211_defragment_element(const struct element *elem, const u8 *ies,
				    size_t ieslen, u8 *data, size_t data_len,
				    u8 frag_id);


void cfg80211_send_layer2_update(struct net_device *dev, const u8 *addr);




int regulatory_hint(struct wiphy *wiphy, const char *alpha2);


int regulatory_set_wiphy_regd(struct wiphy *wiphy,
			      struct ieee80211_regdomain *rd);


int regulatory_set_wiphy_regd_sync(struct wiphy *wiphy,
				   struct ieee80211_regdomain *rd);


void wiphy_apply_custom_regulatory(struct wiphy *wiphy,
				   const struct ieee80211_regdomain *regd);


const struct ieee80211_reg_rule *freq_reg_info(struct wiphy *wiphy,
					       u32 center_freq);


const char *reg_initiator_name(enum nl80211_reg_initiator initiator);


bool regulatory_pre_cac_allowed(struct wiphy *wiphy);




int reg_query_regdb_wmm(char *alpha2, int freq,
			struct ieee80211_reg_rule *rule);




void cfg80211_scan_done(struct cfg80211_scan_request *request,
			struct cfg80211_scan_info *info);


void cfg80211_sched_scan_results(struct wiphy *wiphy, u64 reqid);


void cfg80211_sched_scan_stopped(struct wiphy *wiphy, u64 reqid);


void cfg80211_sched_scan_stopped_locked(struct wiphy *wiphy, u64 reqid);


struct cfg80211_bss * __must_check
cfg80211_inform_bss_frame_data(struct wiphy *wiphy,
			       struct cfg80211_inform_bss *data,
			       struct ieee80211_mgmt *mgmt, size_t len,
			       gfp_t gfp);

static inline struct cfg80211_bss * __must_check
cfg80211_inform_bss_width_frame(struct wiphy *wiphy,
				struct ieee80211_channel *rx_channel,
				enum nl80211_bss_scan_width scan_width,
				struct ieee80211_mgmt *mgmt, size_t len,
				s32 signal, gfp_t gfp)
{
	struct cfg80211_inform_bss data = {
		.chan = rx_channel,
		.scan_width = scan_width,
		.signal = signal,
	};

	return cfg80211_inform_bss_frame_data(wiphy, &data, mgmt, len, gfp);
}

static inline struct cfg80211_bss * __must_check
cfg80211_inform_bss_frame(struct wiphy *wiphy,
			  struct ieee80211_channel *rx_channel,
			  struct ieee80211_mgmt *mgmt, size_t len,
			  s32 signal, gfp_t gfp)
{
	struct cfg80211_inform_bss data = {
		.chan = rx_channel,
		.scan_width = NL80211_BSS_CHAN_WIDTH_20,
		.signal = signal,
	};

	return cfg80211_inform_bss_frame_data(wiphy, &data, mgmt, len, gfp);
}


static inline void cfg80211_gen_new_bssid(const u8 *bssid, u8 max_bssid,
					  u8 mbssid_index, u8 *new_bssid)
{
	u64 bssid_u64 = ether_addr_to_u64(bssid);
	u64 mask = GENMASK_ULL(max_bssid - 1, 0);
	u64 new_bssid_u64;

	new_bssid_u64 = bssid_u64 & ~mask;

	new_bssid_u64 |= ((bssid_u64 & mask) + mbssid_index) & mask;

	u64_to_ether_addr(new_bssid_u64, new_bssid);
}


bool cfg80211_is_element_inherited(const struct element *element,
				   const struct element *non_inherit_element);


size_t cfg80211_merge_profile(const u8 *ie, size_t ielen,
			      const struct element *mbssid_elem,
			      const struct element *sub_elem,
			      u8 *merged_ie, size_t max_copy_len);


enum cfg80211_bss_frame_type {
	CFG80211_BSS_FTYPE_UNKNOWN,
	CFG80211_BSS_FTYPE_BEACON,
	CFG80211_BSS_FTYPE_PRESP,
};


int cfg80211_get_ies_channel_number(const u8 *ie, size_t ielen,
				    enum nl80211_band band);


struct cfg80211_bss * __must_check
cfg80211_inform_bss_data(struct wiphy *wiphy,
			 struct cfg80211_inform_bss *data,
			 enum cfg80211_bss_frame_type ftype,
			 const u8 *bssid, u64 tsf, u16 capability,
			 u16 beacon_interval, const u8 *ie, size_t ielen,
			 gfp_t gfp);

static inline struct cfg80211_bss * __must_check
cfg80211_inform_bss_width(struct wiphy *wiphy,
			  struct ieee80211_channel *rx_channel,
			  enum nl80211_bss_scan_width scan_width,
			  enum cfg80211_bss_frame_type ftype,
			  const u8 *bssid, u64 tsf, u16 capability,
			  u16 beacon_interval, const u8 *ie, size_t ielen,
			  s32 signal, gfp_t gfp)
{
	struct cfg80211_inform_bss data = {
		.chan = rx_channel,
		.scan_width = scan_width,
		.signal = signal,
	};

	return cfg80211_inform_bss_data(wiphy, &data, ftype, bssid, tsf,
					capability, beacon_interval, ie, ielen,
					gfp);
}

static inline struct cfg80211_bss * __must_check
cfg80211_inform_bss(struct wiphy *wiphy,
		    struct ieee80211_channel *rx_channel,
		    enum cfg80211_bss_frame_type ftype,
		    const u8 *bssid, u64 tsf, u16 capability,
		    u16 beacon_interval, const u8 *ie, size_t ielen,
		    s32 signal, gfp_t gfp)
{
	struct cfg80211_inform_bss data = {
		.chan = rx_channel,
		.scan_width = NL80211_BSS_CHAN_WIDTH_20,
		.signal = signal,
	};

	return cfg80211_inform_bss_data(wiphy, &data, ftype, bssid, tsf,
					capability, beacon_interval, ie, ielen,
					gfp);
}


struct cfg80211_bss *cfg80211_get_bss(struct wiphy *wiphy,
				      struct ieee80211_channel *channel,
				      const u8 *bssid,
				      const u8 *ssid, size_t ssid_len,
				      enum ieee80211_bss_type bss_type,
				      enum ieee80211_privacy privacy);
static inline struct cfg80211_bss *
cfg80211_get_ibss(struct wiphy *wiphy,
		  struct ieee80211_channel *channel,
		  const u8 *ssid, size_t ssid_len)
{
	return cfg80211_get_bss(wiphy, channel, NULL, ssid, ssid_len,
				IEEE80211_BSS_TYPE_IBSS,
				IEEE80211_PRIVACY_ANY);
}


void cfg80211_ref_bss(struct wiphy *wiphy, struct cfg80211_bss *bss);


void cfg80211_put_bss(struct wiphy *wiphy, struct cfg80211_bss *bss);


void cfg80211_unlink_bss(struct wiphy *wiphy, struct cfg80211_bss *bss);


void cfg80211_bss_iter(struct wiphy *wiphy,
		       struct cfg80211_chan_def *chandef,
		       void (*iter)(struct wiphy *wiphy,
				    struct cfg80211_bss *bss,
				    void *data),
		       void *iter_data);

static inline enum nl80211_bss_scan_width
cfg80211_chandef_to_scan_width(const struct cfg80211_chan_def *chandef)
{
	switch (chandef->width) {
	case NL80211_CHAN_WIDTH_5:
		return NL80211_BSS_CHAN_WIDTH_5;
	case NL80211_CHAN_WIDTH_10:
		return NL80211_BSS_CHAN_WIDTH_10;
	default:
		return NL80211_BSS_CHAN_WIDTH_20;
	}
}


void cfg80211_rx_mlme_mgmt(struct net_device *dev, const u8 *buf, size_t len);


void cfg80211_auth_timeout(struct net_device *dev, const u8 *addr);


struct cfg80211_rx_assoc_resp {
	const u8 *buf;
	size_t len;
	const u8 *req_ies;
	size_t req_ies_len;
	int uapsd_queues;
	const u8 *ap_mld_addr;
	struct {
		u8 addr[ETH_ALEN] __aligned(2);
		struct cfg80211_bss *bss;
		u16 status;
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
};


void cfg80211_rx_assoc_resp(struct net_device *dev,
			    struct cfg80211_rx_assoc_resp *data);


struct cfg80211_assoc_failure {
	const u8 *ap_mld_addr;
	struct cfg80211_bss *bss[IEEE80211_MLD_MAX_NUM_LINKS];
	bool timeout;
};


void cfg80211_assoc_failure(struct net_device *dev,
			    struct cfg80211_assoc_failure *data);


void cfg80211_tx_mlme_mgmt(struct net_device *dev, const u8 *buf, size_t len,
			   bool reconnect);


void cfg80211_rx_unprot_mlme_mgmt(struct net_device *dev,
				  const u8 *buf, size_t len);


void cfg80211_michael_mic_failure(struct net_device *dev, const u8 *addr,
				  enum nl80211_key_type key_type, int key_id,
				  const u8 *tsc, gfp_t gfp);


void cfg80211_ibss_joined(struct net_device *dev, const u8 *bssid,
			  struct ieee80211_channel *channel, gfp_t gfp);


void cfg80211_notify_new_peer_candidate(struct net_device *dev,
		const u8 *macaddr, const u8 *ie, u8 ie_len,
		int sig_dbm, gfp_t gfp);




void wiphy_rfkill_set_hw_state_reason(struct wiphy *wiphy, bool blocked,
				      enum rfkill_hard_block_reasons reason);

static inline void wiphy_rfkill_set_hw_state(struct wiphy *wiphy, bool blocked)
{
	wiphy_rfkill_set_hw_state_reason(wiphy, blocked,
					 RFKILL_HARD_BLOCK_SIGNAL);
}


void wiphy_rfkill_start_polling(struct wiphy *wiphy);


static inline void wiphy_rfkill_stop_polling(struct wiphy *wiphy)
{
	rfkill_pause_polling(wiphy->rfkill);
}



struct sk_buff *__cfg80211_alloc_reply_skb(struct wiphy *wiphy,
					   enum nl80211_commands cmd,
					   enum nl80211_attrs attr,
					   int approxlen);

struct sk_buff *__cfg80211_alloc_event_skb(struct wiphy *wiphy,
					   struct wireless_dev *wdev,
					   enum nl80211_commands cmd,
					   enum nl80211_attrs attr,
					   unsigned int portid,
					   int vendor_event_idx,
					   int approxlen, gfp_t gfp);

void __cfg80211_send_event_skb(struct sk_buff *skb, gfp_t gfp);


static inline struct sk_buff *
cfg80211_vendor_cmd_alloc_reply_skb(struct wiphy *wiphy, int approxlen)
{
	return __cfg80211_alloc_reply_skb(wiphy, NL80211_CMD_VENDOR,
					  NL80211_ATTR_VENDOR_DATA, approxlen);
}


int cfg80211_vendor_cmd_reply(struct sk_buff *skb);


unsigned int cfg80211_vendor_cmd_get_sender(struct wiphy *wiphy);


static inline struct sk_buff *
cfg80211_vendor_event_alloc(struct wiphy *wiphy, struct wireless_dev *wdev,
			     int approxlen, int event_idx, gfp_t gfp)
{
	return __cfg80211_alloc_event_skb(wiphy, wdev, NL80211_CMD_VENDOR,
					  NL80211_ATTR_VENDOR_DATA,
					  0, event_idx, approxlen, gfp);
}


static inline struct sk_buff *
cfg80211_vendor_event_alloc_ucast(struct wiphy *wiphy,
				  struct wireless_dev *wdev,
				  unsigned int portid, int approxlen,
				  int event_idx, gfp_t gfp)
{
	return __cfg80211_alloc_event_skb(wiphy, wdev, NL80211_CMD_VENDOR,
					  NL80211_ATTR_VENDOR_DATA,
					  portid, event_idx, approxlen, gfp);
}


static inline void cfg80211_vendor_event(struct sk_buff *skb, gfp_t gfp)
{
	__cfg80211_send_event_skb(skb, gfp);
}

#ifdef CONFIG_NL80211_TESTMODE



static inline struct sk_buff *
cfg80211_testmode_alloc_reply_skb(struct wiphy *wiphy, int approxlen)
{
	return __cfg80211_alloc_reply_skb(wiphy, NL80211_CMD_TESTMODE,
					  NL80211_ATTR_TESTDATA, approxlen);
}


static inline int cfg80211_testmode_reply(struct sk_buff *skb)
{
	return cfg80211_vendor_cmd_reply(skb);
}


static inline struct sk_buff *
cfg80211_testmode_alloc_event_skb(struct wiphy *wiphy, int approxlen, gfp_t gfp)
{
	return __cfg80211_alloc_event_skb(wiphy, NULL, NL80211_CMD_TESTMODE,
					  NL80211_ATTR_TESTDATA, 0, -1,
					  approxlen, gfp);
}


static inline void cfg80211_testmode_event(struct sk_buff *skb, gfp_t gfp)
{
	__cfg80211_send_event_skb(skb, gfp);
}

#define CFG80211_TESTMODE_CMD(cmd)	.testmode_cmd = (cmd),
#define CFG80211_TESTMODE_DUMP(cmd)	.testmode_dump = (cmd),
#else
#define CFG80211_TESTMODE_CMD(cmd)
#define CFG80211_TESTMODE_DUMP(cmd)
#endif


struct cfg80211_fils_resp_params {
	const u8 *kek;
	size_t kek_len;
	bool update_erp_next_seq_num;
	u16 erp_next_seq_num;
	const u8 *pmk;
	size_t pmk_len;
	const u8 *pmkid;
};


struct cfg80211_connect_resp_params {
	int status;
	const u8 *req_ie;
	size_t req_ie_len;
	const u8 *resp_ie;
	size_t resp_ie_len;
	struct cfg80211_fils_resp_params fils;
	enum nl80211_timeout_reason timeout_reason;

	const u8 *ap_mld_addr;
	u16 valid_links;
	struct {
		const u8 *addr;
		const u8 *bssid;
		struct cfg80211_bss *bss;
		u16 status;
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
};


void cfg80211_connect_done(struct net_device *dev,
			   struct cfg80211_connect_resp_params *params,
			   gfp_t gfp);


static inline void
cfg80211_connect_bss(struct net_device *dev, const u8 *bssid,
		     struct cfg80211_bss *bss, const u8 *req_ie,
		     size_t req_ie_len, const u8 *resp_ie,
		     size_t resp_ie_len, int status, gfp_t gfp,
		     enum nl80211_timeout_reason timeout_reason)
{
	struct cfg80211_connect_resp_params params;

	memset(&params, 0, sizeof(params));
	params.status = status;
	params.links[0].bssid = bssid;
	params.links[0].bss = bss;
	params.req_ie = req_ie;
	params.req_ie_len = req_ie_len;
	params.resp_ie = resp_ie;
	params.resp_ie_len = resp_ie_len;
	params.timeout_reason = timeout_reason;

	cfg80211_connect_done(dev, &params, gfp);
}


static inline void
cfg80211_connect_result(struct net_device *dev, const u8 *bssid,
			const u8 *req_ie, size_t req_ie_len,
			const u8 *resp_ie, size_t resp_ie_len,
			u16 status, gfp_t gfp)
{
	cfg80211_connect_bss(dev, bssid, NULL, req_ie, req_ie_len, resp_ie,
			     resp_ie_len, status, gfp,
			     NL80211_TIMEOUT_UNSPECIFIED);
}


static inline void
cfg80211_connect_timeout(struct net_device *dev, const u8 *bssid,
			 const u8 *req_ie, size_t req_ie_len, gfp_t gfp,
			 enum nl80211_timeout_reason timeout_reason)
{
	cfg80211_connect_bss(dev, bssid, NULL, req_ie, req_ie_len, NULL, 0, -1,
			     gfp, timeout_reason);
}


struct cfg80211_roam_info {
	const u8 *req_ie;
	size_t req_ie_len;
	const u8 *resp_ie;
	size_t resp_ie_len;
	struct cfg80211_fils_resp_params fils;

	const u8 *ap_mld_addr;
	u16 valid_links;
	struct {
		const u8 *addr;
		const u8 *bssid;
		struct ieee80211_channel *channel;
		struct cfg80211_bss *bss;
	} links[IEEE80211_MLD_MAX_NUM_LINKS];
};


void cfg80211_roamed(struct net_device *dev, struct cfg80211_roam_info *info,
		     gfp_t gfp);


void cfg80211_port_authorized(struct net_device *dev, const u8 *bssid,
			      const u8* td_bitmap, u8 td_bitmap_len, gfp_t gfp);


void cfg80211_disconnected(struct net_device *dev, u16 reason,
			   const u8 *ie, size_t ie_len,
			   bool locally_generated, gfp_t gfp);


void cfg80211_ready_on_channel(struct wireless_dev *wdev, u64 cookie,
			       struct ieee80211_channel *chan,
			       unsigned int duration, gfp_t gfp);


void cfg80211_remain_on_channel_expired(struct wireless_dev *wdev, u64 cookie,
					struct ieee80211_channel *chan,
					gfp_t gfp);


void cfg80211_tx_mgmt_expired(struct wireless_dev *wdev, u64 cookie,
			      struct ieee80211_channel *chan, gfp_t gfp);


int cfg80211_sinfo_alloc_tid_stats(struct station_info *sinfo, gfp_t gfp);


static inline void cfg80211_sinfo_release_content(struct station_info *sinfo)
{
	kfree(sinfo->pertid);
}


void cfg80211_new_sta(struct net_device *dev, const u8 *mac_addr,
		      struct station_info *sinfo, gfp_t gfp);


void cfg80211_del_sta_sinfo(struct net_device *dev, const u8 *mac_addr,
			    struct station_info *sinfo, gfp_t gfp);


static inline void cfg80211_del_sta(struct net_device *dev,
				    const u8 *mac_addr, gfp_t gfp)
{
	cfg80211_del_sta_sinfo(dev, mac_addr, NULL, gfp);
}


void cfg80211_conn_failed(struct net_device *dev, const u8 *mac_addr,
			  enum nl80211_connect_failed_reason reason,
			  gfp_t gfp);


struct cfg80211_rx_info {
	int freq;
	int sig_dbm;
	bool have_link_id;
	u8 link_id;
	const u8 *buf;
	size_t len;
	u32 flags;
	u64 rx_tstamp;
	u64 ack_tstamp;
};


bool cfg80211_rx_mgmt_ext(struct wireless_dev *wdev,
			  struct cfg80211_rx_info *info);


static inline bool cfg80211_rx_mgmt_khz(struct wireless_dev *wdev, int freq,
					int sig_dbm, const u8 *buf, size_t len,
					u32 flags)
{
	struct cfg80211_rx_info info = {
		.freq = freq,
		.sig_dbm = sig_dbm,
		.buf = buf,
		.len = len,
		.flags = flags
	};

	return cfg80211_rx_mgmt_ext(wdev, &info);
}


static inline bool cfg80211_rx_mgmt(struct wireless_dev *wdev, int freq,
				    int sig_dbm, const u8 *buf, size_t len,
				    u32 flags)
{
	struct cfg80211_rx_info info = {
		.freq = MHZ_TO_KHZ(freq),
		.sig_dbm = sig_dbm,
		.buf = buf,
		.len = len,
		.flags = flags
	};

	return cfg80211_rx_mgmt_ext(wdev, &info);
}


struct cfg80211_tx_status {
	u64 cookie;
	u64 tx_tstamp;
	u64 ack_tstamp;
	const u8 *buf;
	size_t len;
	bool ack;
};


void cfg80211_mgmt_tx_status_ext(struct wireless_dev *wdev,
				 struct cfg80211_tx_status *status, gfp_t gfp);


static inline void cfg80211_mgmt_tx_status(struct wireless_dev *wdev,
					   u64 cookie, const u8 *buf,
					   size_t len, bool ack, gfp_t gfp)
{
	struct cfg80211_tx_status status = {
		.cookie = cookie,
		.buf = buf,
		.len = len,
		.ack = ack
	};

	cfg80211_mgmt_tx_status_ext(wdev, &status, gfp);
}


void cfg80211_control_port_tx_status(struct wireless_dev *wdev, u64 cookie,
				     const u8 *buf, size_t len, bool ack,
				     gfp_t gfp);


bool cfg80211_rx_control_port(struct net_device *dev, struct sk_buff *skb,
			      bool unencrypted, int link_id);


void cfg80211_cqm_rssi_notify(struct net_device *dev,
			      enum nl80211_cqm_rssi_threshold_event rssi_event,
			      s32 rssi_level, gfp_t gfp);


void cfg80211_cqm_pktloss_notify(struct net_device *dev,
				 const u8 *peer, u32 num_packets, gfp_t gfp);


void cfg80211_cqm_txe_notify(struct net_device *dev, const u8 *peer,
			     u32 num_packets, u32 rate, u32 intvl, gfp_t gfp);


void cfg80211_cqm_beacon_loss_notify(struct net_device *dev, gfp_t gfp);


void __cfg80211_radar_event(struct wiphy *wiphy,
			    struct cfg80211_chan_def *chandef,
			    bool offchan, gfp_t gfp);

static inline void
cfg80211_radar_event(struct wiphy *wiphy,
		     struct cfg80211_chan_def *chandef,
		     gfp_t gfp)
{
	__cfg80211_radar_event(wiphy, chandef, false, gfp);
}

static inline void
cfg80211_background_radar_event(struct wiphy *wiphy,
				struct cfg80211_chan_def *chandef,
				gfp_t gfp)
{
	__cfg80211_radar_event(wiphy, chandef, true, gfp);
}


void cfg80211_sta_opmode_change_notify(struct net_device *dev, const u8 *mac,
				       struct sta_opmode_info *sta_opmode,
				       gfp_t gfp);


void cfg80211_cac_event(struct net_device *netdev,
			const struct cfg80211_chan_def *chandef,
			enum nl80211_radar_event event, gfp_t gfp);


void cfg80211_background_cac_abort(struct wiphy *wiphy);


void cfg80211_gtk_rekey_notify(struct net_device *dev, const u8 *bssid,
			       const u8 *replay_ctr, gfp_t gfp);


void cfg80211_pmksa_candidate_notify(struct net_device *dev, int index,
				     const u8 *bssid, bool preauth, gfp_t gfp);


bool cfg80211_rx_spurious_frame(struct net_device *dev,
				const u8 *addr, gfp_t gfp);


bool cfg80211_rx_unexpected_4addr_frame(struct net_device *dev,
					const u8 *addr, gfp_t gfp);


void cfg80211_probe_status(struct net_device *dev, const u8 *addr,
			   u64 cookie, bool acked, s32 ack_signal,
			   bool is_valid_ack_signal, gfp_t gfp);


void cfg80211_report_obss_beacon_khz(struct wiphy *wiphy, const u8 *frame,
				     size_t len, int freq, int sig_dbm);


static inline void cfg80211_report_obss_beacon(struct wiphy *wiphy,
					       const u8 *frame, size_t len,
					       int freq, int sig_dbm)
{
	cfg80211_report_obss_beacon_khz(wiphy, frame, len, MHZ_TO_KHZ(freq),
					sig_dbm);
}


bool cfg80211_reg_can_beacon(struct wiphy *wiphy,
			     struct cfg80211_chan_def *chandef,
			     enum nl80211_iftype iftype);


bool cfg80211_reg_can_beacon_relax(struct wiphy *wiphy,
				   struct cfg80211_chan_def *chandef,
				   enum nl80211_iftype iftype);


void cfg80211_ch_switch_notify(struct net_device *dev,
			       struct cfg80211_chan_def *chandef,
			       unsigned int link_id, u16 punct_bitmap);


void cfg80211_ch_switch_started_notify(struct net_device *dev,
				       struct cfg80211_chan_def *chandef,
				       unsigned int link_id, u8 count,
				       bool quiet, u16 punct_bitmap);


bool ieee80211_operating_class_to_band(u8 operating_class,
				       enum nl80211_band *band);


bool ieee80211_chandef_to_operating_class(struct cfg80211_chan_def *chandef,
					  u8 *op_class);


static inline u32
ieee80211_chandef_to_khz(const struct cfg80211_chan_def *chandef)
{
	return MHZ_TO_KHZ(chandef->center_freq1) + chandef->freq1_offset;
}


void cfg80211_tdls_oper_request(struct net_device *dev, const u8 *peer,
				enum nl80211_tdls_operation oper,
				u16 reason_code, gfp_t gfp);


u32 cfg80211_calculate_bitrate(struct rate_info *rate);


void cfg80211_unregister_wdev(struct wireless_dev *wdev);


int cfg80211_register_netdevice(struct net_device *dev);


static inline void cfg80211_unregister_netdevice(struct net_device *dev)
{
#if IS_ENABLED(CONFIG_CFG80211)
	cfg80211_unregister_wdev(dev->ieee80211_ptr);
#endif
}


struct cfg80211_ft_event_params {
	const u8 *ies;
	size_t ies_len;
	const u8 *target_ap;
	const u8 *ric_ies;
	size_t ric_ies_len;
};


void cfg80211_ft_event(struct net_device *netdev,
		       struct cfg80211_ft_event_params *ft_event);


int cfg80211_get_p2p_attr(const u8 *ies, unsigned int len,
			  enum ieee80211_p2p_attr_id attr,
			  u8 *buf, unsigned int bufsize);


size_t ieee80211_ie_split_ric(const u8 *ies, size_t ielen,
			      const u8 *ids, int n_ids,
			      const u8 *after_ric, int n_after_ric,
			      size_t offset);


static inline size_t ieee80211_ie_split(const u8 *ies, size_t ielen,
					const u8 *ids, int n_ids, size_t offset)
{
	return ieee80211_ie_split_ric(ies, ielen, ids, n_ids, NULL, 0, offset);
}


void cfg80211_report_wowlan_wakeup(struct wireless_dev *wdev,
				   struct cfg80211_wowlan_wakeup *wakeup,
				   gfp_t gfp);


void cfg80211_crit_proto_stopped(struct wireless_dev *wdev, gfp_t gfp);


unsigned int ieee80211_get_num_supported_channels(struct wiphy *wiphy);


int cfg80211_check_combinations(struct wiphy *wiphy,
				struct iface_combination_params *params);


int cfg80211_iter_combinations(struct wiphy *wiphy,
			       struct iface_combination_params *params,
			       void (*iter)(const struct ieee80211_iface_combination *c,
					    void *data),
			       void *data);


void cfg80211_stop_iface(struct wiphy *wiphy, struct wireless_dev *wdev,
			 gfp_t gfp);


void cfg80211_shutdown_all_interfaces(struct wiphy *wiphy);


static inline void wiphy_ext_feature_set(struct wiphy *wiphy,
					 enum nl80211_ext_feature_index ftidx)
{
	u8 *ft_byte;

	ft_byte = &wiphy->ext_features[ftidx / 8];
	*ft_byte |= BIT(ftidx % 8);
}


static inline bool
wiphy_ext_feature_isset(struct wiphy *wiphy,
			enum nl80211_ext_feature_index ftidx)
{
	u8 ft_byte;

	ft_byte = wiphy->ext_features[ftidx / 8];
	return (ft_byte & BIT(ftidx % 8)) != 0;
}


void cfg80211_free_nan_func(struct cfg80211_nan_func *f);


struct cfg80211_nan_match_params {
	enum nl80211_nan_function_type type;
	u8 inst_id;
	u8 peer_inst_id;
	const u8 *addr;
	u8 info_len;
	const u8 *info;
	u64 cookie;
};


void cfg80211_nan_match(struct wireless_dev *wdev,
			struct cfg80211_nan_match_params *match, gfp_t gfp);


void cfg80211_nan_func_terminated(struct wireless_dev *wdev,
				  u8 inst_id,
				  enum nl80211_nan_func_term_reason reason,
				  u64 cookie, gfp_t gfp);


void cfg80211_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info);


int cfg80211_external_auth_request(struct net_device *netdev,
				   struct cfg80211_external_auth_params *params,
				   gfp_t gfp);


void cfg80211_pmsr_report(struct wireless_dev *wdev,
			  struct cfg80211_pmsr_request *req,
			  struct cfg80211_pmsr_result *result,
			  gfp_t gfp);


void cfg80211_pmsr_complete(struct wireless_dev *wdev,
			    struct cfg80211_pmsr_request *req,
			    gfp_t gfp);


bool cfg80211_iftype_allowed(struct wiphy *wiphy, enum nl80211_iftype iftype,
			     bool is_4addr, u8 check_swif);



void cfg80211_assoc_comeback(struct net_device *netdev,
			     const u8 *ap_addr, u32 timeout);





#define wiphy_printk(level, wiphy, format, args...)		\
	dev_printk(level, &(wiphy)->dev, format, ##args)
#define wiphy_emerg(wiphy, format, args...)			\
	dev_emerg(&(wiphy)->dev, format, ##args)
#define wiphy_alert(wiphy, format, args...)			\
	dev_alert(&(wiphy)->dev, format, ##args)
#define wiphy_crit(wiphy, format, args...)			\
	dev_crit(&(wiphy)->dev, format, ##args)
#define wiphy_err(wiphy, format, args...)			\
	dev_err(&(wiphy)->dev, format, ##args)
#define wiphy_warn(wiphy, format, args...)			\
	dev_warn(&(wiphy)->dev, format, ##args)
#define wiphy_notice(wiphy, format, args...)			\
	dev_notice(&(wiphy)->dev, format, ##args)
#define wiphy_info(wiphy, format, args...)			\
	dev_info(&(wiphy)->dev, format, ##args)
#define wiphy_info_once(wiphy, format, args...)			\
	dev_info_once(&(wiphy)->dev, format, ##args)

#define wiphy_err_ratelimited(wiphy, format, args...)		\
	dev_err_ratelimited(&(wiphy)->dev, format, ##args)
#define wiphy_warn_ratelimited(wiphy, format, args...)		\
	dev_warn_ratelimited(&(wiphy)->dev, format, ##args)

#define wiphy_debug(wiphy, format, args...)			\
	wiphy_printk(KERN_DEBUG, wiphy, format, ##args)

#define wiphy_dbg(wiphy, format, args...)			\
	dev_dbg(&(wiphy)->dev, format, ##args)

#if defined(VERBOSE_DEBUG)
#define wiphy_vdbg	wiphy_dbg
#else
#define wiphy_vdbg(wiphy, format, args...)				\
({									\
	if (0)								\
		wiphy_printk(KERN_DEBUG, wiphy, format, ##args);	\
	0;								\
})
#endif


#define wiphy_WARN(wiphy, format, args...)			\
	WARN(1, "wiphy: %s\n" format, wiphy_name(wiphy), ##args);


void cfg80211_update_owe_info_event(struct net_device *netdev,
				    struct cfg80211_update_owe_info *owe_info,
				    gfp_t gfp);


void cfg80211_bss_flush(struct wiphy *wiphy);


int cfg80211_bss_color_notify(struct net_device *dev,
			      enum nl80211_commands cmd, u8 count,
			      u64 color_bitmap);


static inline int cfg80211_obss_color_collision_notify(struct net_device *dev,
						       u64 color_bitmap)
{
	return cfg80211_bss_color_notify(dev, NL80211_CMD_OBSS_COLOR_COLLISION,
					 0, color_bitmap);
}


static inline int cfg80211_color_change_started_notify(struct net_device *dev,
						       u8 count)
{
	return cfg80211_bss_color_notify(dev, NL80211_CMD_COLOR_CHANGE_STARTED,
					 count, 0);
}


static inline int cfg80211_color_change_aborted_notify(struct net_device *dev)
{
	return cfg80211_bss_color_notify(dev, NL80211_CMD_COLOR_CHANGE_ABORTED,
					 0, 0);
}


static inline int cfg80211_color_change_notify(struct net_device *dev)
{
	return cfg80211_bss_color_notify(dev,
					 NL80211_CMD_COLOR_CHANGE_COMPLETED,
					 0, 0);
}


bool cfg80211_valid_disable_subchannel_bitmap(u16 *bitmap,
					      const struct cfg80211_chan_def *chandef);


void cfg80211_links_removed(struct net_device *dev, u16 link_mask);

#endif 
