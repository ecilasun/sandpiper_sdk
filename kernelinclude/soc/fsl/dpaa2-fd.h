/* SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause) */

#ifndef __FSL_DPAA2_FD_H
#define __FSL_DPAA2_FD_H

#include <linux/byteorder/generic.h>
#include <linux/types.h>




struct dpaa2_fd {
	union {
		u32 words[8];
		struct dpaa2_fd_simple {
			__le64 addr;
			__le32 len;
			__le16 bpid;
			__le16 format_offset;
			__le32 frc;
			__le32 ctrl;
			__le64 flc;
		} simple;
	};
};

#define FD_SHORT_LEN_FLAG_MASK	0x1
#define FD_SHORT_LEN_FLAG_SHIFT	14
#define FD_SHORT_LEN_MASK	0x3FFFF
#define FD_OFFSET_MASK		0x0FFF
#define FD_FORMAT_MASK		0x3
#define FD_FORMAT_SHIFT		12
#define FD_BPID_MASK		0x3FFF
#define SG_SHORT_LEN_FLAG_MASK	0x1
#define SG_SHORT_LEN_FLAG_SHIFT	14
#define SG_SHORT_LEN_MASK	0x1FFFF
#define SG_OFFSET_MASK		0x0FFF
#define SG_FORMAT_MASK		0x3
#define SG_FORMAT_SHIFT		12
#define SG_BPID_MASK		0x3FFF
#define SG_FINAL_FLAG_MASK	0x1
#define SG_FINAL_FLAG_SHIFT	15
#define FL_SHORT_LEN_FLAG_MASK	0x1
#define FL_SHORT_LEN_FLAG_SHIFT	14
#define FL_SHORT_LEN_MASK	0x3FFFF
#define FL_OFFSET_MASK		0x0FFF
#define FL_FORMAT_MASK		0x3
#define FL_FORMAT_SHIFT		12
#define FL_BPID_MASK		0x3FFF
#define FL_FINAL_FLAG_MASK	0x1
#define FL_FINAL_FLAG_SHIFT	15


#define FD_CTRL_ERR_MASK	0x000000FF
#define FD_CTRL_UFD		0x00000004
#define FD_CTRL_SBE		0x00000008
#define FD_CTRL_FLC		0x00000010
#define FD_CTRL_FSE		0x00000020
#define FD_CTRL_FAERR		0x00000040


#define FD_CTRL_PTA		0x00800000
#define FD_CTRL_PTV1		0x00400000

enum dpaa2_fd_format {
	dpaa2_fd_single = 0,
	dpaa2_fd_list,
	dpaa2_fd_sg
};


static inline dma_addr_t dpaa2_fd_get_addr(const struct dpaa2_fd *fd)
{
	return (dma_addr_t)le64_to_cpu(fd->simple.addr);
}


static inline void dpaa2_fd_set_addr(struct dpaa2_fd *fd, dma_addr_t addr)
{
	fd->simple.addr = cpu_to_le64(addr);
}


static inline u32 dpaa2_fd_get_frc(const struct dpaa2_fd *fd)
{
	return le32_to_cpu(fd->simple.frc);
}


static inline void dpaa2_fd_set_frc(struct dpaa2_fd *fd, u32 frc)
{
	fd->simple.frc = cpu_to_le32(frc);
}


static inline u32 dpaa2_fd_get_ctrl(const struct dpaa2_fd *fd)
{
	return le32_to_cpu(fd->simple.ctrl);
}


static inline void dpaa2_fd_set_ctrl(struct dpaa2_fd *fd, u32 ctrl)
{
	fd->simple.ctrl = cpu_to_le32(ctrl);
}


static inline dma_addr_t dpaa2_fd_get_flc(const struct dpaa2_fd *fd)
{
	return (dma_addr_t)le64_to_cpu(fd->simple.flc);
}


static inline void dpaa2_fd_set_flc(struct dpaa2_fd *fd,  dma_addr_t flc_addr)
{
	fd->simple.flc = cpu_to_le64(flc_addr);
}

static inline bool dpaa2_fd_short_len(const struct dpaa2_fd *fd)
{
	return !!((le16_to_cpu(fd->simple.format_offset) >>
		  FD_SHORT_LEN_FLAG_SHIFT) & FD_SHORT_LEN_FLAG_MASK);
}


static inline u32 dpaa2_fd_get_len(const struct dpaa2_fd *fd)
{
	if (dpaa2_fd_short_len(fd))
		return le32_to_cpu(fd->simple.len) & FD_SHORT_LEN_MASK;

	return le32_to_cpu(fd->simple.len);
}


static inline void dpaa2_fd_set_len(struct dpaa2_fd *fd, u32 len)
{
	fd->simple.len = cpu_to_le32(len);
}


static inline uint16_t dpaa2_fd_get_offset(const struct dpaa2_fd *fd)
{
	return le16_to_cpu(fd->simple.format_offset) & FD_OFFSET_MASK;
}


