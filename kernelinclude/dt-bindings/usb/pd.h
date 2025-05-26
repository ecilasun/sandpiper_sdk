/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DT_POWER_DELIVERY_H
#define __DT_POWER_DELIVERY_H


#define PDO_TYPE_FIXED		0
#define PDO_TYPE_BATT		1
#define PDO_TYPE_VAR		2
#define PDO_TYPE_APDO		3

#define PDO_TYPE_SHIFT		30
#define PDO_TYPE_MASK		0x3

#define PDO_TYPE(t)	((t) << PDO_TYPE_SHIFT)

#define PDO_VOLT_MASK		0x3ff
#define PDO_CURR_MASK		0x3ff
#define PDO_PWR_MASK		0x3ff

#define PDO_FIXED_DUAL_ROLE	(1 << 29) 
#define PDO_FIXED_SUSPEND	(1 << 28) 
#define PDO_FIXED_HIGHER_CAP	(1 << 28) 
#define PDO_FIXED_EXTPOWER	(1 << 27) 
#define PDO_FIXED_USB_COMM	(1 << 26) 
#define PDO_FIXED_DATA_SWAP	(1 << 25) 
#define PDO_FIXED_VOLT_SHIFT	10	
#define PDO_FIXED_CURR_SHIFT	0	

#define PDO_FIXED_VOLT(mv)	((((mv) / 50) & PDO_VOLT_MASK) << PDO_FIXED_VOLT_SHIFT)
#define PDO_FIXED_CURR(ma)	((((ma) / 10) & PDO_CURR_MASK) << PDO_FIXED_CURR_SHIFT)

#define PDO_FIXED(mv, ma, flags)			\
	(PDO_TYPE(PDO_TYPE_FIXED) | (flags) |		\
	 PDO_FIXED_VOLT(mv) | PDO_FIXED_CURR(ma))

#define VSAFE5V 5000 

#define PDO_BATT_MAX_VOLT_SHIFT	20	
#define PDO_BATT_MIN_VOLT_SHIFT	10	
#define PDO_BATT_MAX_PWR_SHIFT	0	

#define PDO_BATT_MIN_VOLT(mv) ((((mv) / 50) & PDO_VOLT_MASK) << PDO_BATT_MIN_VOLT_SHIFT)
#define PDO_BATT_MAX_VOLT(mv) ((((mv) / 50) & PDO_VOLT_MASK) << PDO_BATT_MAX_VOLT_SHIFT)
#define PDO_BATT_MAX_POWER(mw) ((((mw) / 250) & PDO_PWR_MASK) << PDO_BATT_MAX_PWR_SHIFT)

#define PDO_BATT(min_mv, max_mv, max_mw)			\
	(PDO_TYPE(PDO_TYPE_BATT) | PDO_BATT_MIN_VOLT(min_mv) |	\
	 PDO_BATT_MAX_VOLT(max_mv) | PDO_BATT_MAX_POWER(max_mw))

#define PDO_VAR_MAX_VOLT_SHIFT	20	
#define PDO_VAR_MIN_VOLT_SHIFT	10	
#define PDO_VAR_MAX_CURR_SHIFT	0	

#define PDO_VAR_MIN_VOLT(mv) ((((mv) / 50) & PDO_VOLT_MASK) << PDO_VAR_MIN_VOLT_SHIFT)
#define PDO_VAR_MAX_VOLT(mv) ((((mv) / 50) & PDO_VOLT_MASK) << PDO_VAR_MAX_VOLT_SHIFT)
#define PDO_VAR_MAX_CURR(ma) ((((ma) / 10) & PDO_CURR_MASK) << PDO_VAR_MAX_CURR_SHIFT)

#define PDO_VAR(min_mv, max_mv, max_ma)				\
	(PDO_TYPE(PDO_TYPE_VAR) | PDO_VAR_MIN_VOLT(min_mv) |	\
	 PDO_VAR_MAX_VOLT(max_mv) | PDO_VAR_MAX_CURR(max_ma))

#define APDO_TYPE_PPS		0

#define PDO_APDO_TYPE_SHIFT	28	
#define PDO_APDO_TYPE_MASK	0x3

