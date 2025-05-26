/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_DEVICE_MAPPER_H
#define _LINUX_DEVICE_MAPPER_H

#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/dm-ioctl.h>
#include <linux/math64.h>
#include <linux/ratelimit.h>

struct dm_dev;
struct dm_target;
struct dm_table;
struct dm_report_zones_args;
struct mapped_device;
struct bio_vec;
enum dax_access_mode;


enum dm_queue_mode {
	DM_TYPE_NONE		 = 0,
	DM_TYPE_BIO_BASED	 = 1,
	DM_TYPE_REQUEST_BASED	 = 2,
	DM_TYPE_DAX_BIO_BASED	 = 3,
};

typedef enum { STATUSTYPE_INFO, STATUSTYPE_TABLE, STATUSTYPE_IMA } status_type_t;

union map_info {
	void *ptr;
};


typedef int (*dm_ctr_fn) (struct dm_target *target,
			  unsigned int argc, char **argv);


typedef void (*dm_dtr_fn) (struct dm_target *ti);


typedef int (*dm_map_fn) (struct dm_target *ti, struct bio *bio);
typedef int (*dm_clone_and_map_request_fn) (struct dm_target *ti,
					    struct request *rq,
					    union map_info *map_context,
					    struct request **clone);
typedef void (*dm_release_clone_request_fn) (struct request *clone,
					     union map_info *map_context);


typedef int (*dm_endio_fn) (struct dm_target *ti,
			    struct bio *bio, blk_status_t *error);
typedef int (*dm_request_endio_fn) (struct dm_target *ti,
				    struct request *clone, blk_status_t error,
				    union map_info *map_context);

typedef void (*dm_presuspend_fn) (struct dm_target *ti);
typedef void (*dm_presuspend_undo_fn) (struct dm_target *ti);
typedef void (*dm_postsuspend_fn) (struct dm_target *ti);
typedef int (*dm_preresume_fn) (struct dm_target *ti);
typedef void (*dm_resume_fn) (struct dm_target *ti);

typedef void (*dm_status_fn) (struct dm_target *ti, status_type_t status_type,
			      unsigned int status_flags, char *result, unsigned int maxlen);

typedef int (*dm_message_fn) (struct dm_target *ti, unsigned int argc, char **argv,
			      char *result, unsigned int maxlen);

typedef int (*dm_prepare_ioctl_fn) (struct dm_target *ti, struct block_device **bdev);

#ifdef CONFIG_BLK_DEV_ZONED
typedef int (*dm_report_zones_fn) (struct dm_target *ti,
				   struct dm_report_zones_args *args,
				   unsigned int nr_zones);
#else

typedef int (*dm_report_zones_fn) (struct dm_target *dummy);
#endif


typedef int (*iterate_devices_callout_fn) (struct dm_target *ti,
					   struct dm_dev *dev,
					   sector_t start, sector_t len,
					   void *data);


typedef int (*dm_iterate_devices_fn) (struct dm_target *ti,
				      iterate_devices_callout_fn fn,
				      void *data);

typedef void (*dm_io_hints_fn) (struct dm_target *ti,
				struct queue_limits *limits);


typedef int (*dm_busy_fn) (struct dm_target *ti);


typedef long (*dm_dax_direct_access_fn) (struct dm_target *ti, pgoff_t pgoff,
		long nr_pages, enum dax_access_mode node, void **kaddr,
		pfn_t *pfn);
typedef int (*dm_dax_zero_page_range_fn)(struct dm_target *ti, pgoff_t pgoff,
		size_t nr_pages);


typedef size_t (*dm_dax_recovery_write_fn)(struct dm_target *ti, pgoff_t pgoff,
		void *addr, size_t bytes, struct iov_iter *i);

void dm_error(const char *message);

struct dm_dev {
	struct block_device *bdev;
	struct dax_device *dax_dev;
	blk_mode_t mode;
	char name[16];
};


int dm_get_device(struct dm_target *ti, const char *path, blk_mode_t mode,
		  struct dm_dev **result);
