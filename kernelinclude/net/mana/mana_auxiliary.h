/* SPDX-License-Identifier: GPL-2.0-only */


#include "mana.h"
#include <linux/auxiliary_bus.h>

struct mana_adev {
	struct auxiliary_device adev;
	struct gdma_dev *mdev;
};
