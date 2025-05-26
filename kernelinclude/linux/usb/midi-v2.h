/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_USB_MIDI_V2_H
#define __LINUX_USB_MIDI_V2_H

#include <linux/types.h>
#include <linux/usb/midi.h>


#define USB_DT_CS_GR_TRM_BLOCK	0x26





#define USB_MS_GENERAL_2_0	0x02


#define USB_MS_GR_TRM_BLOCK_UNDEFINED	0x00
#define USB_MS_GR_TRM_BLOCK_HEADER	0x01
#define USB_MS_GR_TRM_BLOCK		0x02


#define USB_MS_REV_MIDI_1_0		0x0100
#define USB_MS_REV_MIDI_2_0		0x0200





#define USB_MS_GR_TRM_BLOCK_TYPE_BIDIRECTIONAL	0x00
#define USB_MS_GR_TRM_BLOCK_TYPE_INPUT_ONLY	0x01
#define USB_MS_GR_TRM_BLOCK_TYPE_OUTPUT_ONLY	0x02


#define USB_MS_MIDI_PROTO_UNKNOWN	0x00 
#define USB_MS_MIDI_PROTO_1_0_64	0x01 
#define USB_MS_MIDI_PROTO_1_0_64_JRTS	0x02 
#define USB_MS_MIDI_PROTO_1_0_128	0x03 
#define USB_MS_MIDI_PROTO_1_0_128_JRTS	0x04 
#define USB_MS_MIDI_PROTO_2_0		0x11 
#define USB_MS_MIDI_PROTO_2_0_JRTS	0x12 





struct usb_ms20_endpoint_descriptor {
	__u8  bLength;			
	__u8  bDescriptorType;		
	__u8  bDescriptorSubtype;	
	__u8  bNumGrpTrmBlock;		
	__u8  baAssoGrpTrmBlkID[];	
} __packed;

#define USB_DT_MS20_ENDPOINT_SIZE(n)	(4 + (n))


#define DECLARE_USB_MS20_ENDPOINT_DESCRIPTOR(n)			\
struct usb_ms20_endpoint_descriptor_##n {			\
	__u8  bLength;						\
	__u8  bDescriptorType;					\
	__u8  bDescriptorSubtype;				\
	__u8  bNumGrpTrmBlock;					\
	__u8  baAssoGrpTrmBlkID[n];				\
} __packed


struct usb_ms20_gr_trm_block_header_descriptor {
	__u8  bLength;			
	__u8  bDescriptorType;		
	__u8  bDescriptorSubtype;	
	__le16 wTotalLength;		
} __packed;


struct usb_ms20_gr_trm_block_descriptor {
	__u8  bLength;			
	__u8  bDescriptorType;		
	__u8  bDescriptorSubtype;	
	__u8  bGrpTrmBlkID;		
	__u8  bGrpTrmBlkType;		
	__u8  nGroupTrm;		
	__u8  nNumGroupTrm;		
	__u8  iBlockItem;		
	__u8  bMIDIProtocol;		
	__le16 wMaxInputBandwidth;	
	__le16 wMaxOutputBandwidth;	
} __packed;

#endif 