void dm_put_device(struct dm_target *ti, struct dm_dev *d);



struct target_type {
	uint64_t features;
	const char *name;
	struct module *module;
	unsigned int version[3];
	dm_ctr_fn ctr;
	dm_dtr_fn dtr;
	dm_map_fn map;
	dm_clone_and_map_request_fn clone_and_map_rq;
	dm_release_clone_request_fn release_clone_rq;
	dm_endio_fn end_io;
	dm_request_endio_fn rq_end_io;
	dm_presuspend_fn presuspend;
	dm_presuspend_undo_fn presuspend_undo;
	dm_postsuspend_fn postsuspend;
	dm_preresume_fn preresume;
	dm_resume_fn resume;
	dm_status_fn status;
	dm_message_fn message;
	dm_prepare_ioctl_fn prepare_ioctl;
	dm_report_zones_fn report_zones;
	dm_busy_fn busy;
	dm_iterate_devices_fn iterate_devices;
	dm_io_hints_fn io_hints;
	dm_dax_direct_access_fn direct_access;
	dm_dax_zero_page_range_fn dax_zero_page_range;
	dm_dax_recovery_write_fn dax_recovery_write;

	
	struct list_head list;
};




#define DM_TARGET_SINGLETON		0x00000001
#define dm_target_needs_singleton(type)	((type)->features & DM_TARGET_SINGLETON)


#define DM_TARGET_ALWAYS_WRITEABLE	0x00000002
#define dm_target_always_writeable(type) \
		((type)->features & DM_TARGET_ALWAYS_WRITEABLE)


#define DM_TARGET_IMMUTABLE		0x00000004
#define dm_target_is_immutable(type)	((type)->features & DM_TARGET_IMMUTABLE)


#define DM_TARGET_WILDCARD		0x00000008
#define dm_target_is_wildcard(type)	((type)->features & DM_TARGET_WILDCARD)


#define DM_TARGET_INTEGRITY		0x00000010
#define dm_target_has_integrity(type)	((type)->features & DM_TARGET_INTEGRITY)


#define DM_TARGET_PASSES_INTEGRITY	0x00000020
#define dm_target_passes_integrity(type) ((type)->features & DM_TARGET_PASSES_INTEGRITY)


#ifdef CONFIG_BLK_DEV_ZONED
#define DM_TARGET_ZONED_HM		0x00000040
#define dm_target_supports_zoned_hm(type) ((type)->features & DM_TARGET_ZONED_HM)
#else
#define DM_TARGET_ZONED_HM		0x00000000
#define dm_target_supports_zoned_hm(type) (false)
#endif


#define DM_TARGET_NOWAIT		0x00000080
#define dm_target_supports_nowait(type) ((type)->features & DM_TARGET_NOWAIT)


#define DM_TARGET_PASSES_CRYPTO		0x00000100
#define dm_target_passes_crypto(type) ((type)->features & DM_TARGET_PASSES_CRYPTO)

#ifdef CONFIG_BLK_DEV_ZONED
#define DM_TARGET_MIXED_ZONED_MODEL	0x00000200
#define dm_target_supports_mixed_zoned_model(type) \
	((type)->features & DM_TARGET_MIXED_ZONED_MODEL)
#else
#define DM_TARGET_MIXED_ZONED_MODEL	0x00000000
#define dm_target_supports_mixed_zoned_model(type) (false)
#endif

struct dm_target {
	struct dm_table *table;
	struct target_type *type;

	
	sector_t begin;
	sector_t len;

	
	uint32_t max_io_len;

	
	unsigned int num_flush_bios;

	
	unsigned int num_discard_bios;

	
	unsigned int num_secure_erase_bios;

	
	unsigned int num_write_zeroes_bios;

	
	unsigned int per_io_data_size;

	
	void *private;

	
	char *error;

	
	bool flush_supported:1;

	
	bool discards_supported:1;

	
	bool max_discard_granularity:1;

	
	bool max_secure_erase_granularity:1;

	
	bool max_write_zeroes_granularity:1;

	
	bool limit_swap_bios:1;

	
	bool emulate_zone_append:1;

	
	bool accounts_remapped_io:1;

	
	bool needs_bio_set_dev:1;
};

