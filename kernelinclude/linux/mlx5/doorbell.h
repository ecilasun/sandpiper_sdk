

#ifndef MLX5_DOORBELL_H
#define MLX5_DOORBELL_H

#define MLX5_BF_OFFSET	      0x800
#define MLX5_CQ_DOORBELL      0x20



static inline void mlx5_write64(__be32 val[2], void __iomem *dest)
{
#if BITS_PER_LONG == 64
	__raw_writeq(*(u64 *)val, dest);
#else
	__raw_writel((__force u32) val[0], dest);
	__raw_writel((__force u32) val[1], dest + 4);
#endif
}

#endif 
