/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __IOSYS_MAP_H__
#define __IOSYS_MAP_H__

#include <linux/compiler_types.h>
#include <linux/io.h>
#include <linux/string.h>




struct iosys_map {
	union {
		void __iomem *vaddr_iomem;
		void *vaddr;
	};
	bool is_iomem;
};


#define IOSYS_MAP_INIT_VADDR(vaddr_)	\
	{				\
		.vaddr = (vaddr_),	\
		.is_iomem = false,	\
	}


#define IOSYS_MAP_INIT_VADDR_IOMEM(vaddr_iomem_)	\
	{						\
		.vaddr_iomem = (vaddr_iomem_),		\
		.is_iomem = true,			\
	}


#define IOSYS_MAP_INIT_OFFSET(map_, offset_) ({				\
	struct iosys_map copy = *map_;					\
	iosys_map_incr(&copy, offset_);					\
	copy;								\
})


static inline void iosys_map_set_vaddr(struct iosys_map *map, void *vaddr)
{
	map->vaddr = vaddr;
	map->is_iomem = false;
}


static inline void iosys_map_set_vaddr_iomem(struct iosys_map *map,
					     void __iomem *vaddr_iomem)
{
	map->vaddr_iomem = vaddr_iomem;
	map->is_iomem = true;
}


static inline bool iosys_map_is_equal(const struct iosys_map *lhs,
				      const struct iosys_map *rhs)
{
	if (lhs->is_iomem != rhs->is_iomem)
		return false;
	else if (lhs->is_iomem)
		return lhs->vaddr_iomem == rhs->vaddr_iomem;
	else
		return lhs->vaddr == rhs->vaddr;
}


static inline bool iosys_map_is_null(const struct iosys_map *map)
{
	if (map->is_iomem)
		return !map->vaddr_iomem;
	return !map->vaddr;
}


static inline bool iosys_map_is_set(const struct iosys_map *map)
{
	return !iosys_map_is_null(map);
}


static inline void iosys_map_clear(struct iosys_map *map)
{
	if (map->is_iomem) {
		map->vaddr_iomem = NULL;
		map->is_iomem = false;
	} else {
		map->vaddr = NULL;
	}
}


static inline void iosys_map_memcpy_to(struct iosys_map *dst, size_t dst_offset,
				       const void *src, size_t len)
{
	if (dst->is_iomem)
		memcpy_toio(dst->vaddr_iomem + dst_offset, src, len);
	else
		memcpy(dst->vaddr + dst_offset, src, len);
}


static inline void iosys_map_memcpy_from(void *dst, const struct iosys_map *src,
					 size_t src_offset, size_t len)
{
	if (src->is_iomem)
		memcpy_fromio(dst, src->vaddr_iomem + src_offset, len);
	else
		memcpy(dst, src->vaddr + src_offset, len);
}


static inline void iosys_map_incr(struct iosys_map *map, size_t incr)
{
	if (map->is_iomem)
		map->vaddr_iomem += incr;
	else
		map->vaddr += incr;
}


static inline void iosys_map_memset(struct iosys_map *dst, size_t offset,
				    int value, size_t len)
{
	if (dst->is_iomem)
		memset_io(dst->vaddr_iomem + offset, value, len);
	else
		memset(dst->vaddr + offset, value, len);
}

#ifdef CONFIG_64BIT
#define __iosys_map_rd_io_u64_case(val_, vaddr_iomem_)				\
	u64: val_ = readq(vaddr_iomem_)
#define __iosys_map_wr_io_u64_case(val_, vaddr_iomem_)				\
	u64: writeq(val_, vaddr_iomem_)
#else
#define __iosys_map_rd_io_u64_case(val_, vaddr_iomem_)				\
	u64: memcpy_fromio(&(val_), vaddr_iomem_, sizeof(u64))
#define __iosys_map_wr_io_u64_case(val_, vaddr_iomem_)				\
	u64: memcpy_toio(vaddr_iomem_, &(val_), sizeof(u64))
#endif

#define __iosys_map_rd_io(val__, vaddr_iomem__, type__) _Generic(val__,		\
	u8: val__ = readb(vaddr_iomem__),					\
	u16: val__ = readw(vaddr_iomem__),					\
	u32: val__ = readl(vaddr_iomem__),					\
	__iosys_map_rd_io_u64_case(val__, vaddr_iomem__))

#define __iosys_map_rd_sys(val__, vaddr__, type__)				\
	val__ = READ_ONCE(*(type__ *)(vaddr__))

#define __iosys_map_wr_io(val__, vaddr_iomem__, type__) _Generic(val__,		\
	u8: writeb(val__, vaddr_iomem__),					\
	u16: writew(val__, vaddr_iomem__),					\
	u32: writel(val__, vaddr_iomem__),					\
	__iosys_map_wr_io_u64_case(val__, vaddr_iomem__))

#define __iosys_map_wr_sys(val__, vaddr__, type__)				\
	WRITE_ONCE(*(type__ *)(vaddr__), val__)


#define iosys_map_rd(map__, offset__, type__) ({				\
	type__ val;								\
	if ((map__)->is_iomem) {						\
		__iosys_map_rd_io(val, (map__)->vaddr_iomem + (offset__), type__);\
	} else {								\
		__iosys_map_rd_sys(val, (map__)->vaddr + (offset__), type__);	\
	}									\
	val;									\
})


#define iosys_map_wr(map__, offset__, type__, val__) ({				\
	type__ val = (val__);							\
	if ((map__)->is_iomem) {						\
		__iosys_map_wr_io(val, (map__)->vaddr_iomem + (offset__), type__);\
	} else {								\
		__iosys_map_wr_sys(val, (map__)->vaddr + (offset__), type__);	\
	}									\
})


#define iosys_map_rd_field(map__, struct_offset__, struct_type__, field__) ({	\
	struct_type__ *s;							\
	iosys_map_rd(map__, struct_offset__ + offsetof(struct_type__, field__),	\
		     typeof(s->field__));					\
})


#define iosys_map_wr_field(map__, struct_offset__, struct_type__, field__, val__) ({	\
	struct_type__ *s;								\
	iosys_map_wr(map__, struct_offset__ + offsetof(struct_type__, field__),		\
		     typeof(s->field__), val__);					\
})

#endif 