void *dm_per_bio_data(struct bio *bio, size_t data_size);
struct bio *dm_bio_from_per_bio_data(void *data, size_t data_size);
unsigned int dm_bio_get_target_bio_nr(const struct bio *bio);

u64 dm_start_time_ns_from_clone(struct bio *bio);

int dm_register_target(struct target_type *t);
void dm_unregister_target(struct target_type *t);


struct dm_arg_set {
	unsigned int argc;
	char **argv;
};


struct dm_arg {
	unsigned int min;
	unsigned int max;
	char *error;
};


int dm_read_arg(const struct dm_arg *arg, struct dm_arg_set *arg_set,
		unsigned int *value, char **error);


int dm_read_arg_group(const struct dm_arg *arg, struct dm_arg_set *arg_set,
		      unsigned int *num_args, char **error);


const char *dm_shift_arg(struct dm_arg_set *as);


void dm_consume_args(struct dm_arg_set *as, unsigned int num_args);




#define DM_ANY_MINOR (-1)
int dm_create(int minor, struct mapped_device **md);


struct mapped_device *dm_get_md(dev_t dev);
void dm_get(struct mapped_device *md);
int dm_hold(struct mapped_device *md);
void dm_put(struct mapped_device *md);


void dm_set_mdptr(struct mapped_device *md, void *ptr);
void *dm_get_mdptr(struct mapped_device *md);


int dm_suspend(struct mapped_device *md, unsigned int suspend_flags);
int dm_resume(struct mapped_device *md);


uint32_t dm_get_event_nr(struct mapped_device *md);
int dm_wait_event(struct mapped_device *md, int event_nr);
uint32_t dm_next_uevent_seq(struct mapped_device *md);
void dm_uevent_add(struct mapped_device *md, struct list_head *elist);


const char *dm_device_name(struct mapped_device *md);
int dm_copy_name_and_uuid(struct mapped_device *md, char *name, char *uuid);
struct gendisk *dm_disk(struct mapped_device *md);
int dm_suspended(struct dm_target *ti);
int dm_post_suspending(struct dm_target *ti);
int dm_noflush_suspending(struct dm_target *ti);
void dm_accept_partial_bio(struct bio *bio, unsigned int n_sectors);
void dm_submit_bio_remap(struct bio *clone, struct bio *tgt_clone);
union map_info *dm_get_rq_mapinfo(struct request *rq);

#ifdef CONFIG_BLK_DEV_ZONED
struct dm_report_zones_args {
	struct dm_target *tgt;
	sector_t next_sector;

	void *orig_data;
	report_zones_cb orig_cb;
	unsigned int zone_idx;

	
	sector_t start;
};
int dm_report_zones(struct block_device *bdev, sector_t start, sector_t sector,
		    struct dm_report_zones_args *args, unsigned int nr_zones);
#endif 


int __init dm_early_create(struct dm_ioctl *dmi,
			   struct dm_target_spec **spec_array,
			   char **target_params_array);


int dm_get_geometry(struct mapped_device *md, struct hd_geometry *geo);
int dm_set_geometry(struct mapped_device *md, struct hd_geometry *geo);




int dm_table_create(struct dm_table **result, blk_mode_t mode,
		    unsigned int num_targets, struct mapped_device *md);


int dm_table_add_target(struct dm_table *t, const char *type,
			sector_t start, sector_t len, char *params);


void dm_table_set_type(struct dm_table *t, enum dm_queue_mode type);


int dm_table_complete(struct dm_table *t);


void dm_table_destroy(struct dm_table *t);


int __must_check dm_set_target_max_io_len(struct dm_target *ti, sector_t len);


struct dm_table *dm_get_live_table(struct mapped_device *md, int *srcu_idx);
void dm_put_live_table(struct mapped_device *md, int srcu_idx);
void dm_sync_table(struct mapped_device *md);