#define PDO_APDO_TYPE(t)	((t) << PDO_APDO_TYPE_SHIFT)

#define PDO_PPS_APDO_MAX_VOLT_SHIFT	17	
#define PDO_PPS_APDO_MIN_VOLT_SHIFT	8	
#define PDO_PPS_APDO_MAX_CURR_SHIFT	0	

#define PDO_PPS_APDO_VOLT_MASK	0xff
#define PDO_PPS_APDO_CURR_MASK	0x7f

#define PDO_PPS_APDO_MIN_VOLT(mv)	\
	((((mv) / 100) & PDO_PPS_APDO_VOLT_MASK) << PDO_PPS_APDO_MIN_VOLT_SHIFT)
#define PDO_PPS_APDO_MAX_VOLT(mv)	\
	((((mv) / 100) & PDO_PPS_APDO_VOLT_MASK) << PDO_PPS_APDO_MAX_VOLT_SHIFT)
#define PDO_PPS_APDO_MAX_CURR(ma)	\
	((((ma) / 50) & PDO_PPS_APDO_CURR_MASK) << PDO_PPS_APDO_MAX_CURR_SHIFT)

#define PDO_PPS_APDO(min_mv, max_mv, max_ma)					\
	(PDO_TYPE(PDO_TYPE_APDO) | PDO_APDO_TYPE(APDO_TYPE_PPS) |		\
	 PDO_PPS_APDO_MIN_VOLT(min_mv) | PDO_PPS_APDO_MAX_VOLT(max_mv) |	\
	 PDO_PPS_APDO_MAX_CURR(max_ma))

 
#define FRS_DEFAULT_POWER      1
#define FRS_5V_1P5A            2
#define FRS_5V_3A              3




#define IDH_PTYPE_UNDEF		0


#define IDH_PTYPE_NOT_UFP       0
#define IDH_PTYPE_HUB           1
#define IDH_PTYPE_PERIPH        2
#define IDH_PTYPE_PSD           3
#define IDH_PTYPE_AMA           5


#define IDH_PTYPE_NOT_CABLE     0
#define IDH_PTYPE_PCABLE        3
#define IDH_PTYPE_ACABLE        4
#define IDH_PTYPE_VPD           6


#define IDH_PTYPE_NOT_DFP       0
#define IDH_PTYPE_DFP_HUB       1
#define IDH_PTYPE_DFP_HOST      2
#define IDH_PTYPE_DFP_PB        3

#define VDO_IDH(usbh, usbd, ufp_cable, is_modal, dfp, conn, vid)                \
	((usbh) << 31 | (usbd) << 30 | ((ufp_cable) & 0x7) << 27                \
	 | (is_modal) << 26 | ((dfp) & 0x7) << 23 | ((conn) & 0x3) << 21        \
	 | ((vid) & 0xffff))


#define VDO_CERT(xid)		((xid) & 0xffffffff)


#define VDO_PRODUCT(pid, bcd)   (((pid) & 0xffff) << 16 | ((bcd) & 0xffff))



#define UFP_VDO_VER1_2		2


#define DEV_USB2_CAPABLE	(1 << 0)
#define DEV_USB2_BILLBOARD	(1 << 1)
#define DEV_USB3_CAPABLE	(1 << 2)
#define DEV_USB4_CAPABLE	(1 << 3)


#define UFP_RECEPTACLE		2
#define UFP_CAPTIVE		3


#define AMA_VCONN_PWR_1W	0
#define AMA_VCONN_PWR_1W5	1
#define AMA_VCONN_PWR_2W	2
#define AMA_VCONN_PWR_3W	3
#define AMA_VCONN_PWR_4W	4
#define AMA_VCONN_PWR_5W	5
#define AMA_VCONN_PWR_6W	6


#define AMA_VCONN_NOT_REQ	0
#define AMA_VCONN_REQ		1


#define AMA_VBUS_REQ		0
#define AMA_VBUS_NOT_REQ	1


