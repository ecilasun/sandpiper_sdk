

#ifndef MLX4_DRIVER_H
#define MLX4_DRIVER_H

#include <net/devlink.h>
#include <linux/auxiliary_bus.h>
#include <linux/notifier.h>
#include <linux/mlx4/device.h>

#define MLX4_ADEV_NAME "mlx4_core"

struct mlx4_dev;

#define MLX4_MAC_MASK	   0xffffffffffffULL

enum mlx4_dev_event {
	MLX4_DEV_EVENT_CATASTROPHIC_ERROR,
	MLX4_DEV_EVENT_PORT_UP,
	MLX4_DEV_EVENT_PORT_DOWN,
	MLX4_DEV_EVENT_PORT_REINIT,
	MLX4_DEV_EVENT_PORT_MGMT_CHANGE,
	MLX4_DEV_EVENT_SLAVE_INIT,
	MLX4_DEV_EVENT_SLAVE_SHUTDOWN,
};

enum {
	MLX4_INTFF_BONDING	= 1 << 0
};

struct mlx4_adrv {
	struct auxiliary_driver	adrv;
	enum mlx4_protocol	protocol;
	int			flags;
};

int mlx4_register_auxiliary_driver(struct mlx4_adrv *madrv);
void mlx4_unregister_auxiliary_driver(struct mlx4_adrv *madrv);

int mlx4_register_event_notifier(struct mlx4_dev *dev,
				 struct notifier_block *nb);
int mlx4_unregister_event_notifier(struct mlx4_dev *dev,
				   struct notifier_block *nb);

struct devlink_port *mlx4_get_devlink_port(struct mlx4_dev *dev, int port);

#endif 