static inline void dpaa2_fd_set_offset(struct dpaa2_fd *fd, uint16_t offset)
{
	fd->simple.format_offset &= cpu_to_le16(~FD_OFFSET_MASK);
	fd->simple.format_offset |= cpu_to_le16(offset);
}


static inline enum dpaa2_fd_format dpaa2_fd_get_format(
						const struct dpaa2_fd *fd)
{
	return (enum dpaa2_fd_format)((le16_to_cpu(fd->simple.format_offset)
				      >> FD_FORMAT_SHIFT) & FD_FORMAT_MASK);
}


static inline void dpaa2_fd_set_format(struct dpaa2_fd *fd,
				       enum dpaa2_fd_format format)
{
	fd->simple.format_offset &=
		cpu_to_le16(~(FD_FORMAT_MASK << FD_FORMAT_SHIFT));
	fd->simple.format_offset |= cpu_to_le16(format << FD_FORMAT_SHIFT);
}


static inline uint16_t dpaa2_fd_get_bpid(const struct dpaa2_fd *fd)
{
	return le16_to_cpu(fd->simple.bpid) & FD_BPID_MASK;
}


static inline void dpaa2_fd_set_bpid(struct dpaa2_fd *fd, uint16_t bpid)
{
	fd->simple.bpid &= cpu_to_le16(~(FD_BPID_MASK));
	fd->simple.bpid |= cpu_to_le16(bpid);
}


struct dpaa2_sg_entry {
	__le64 addr;
	__le32 len;
	__le16 bpid;
	__le16 format_offset;
};

enum dpaa2_sg_format {
	dpaa2_sg_single = 0,
	dpaa2_sg_frame_data,
	dpaa2_sg_sgt_ext
};




static inline dma_addr_t dpaa2_sg_get_addr(const struct dpaa2_sg_entry *sg)
{
	return (dma_addr_t)le64_to_cpu(sg->addr);
}


static inline void dpaa2_sg_set_addr(struct dpaa2_sg_entry *sg, dma_addr_t addr)
{
	sg->addr = cpu_to_le64(addr);
}

static inline bool dpaa2_sg_short_len(const struct dpaa2_sg_entry *sg)
{
	return !!((le16_to_cpu(sg->format_offset) >> SG_SHORT_LEN_FLAG_SHIFT)
		& SG_SHORT_LEN_FLAG_MASK);
}


static inline u32 dpaa2_sg_get_len(const struct dpaa2_sg_entry *sg)
{
	if (dpaa2_sg_short_len(sg))
		return le32_to_cpu(sg->len) & SG_SHORT_LEN_MASK;

	return le32_to_cpu(sg->len);
}


static inline void dpaa2_sg_set_len(struct dpaa2_sg_entry *sg, u32 len)
{
	sg->len = cpu_to_le32(len);
}


static inline u16 dpaa2_sg_get_offset(const struct dpaa2_sg_entry *sg)
{
	return le16_to_cpu(sg->format_offset) & SG_OFFSET_MASK;
}


static inline void dpaa2_sg_set_offset(struct dpaa2_sg_entry *sg,
				       u16 offset)
{
	sg->format_offset &= cpu_to_le16(~SG_OFFSET_MASK);
	sg->format_offset |= cpu_to_le16(offset);
}


static inline enum dpaa2_sg_format
	dpaa2_sg_get_format(const struct dpaa2_sg_entry *sg)
{
	return (enum dpaa2_sg_format)((le16_to_cpu(sg->format_offset)
				       >> SG_FORMAT_SHIFT) & SG_FORMAT_MASK);
}


static inline void dpaa2_sg_set_format(struct dpaa2_sg_entry *sg,
				       enum dpaa2_sg_format format)
{
	sg->format_offset &= cpu_to_le16(~(SG_FORMAT_MASK << SG_FORMAT_SHIFT));
	sg->format_offset |= cpu_to_le16(format << SG_FORMAT_SHIFT);
}


static inline u16 dpaa2_sg_get_bpid(const struct dpaa2_sg_entry *sg)
{
	return le16_to_cpu(sg->bpid) & SG_BPID_MASK;
}


static inline void dpaa2_sg_set_bpid(struct dpaa2_sg_entry *sg, u16 bpid)
{
	sg->bpid &= cpu_to_le16(~(SG_BPID_MASK));
	sg->bpid |= cpu_to_le16(bpid);
}


static inline bool dpaa2_sg_is_final(const struct dpaa2_sg_entry *sg)
{
	return !!(le16_to_cpu(sg->format_offset) >> SG_FINAL_FLAG_SHIFT);
}


static inline void dpaa2_sg_set_final(struct dpaa2_sg_entry *sg, bool final)
{
	sg->format_offset &= cpu_to_le16((~(SG_FINAL_FLAG_MASK
					 << SG_FINAL_FLAG_SHIFT)) & 0xFFFF);
	sg->format_offset |= cpu_to_le16(final << SG_FINAL_FLAG_SHIFT);
}