#define UFP_ALTMODE_NOT_SUPP	0
#define UFP_ALTMODE_TBT3	(1 << 0)
#define UFP_ALTMODE_RECFG	(1 << 1)
#define UFP_ALTMODE_NO_RECFG	(1 << 2)


#define UFP_USB2_ONLY		0
#define UFP_USB32_GEN1		1
#define UFP_USB32_4_GEN2	2
#define UFP_USB4_GEN3		3

#define VDO_UFP(ver, cap, conn, vcpwr, vcr, vbr, alt, spd)			\
	(((ver) & 0x7) << 29 | ((cap) & 0xf) << 24 | ((conn) & 0x3) << 22	\
	 | ((vcpwr) & 0x7) << 8 | (vcr) << 7 | (vbr) << 6 | ((alt) & 0x7) << 3	\
	 | ((spd) & 0x7))


#define DFP_VDO_VER1_1		1
#define HOST_USB2_CAPABLE	(1 << 0)
#define HOST_USB3_CAPABLE	(1 << 1)
#define HOST_USB4_CAPABLE	(1 << 2)
#define DFP_RECEPTACLE		2
#define DFP_CAPTIVE		3

#define VDO_DFP(ver, cap, conn, pnum)						\
	(((ver) & 0x7) << 29 | ((cap) & 0x7) << 24 | ((conn) & 0x3) << 22	\
	 | ((pnum) & 0x1f))



#define CABLE_VDO_VER1_0	0
#define CABLE_VDO_VER1_3	3


#define CABLE_ATYPE		0
#define CABLE_BTYPE		1
#define CABLE_CTYPE		2
#define CABLE_CAPTIVE		3


#define CABLE_LATENCY_1M	1
#define CABLE_LATENCY_2M	2
#define CABLE_LATENCY_3M	3
#define CABLE_LATENCY_4M	4
#define CABLE_LATENCY_5M	5
#define CABLE_LATENCY_6M	6
#define CABLE_LATENCY_7M	7
#define CABLE_LATENCY_7M_PLUS	8


#define PCABLE_VCONN_NOT_REQ	0
#define PCABLE_VCONN_REQ	1
#define ACABLE_ONE_END		2
#define ACABLE_BOTH_END		3


#define CABLE_MAX_VBUS_20V	0
#define CABLE_MAX_VBUS_30V	1
#define CABLE_MAX_VBUS_40V	2
#define CABLE_MAX_VBUS_50V	3


#define ACABLE_SBU_SUPP		0
#define ACABLE_SBU_NOT_SUPP	1
#define ACABLE_SBU_PASSIVE	0
#define ACABLE_SBU_ACTIVE	1


#define CABLE_CURR_DEF		0
#define CABLE_CURR_3A		1
#define CABLE_CURR_5A		2


#define CABLE_USBSS_U2_ONLY	0
#define CABLE_USBSS_U31_GEN1	1
#define CABLE_USBSS_U31_GEN2	2


#define CABLE_USB2_ONLY		0
#define CABLE_USB32_GEN1	1
#define CABLE_USB32_4_GEN2	2
#define CABLE_USB4_GEN3		3

#define VDO_CABLE(hw, fw, cbl, lat, term, tx1d, tx2d, rx1d, rx2d, cur, vps, sopp, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24 | ((cbl) & 0x3) << 18		\
	 | ((lat) & 0x7) << 13 | ((term) & 0x3) << 11 | (tx1d) << 10		\
	 | (tx2d) << 9 | (rx1d) << 8 | (rx2d) << 7 | ((cur) & 0x3) << 5		\
	 | (vps) << 4 | (sopp) << 3 | ((usbss) & 0x7))
#define VDO_PCABLE(hw, fw, ver, conn, lat, term, vbm, cur, spd)			\
	(((hw) & 0xf) << 28 | ((fw) & 0xf) << 24 | ((ver) & 0x7) << 21		\
	 | ((conn) & 0x3) << 18 | ((lat) & 0xf) << 13 | ((term) & 0x3) << 11	\
	 | ((vbm) & 0x3) << 9 | ((cur) & 0x3) << 5 | ((spd) & 0x7))
