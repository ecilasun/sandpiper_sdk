

#ifndef __TRANSOBJ_H__
#define __TRANSOBJ_H__

#include <linux/mlx5/driver.h>

int mlx5_core_alloc_transport_domain(struct mlx5_core_dev *dev, u32 *tdn);
void mlx5_core_dealloc_transport_domain(struct mlx5_core_dev *dev, u32 tdn);
int mlx5_core_create_rq(struct mlx5_core_dev *dev, u32 *in, int inlen,
			u32 *rqn);
int mlx5_core_modify_rq(struct mlx5_core_dev *dev, u32 rqn, u32 *in);
void mlx5_core_destroy_rq(struct mlx5_core_dev *dev, u32 rqn);
int mlx5_core_query_rq(struct mlx5_core_dev *dev, u32 rqn, u32 *out);
int mlx5_core_create_sq(struct mlx5_core_dev *dev, u32 *in, int inlen,
			u32 *sqn);
int mlx5_core_modify_sq(struct mlx5_core_dev *dev, u32 sqn, u32 *in);
void mlx5_core_destroy_sq(struct mlx5_core_dev *dev, u32 sqn);
int mlx5_core_query_sq(struct mlx5_core_dev *dev, u32 sqn, u32 *out);
int mlx5_core_query_sq_state(struct mlx5_core_dev *dev, u32 sqn, u8 *state);
int mlx5_core_create_tir(struct mlx5_core_dev *dev, u32 *in, u32 *tirn);
int mlx5_core_modify_tir(struct mlx5_core_dev *dev, u32 tirn, u32 *in);
void mlx5_core_destroy_tir(struct mlx5_core_dev *dev, u32 tirn);
int mlx5_core_create_tis(struct mlx5_core_dev *dev, u32 *in, u32 *tisn);
int mlx5_core_modify_tis(struct mlx5_core_dev *dev, u32 tisn, u32 *in);
void mlx5_core_destroy_tis(struct mlx5_core_dev *dev, u32 tisn);
int mlx5_core_create_rqt(struct mlx5_core_dev *dev, u32 *in, int inlen,
			 u32 *rqtn);
int mlx5_core_modify_rqt(struct mlx5_core_dev *dev, u32 rqtn, u32 *in,
			 int inlen);
void mlx5_core_destroy_rqt(struct mlx5_core_dev *dev, u32 rqtn);

struct mlx5_hairpin_params {
	u8  log_data_size;
	u8  log_num_packets;
	u16 q_counter;
	int num_channels;
};

struct mlx5_hairpin {
	struct mlx5_core_dev *func_mdev;
	struct mlx5_core_dev *peer_mdev;

	int num_channels;

	u32 *rqn;
	u32 *sqn;

	bool peer_gone;
};

struct mlx5_hairpin *
mlx5_core_hairpin_create(struct mlx5_core_dev *func_mdev,
			 struct mlx5_core_dev *peer_mdev,
			 struct mlx5_hairpin_params *params);

void mlx5_core_hairpin_destroy(struct mlx5_hairpin *pair);
void mlx5_core_hairpin_clear_dead_peer(struct mlx5_hairpin *hp);
#endif 