sector_t dm_table_get_size(struct dm_table *t);
blk_mode_t dm_table_get_mode(struct dm_table *t);
struct mapped_device *dm_table_get_md(struct dm_table *t);
const char *dm_table_device_name(struct dm_table *t);


void dm_table_event(struct dm_table *t);


void dm_table_run_md_queue_async(struct dm_table *t);


struct dm_table *dm_swap_table(struct mapped_device *md,
			       struct dm_table *t);


void dm_destroy_crypto_profile(struct blk_crypto_profile *profile);


#define DM_NAME "device-mapper"

#define DM_FMT(fmt) DM_NAME ": " DM_MSG_PREFIX ": " fmt "\n"

#define DMCRIT(fmt, ...) pr_crit(DM_FMT(fmt), ##__VA_ARGS__)

#define DMERR(fmt, ...) pr_err(DM_FMT(fmt), ##__VA_ARGS__)
#define DMERR_LIMIT(fmt, ...) pr_err_ratelimited(DM_FMT(fmt), ##__VA_ARGS__)
#define DMWARN(fmt, ...) pr_warn(DM_FMT(fmt), ##__VA_ARGS__)
#define DMWARN_LIMIT(fmt, ...) pr_warn_ratelimited(DM_FMT(fmt), ##__VA_ARGS__)
#define DMINFO(fmt, ...) pr_info(DM_FMT(fmt), ##__VA_ARGS__)
#define DMINFO_LIMIT(fmt, ...) pr_info_ratelimited(DM_FMT(fmt), ##__VA_ARGS__)

#define DMDEBUG(fmt, ...) pr_debug(DM_FMT(fmt), ##__VA_ARGS__)
#define DMDEBUG_LIMIT(fmt, ...) pr_debug_ratelimited(DM_FMT(fmt), ##__VA_ARGS__)

#define DMEMIT(x...) (sz += ((sz >= maxlen) ? 0 : scnprintf(result + sz, maxlen - sz, x)))

#define DMEMIT_TARGET_NAME_VERSION(y) \
		DMEMIT("target_name=%s,target_version=%u.%u.%u", \
		       (y)->name, (y)->version[0], (y)->version[1], (y)->version[2])


#define module_dm(name) \
static int __init dm_##name##_init(void) \
{ \
	return dm_register_target(&(name##_target)); \
} \
module_init(dm_##name##_init) \
static void __exit dm_##name##_exit(void) \
{ \
	dm_unregister_target(&(name##_target)); \
} \
module_exit(dm_##name##_exit)


#define DM_ENDIO_DONE		0
#define DM_ENDIO_INCOMPLETE	1
#define DM_ENDIO_REQUEUE	2
#define DM_ENDIO_DELAY_REQUEUE	3


#define DM_MAPIO_SUBMITTED	0
#define DM_MAPIO_REMAPPED	1
#define DM_MAPIO_REQUEUE	DM_ENDIO_REQUEUE
#define DM_MAPIO_DELAY_REQUEUE	DM_ENDIO_DELAY_REQUEUE
#define DM_MAPIO_KILL		4

#define dm_sector_div64(x, y)( \
{ \
	u64 _res; \
	(x) = div64_u64_rem(x, y, &_res); \
	_res; \
} \
)


#define dm_div_up(n, sz) (((n) + (sz) - 1) / (sz))

#define dm_sector_div_up(n, sz) ( \
{ \
	sector_t _r = ((n) + (sz) - 1); \
	sector_div(_r, (sz)); \
	_r; \
} \
)


#define dm_round_up(n, sz) (dm_div_up((n), (sz)) * (sz))


#define dm_target_offset(ti, sector) ((sector) - (ti)->begin)

static inline sector_t to_sector(unsigned long long n)
{
	return (n >> SECTOR_SHIFT);
}

static inline unsigned long to_bytes(sector_t n)
{
	return (n << SECTOR_SHIFT);
}

#endif	