#define VDO_ACABLE1(hw, fw, ver, conn, lat, term, vbm, sbu, sbut, cur, vbt, sopp, spd) \
	(((hw) & 0xf) << 28 | ((fw) & 0xf) << 24 | ((ver) & 0x7) << 21		\
	 | ((conn) & 0x3) << 18	| ((lat) & 0xf) << 13 | ((term) & 0x3) << 11	\
	 | ((vbm) & 0x3) << 9 | (sbu) << 8 | (sbut) << 7 | ((cur) & 0x3) << 5	\
	 | (vbt) << 4 | (sopp) << 3 | ((spd) & 0x7))



#define ACAB2_U3_CLD_10MW_PLUS	0
#define ACAB2_U3_CLD_10MW	1
#define ACAB2_U3_CLD_5MW	2
#define ACAB2_U3_CLD_1MW	3
#define ACAB2_U3_CLD_500UW	4
#define ACAB2_U3_CLD_200UW	5
#define ACAB2_U3_CLD_50UW	6


#define ACAB2_U3U0_DIRECT	0
#define ACAB2_U3U0_U3S		1
#define ACAB2_PHY_COPPER	0
#define ACAB2_PHY_OPTICAL	1
#define ACAB2_REDRIVER		0
#define ACAB2_RETIMER		1
#define ACAB2_USB4_SUPP		0
#define ACAB2_USB4_NOT_SUPP	1
#define ACAB2_USB2_SUPP		0
#define ACAB2_USB2_NOT_SUPP	1
#define ACAB2_USB32_SUPP	0
#define ACAB2_USB32_NOT_SUPP	1
#define ACAB2_LANES_ONE		0
#define ACAB2_LANES_TWO		1
#define ACAB2_OPT_ISO_NO	0
#define ACAB2_OPT_ISO_YES	1
#define ACAB2_GEN_1		0
#define ACAB2_GEN_2_PLUS	1

#define VDO_ACABLE2(mtemp, stemp, u3p, trans, phy, ele, u4, hops, u2, u32, lane, iso, gen)	\
	(((mtemp) & 0xff) << 24 | ((stemp) & 0xff) << 16 | ((u3p) & 0x7) << 12	\
	 | (trans) << 11 | (phy) << 10 | (ele) << 9 | (u4) << 8			\
	 | ((hops) & 0x3) << 6 | (u2) << 5 | (u32) << 4 | (lane) << 3		\
	 | (iso) << 2 | (gen))


#define VDO_AMA(hw, fw, tx1d, tx2d, rx1d, rx2d, vcpwr, vcr, vbr, usbss) \
	(((hw) & 0x7) << 28 | ((fw) & 0x7) << 24			\
	 | (tx1d) << 11 | (tx2d) << 10 | (rx1d) << 9 | (rx2d) << 8	\
	 | ((vcpwr) & 0x7) << 5 | (vcr) << 4 | (vbr) << 3		\
	 | ((usbss) & 0x7))

#define PD_VDO_AMA_VCONN_REQ(vdo)	(((vdo) >> 4) & 1)
#define PD_VDO_AMA_VBUS_REQ(vdo)	(((vdo) >> 3) & 1)

#define AMA_USBSS_U2_ONLY	0
#define AMA_USBSS_U31_GEN1	1
#define AMA_USBSS_U31_GEN2	2
#define AMA_USBSS_BBONLY	3


#define VPD_VDO_VER1_0		0
#define VPD_MAX_VBUS_20V	0
#define VPD_MAX_VBUS_30V	1
#define VPD_MAX_VBUS_40V	2
#define VPD_MAX_VBUS_50V	3
#define VPDCT_CURR_3A		0
#define VPDCT_CURR_5A		1
#define VPDCT_NOT_SUPP		0
#define VPDCT_SUPP		1

#define VDO_VPD(hw, fw, ver, vbm, curr, vbi, gi, ct)			\
	(((hw) & 0xf) << 28 | ((fw) & 0xf) << 24 | ((ver) & 0x7) << 21	\
	 | ((vbm) & 0x3) << 15 | (curr) << 14 | ((vbi) & 0x3f) << 7	\
	 | ((gi) & 0x3f) << 1 | (ct))

#endif 