struct dpaa2_fl_entry {
	__le64 addr;
	__le32 len;
	__le16 bpid;
	__le16 format_offset;
	__le32 frc;
	__le32 ctrl;
	__le64 flc;
};

enum dpaa2_fl_format {
	dpaa2_fl_single = 0,
	dpaa2_fl_res,
	dpaa2_fl_sg
};


static inline dma_addr_t dpaa2_fl_get_addr(const struct dpaa2_fl_entry *fle)
{
	return (dma_addr_t)le64_to_cpu(fle->addr);
}


static inline void dpaa2_fl_set_addr(struct dpaa2_fl_entry *fle,
				     dma_addr_t addr)
{
	fle->addr = cpu_to_le64(addr);
}


static inline u32 dpaa2_fl_get_frc(const struct dpaa2_fl_entry *fle)
{
	return le32_to_cpu(fle->frc);
}


static inline void dpaa2_fl_set_frc(struct dpaa2_fl_entry *fle, u32 frc)
{
	fle->frc = cpu_to_le32(frc);
}


static inline u32 dpaa2_fl_get_ctrl(const struct dpaa2_fl_entry *fle)
{
	return le32_to_cpu(fle->ctrl);
}


static inline void dpaa2_fl_set_ctrl(struct dpaa2_fl_entry *fle, u32 ctrl)
{
	fle->ctrl = cpu_to_le32(ctrl);
}


static inline dma_addr_t dpaa2_fl_get_flc(const struct dpaa2_fl_entry *fle)
{
	return (dma_addr_t)le64_to_cpu(fle->flc);
}


static inline void dpaa2_fl_set_flc(struct dpaa2_fl_entry *fle,
				    dma_addr_t flc_addr)
{
	fle->flc = cpu_to_le64(flc_addr);
}

static inline bool dpaa2_fl_short_len(const struct dpaa2_fl_entry *fle)
{
	return !!((le16_to_cpu(fle->format_offset) >>
		  FL_SHORT_LEN_FLAG_SHIFT) & FL_SHORT_LEN_FLAG_MASK);
}


static inline u32 dpaa2_fl_get_len(const struct dpaa2_fl_entry *fle)
{
	if (dpaa2_fl_short_len(fle))
		return le32_to_cpu(fle->len) & FL_SHORT_LEN_MASK;

	return le32_to_cpu(fle->len);
}


static inline void dpaa2_fl_set_len(struct dpaa2_fl_entry *fle, u32 len)
{
	fle->len = cpu_to_le32(len);
}


static inline u16 dpaa2_fl_get_offset(const struct dpaa2_fl_entry *fle)
{
	return le16_to_cpu(fle->format_offset) & FL_OFFSET_MASK;
}


static inline void dpaa2_fl_set_offset(struct dpaa2_fl_entry *fle, u16 offset)
{
	fle->format_offset &= cpu_to_le16(~FL_OFFSET_MASK);
	fle->format_offset |= cpu_to_le16(offset);
}


static inline enum dpaa2_fl_format dpaa2_fl_get_format(const struct dpaa2_fl_entry *fle)
{
	return (enum dpaa2_fl_format)((le16_to_cpu(fle->format_offset) >>
				       FL_FORMAT_SHIFT) & FL_FORMAT_MASK);
}


static inline void dpaa2_fl_set_format(struct dpaa2_fl_entry *fle,
				       enum dpaa2_fl_format format)
{
	fle->format_offset &= cpu_to_le16(~(FL_FORMAT_MASK << FL_FORMAT_SHIFT));
	fle->format_offset |= cpu_to_le16(format << FL_FORMAT_SHIFT);
}


static inline u16 dpaa2_fl_get_bpid(const struct dpaa2_fl_entry *fle)
{
	return le16_to_cpu(fle->bpid) & FL_BPID_MASK;
}


static inline void dpaa2_fl_set_bpid(struct dpaa2_fl_entry *fle, u16 bpid)
{
	fle->bpid &= cpu_to_le16(~(FL_BPID_MASK));
	fle->bpid |= cpu_to_le16(bpid);
}


static inline bool dpaa2_fl_is_final(const struct dpaa2_fl_entry *fle)
{
	return !!(le16_to_cpu(fle->format_offset) >> FL_FINAL_FLAG_SHIFT);
}


static inline void dpaa2_fl_set_final(struct dpaa2_fl_entry *fle, bool final)
{
	fle->format_offset &= cpu_to_le16((~(FL_FINAL_FLAG_MASK <<
					     FL_FINAL_FLAG_SHIFT)) & 0xFFFF);
	fle->format_offset |= cpu_to_le16(final << FL_FINAL_FLAG_SHIFT);
}

#endif 
