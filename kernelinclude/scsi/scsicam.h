/* SPDX-License-Identifier: GPL-2.0 */


#ifndef SCSICAM_H
#define SCSICAM_H
int scsicam_bios_param(struct block_device *bdev, sector_t capacity, int *ip);
bool scsi_partsize(struct block_device *bdev, sector_t capacity, int geom[3]);
unsigned char *scsi_bios_ptable(struct block_device *bdev);
#endif 
