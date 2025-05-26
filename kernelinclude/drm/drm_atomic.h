

#ifndef DRM_ATOMIC_H_
#define DRM_ATOMIC_H_

#include <drm/drm_crtc.h>
#include <drm/drm_util.h>


struct drm_crtc_commit {
	
	struct drm_crtc *crtc;

	
	struct kref ref;

	
	struct completion flip_done;

	
	struct completion hw_done;

	
	struct completion cleanup_done;

	
	struct list_head commit_entry;

	
	struct drm_pending_vblank_event *event;

	
	bool abort_completion;
};

struct __drm_planes_state {
	struct drm_plane *ptr;
	struct drm_plane_state *state, *old_state, *new_state;
};

struct __drm_crtcs_state {
	struct drm_crtc *ptr;
	struct drm_crtc_state *state, *old_state, *new_state;

	
	struct drm_crtc_commit *commit;

	s32 __user *out_fence_ptr;
	u64 last_vblank_count;
};

struct __drm_connnectors_state {
	struct drm_connector *ptr;
	struct drm_connector_state *state, *old_state, *new_state;
	
	s32 __user *out_fence_ptr;
};

struct drm_private_obj;
struct drm_private_state;


struct drm_private_state_funcs {
	
	struct drm_private_state *(*atomic_duplicate_state)(struct drm_private_obj *obj);

	
	void (*atomic_destroy_state)(struct drm_private_obj *obj,
				     struct drm_private_state *state);

	
	void (*atomic_print_state)(struct drm_printer *p,
				   const struct drm_private_state *state);
};


struct drm_private_obj {
	
	struct list_head head;

	
	struct drm_modeset_lock lock;

	
	struct drm_private_state *state;

	
	const struct drm_private_state_funcs *funcs;
};


#define drm_for_each_privobj(privobj, dev) \
	list_for_each_entry(privobj, &(dev)->mode_config.privobj_list, head)


struct drm_private_state {
	
	struct drm_atomic_state *state;

	
	struct drm_private_obj *obj;
};

struct __drm_private_objs_state {
	struct drm_private_obj *ptr;
	struct drm_private_state *state, *old_state, *new_state;
};


struct drm_atomic_state {
	struct kref ref;

	struct drm_device *dev;

	
	bool allow_modeset : 1;
	
	bool legacy_cursor_update : 1;
	bool async_update : 1;
	
	bool duplicated : 1;
	struct __drm_planes_state *planes;
	struct __drm_crtcs_state *crtcs;
	int num_connector;
	struct __drm_connnectors_state *connectors;
	int num_private_objs;
	struct __drm_private_objs_state *private_objs;

	struct drm_modeset_acquire_ctx *acquire_ctx;

	
	struct drm_crtc_commit *fake_commit;

	
	struct work_struct commit_work;
};

void __drm_crtc_commit_free(struct kref *kref);


static inline struct drm_crtc_commit *drm_crtc_commit_get(struct drm_crtc_commit *commit)
{
	kref_get(&commit->ref);
	return commit;
}


static inline void drm_crtc_commit_put(struct drm_crtc_commit *commit)
{
	kref_put(&commit->ref, __drm_crtc_commit_free);
}

int drm_crtc_commit_wait(struct drm_crtc_commit *commit);

struct drm_atomic_state * __must_check
drm_atomic_state_alloc(struct drm_device *dev);
void drm_atomic_state_clear(struct drm_atomic_state *state);


static inline struct drm_atomic_state *
drm_atomic_state_get(struct drm_atomic_state *state)
{
	kref_get(&state->ref);
	return state;
}

void __drm_atomic_state_free(struct kref *ref);


static inline void drm_atomic_state_put(struct drm_atomic_state *state)
{
	kref_put(&state->ref, __drm_atomic_state_free);
}

int  __must_check
drm_atomic_state_init(struct drm_device *dev, struct drm_atomic_state *state);
void drm_atomic_state_default_clear(struct drm_atomic_state *state);
void drm_atomic_state_default_release(struct drm_atomic_state *state);

struct drm_crtc_state * __must_check
drm_atomic_get_crtc_state(struct drm_atomic_state *state,
			  struct drm_crtc *crtc);
struct drm_plane_state * __must_check
drm_atomic_get_plane_state(struct drm_atomic_state *state,
			   struct drm_plane *plane);
struct drm_connector_state * __must_check
drm_atomic_get_connector_state(struct drm_atomic_state *state,
			       struct drm_connector *connector);

void drm_atomic_private_obj_init(struct drm_device *dev,
				 struct drm_private_obj *obj,
				 struct drm_private_state *state,
				 const struct drm_private_state_funcs *funcs);
void drm_atomic_private_obj_fini(struct drm_private_obj *obj);

struct drm_private_state * __must_check
drm_atomic_get_private_obj_state(struct drm_atomic_state *state,
				 struct drm_private_obj *obj);
struct drm_private_state *
drm_atomic_get_old_private_obj_state(const struct drm_atomic_state *state,
				     struct drm_private_obj *obj);
struct drm_private_state *
drm_atomic_get_new_private_obj_state(const struct drm_atomic_state *state,
				     struct drm_private_obj *obj);

struct drm_connector *
drm_atomic_get_old_connector_for_encoder(const struct drm_atomic_state *state,
					 struct drm_encoder *encoder);
struct drm_connector *
drm_atomic_get_new_connector_for_encoder(const struct drm_atomic_state *state,
					 struct drm_encoder *encoder);

struct drm_crtc *
drm_atomic_get_old_crtc_for_encoder(struct drm_atomic_state *state,
					 struct drm_encoder *encoder);
struct drm_crtc *
drm_atomic_get_new_crtc_for_encoder(struct drm_atomic_state *state,
					 struct drm_encoder *encoder);


static inline struct drm_crtc_state *
drm_atomic_get_existing_crtc_state(const struct drm_atomic_state *state,
				   struct drm_crtc *crtc)
{
	return state->crtcs[drm_crtc_index(crtc)].state;
}


static inline struct drm_crtc_state *
drm_atomic_get_old_crtc_state(const struct drm_atomic_state *state,
			      struct drm_crtc *crtc)
{
	return state->crtcs[drm_crtc_index(crtc)].old_state;
}

static inline struct drm_crtc_state *
drm_atomic_get_new_crtc_state(const struct drm_atomic_state *state,
			      struct drm_crtc *crtc)
{
	return state->crtcs[drm_crtc_index(crtc)].new_state;
}


static inline struct drm_plane_state *
drm_atomic_get_existing_plane_state(const struct drm_atomic_state *state,
				    struct drm_plane *plane)
{
	return state->planes[drm_plane_index(plane)].state;
}


static inline struct drm_plane_state *
drm_atomic_get_old_plane_state(const struct drm_atomic_state *state,
			       struct drm_plane *plane)
{
	return state->planes[drm_plane_index(plane)].old_state;
}


static inline struct drm_plane_state *
drm_atomic_get_new_plane_state(const struct drm_atomic_state *state,
			       struct drm_plane *plane)
{
	return state->planes[drm_plane_index(plane)].new_state;
}


static inline struct drm_connector_state *
drm_atomic_get_existing_connector_state(const struct drm_atomic_state *state,
					struct drm_connector *connector)
{
	int index = drm_connector_index(connector);

	if (index >= state->num_connector)
		return NULL;

	return state->connectors[index].state;
}


static inline struct drm_connector_state *
drm_atomic_get_old_connector_state(const struct drm_atomic_state *state,
				   struct drm_connector *connector)
{
	int index = drm_connector_index(connector);

	if (index >= state->num_connector)
		return NULL;

	return state->connectors[index].old_state;
}


static inline struct drm_connector_state *
drm_atomic_get_new_connector_state(const struct drm_atomic_state *state,
				   struct drm_connector *connector)
{
	int index = drm_connector_index(connector);

	if (index >= state->num_connector)
		return NULL;

	return state->connectors[index].new_state;
}


static inline const struct drm_plane_state *
__drm_atomic_get_current_plane_state(const struct drm_atomic_state *state,
				     struct drm_plane *plane)
{
	if (state->planes[drm_plane_index(plane)].state)
		return state->planes[drm_plane_index(plane)].state;

	return plane->state;
}

int __must_check
drm_atomic_add_encoder_bridges(struct drm_atomic_state *state,
			       struct drm_encoder *encoder);
int __must_check
drm_atomic_add_affected_connectors(struct drm_atomic_state *state,
				   struct drm_crtc *crtc);
int __must_check
drm_atomic_add_affected_planes(struct drm_atomic_state *state,
			       struct drm_crtc *crtc);

int __must_check drm_atomic_check_only(struct drm_atomic_state *state);
int __must_check drm_atomic_commit(struct drm_atomic_state *state);
int __must_check drm_atomic_nonblocking_commit(struct drm_atomic_state *state);

void drm_state_dump(struct drm_device *dev, struct drm_printer *p);


#define for_each_oldnew_connector_in_state(__state, connector, old_connector_state, new_connector_state, __i) \
	for ((__i) = 0;								\
	     (__i) < (__state)->num_connector;					\
	     (__i)++)								\
		for_each_if ((__state)->connectors[__i].ptr &&			\
			     ((connector) = (__state)->connectors[__i].ptr,	\
			     (void)(connector) , \
			     (old_connector_state) = (__state)->connectors[__i].old_state,	\
			     (new_connector_state) = (__state)->connectors[__i].new_state, 1))


#define for_each_old_connector_in_state(__state, connector, old_connector_state, __i) \
	for ((__i) = 0;								\
	     (__i) < (__state)->num_connector;					\
	     (__i)++)								\
		for_each_if ((__state)->connectors[__i].ptr &&			\
			     ((connector) = (__state)->connectors[__i].ptr,	\
			     (void)(connector) , \
			     (old_connector_state) = (__state)->connectors[__i].old_state, 1))


#define for_each_new_connector_in_state(__state, connector, new_connector_state, __i) \
	for ((__i) = 0;								\
	     (__i) < (__state)->num_connector;					\
	     (__i)++)								\
		for_each_if ((__state)->connectors[__i].ptr &&			\
			     ((connector) = (__state)->connectors[__i].ptr,	\
			     (void)(connector) , \
			     (new_connector_state) = (__state)->connectors[__i].new_state, \
			     (void)(new_connector_state) , 1))


#define for_each_oldnew_crtc_in_state(__state, crtc, old_crtc_state, new_crtc_state, __i) \
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_crtc;		\
	     (__i)++)							\
		for_each_if ((__state)->crtcs[__i].ptr &&		\
			     ((crtc) = (__state)->crtcs[__i].ptr,	\
			      (void)(crtc) , \
			     (old_crtc_state) = (__state)->crtcs[__i].old_state, \
			     (void)(old_crtc_state) , \
			     (new_crtc_state) = (__state)->crtcs[__i].new_state, \
			     (void)(new_crtc_state) , 1))


#define for_each_old_crtc_in_state(__state, crtc, old_crtc_state, __i)	\
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_crtc;		\
	     (__i)++)							\
		for_each_if ((__state)->crtcs[__i].ptr &&		\
			     ((crtc) = (__state)->crtcs[__i].ptr,	\
			     (void)(crtc) , \
			     (old_crtc_state) = (__state)->crtcs[__i].old_state, 1))


#define for_each_new_crtc_in_state(__state, crtc, new_crtc_state, __i)	\
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_crtc;		\
	     (__i)++)							\
		for_each_if ((__state)->crtcs[__i].ptr &&		\
			     ((crtc) = (__state)->crtcs[__i].ptr,	\
			     (void)(crtc) , \
			     (new_crtc_state) = (__state)->crtcs[__i].new_state, \
			     (void)(new_crtc_state) , 1))


#define for_each_oldnew_plane_in_state(__state, plane, old_plane_state, new_plane_state, __i) \
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_total_plane;	\
	     (__i)++)							\
		for_each_if ((__state)->planes[__i].ptr &&		\
			     ((plane) = (__state)->planes[__i].ptr,	\
			      (void)(plane) , \
			      (old_plane_state) = (__state)->planes[__i].old_state,\
			      (new_plane_state) = (__state)->planes[__i].new_state, 1))


#define for_each_oldnew_plane_in_state_reverse(__state, plane, old_plane_state, new_plane_state, __i) \
	for ((__i) = ((__state)->dev->mode_config.num_total_plane - 1);	\
	     (__i) >= 0;						\
	     (__i)--)							\
		for_each_if ((__state)->planes[__i].ptr &&		\
			     ((plane) = (__state)->planes[__i].ptr,	\
			      (old_plane_state) = (__state)->planes[__i].old_state,\
			      (new_plane_state) = (__state)->planes[__i].new_state, 1))


#define for_each_new_plane_in_state_reverse(__state, plane, new_plane_state, __i) \
	for ((__i) = ((__state)->dev->mode_config.num_total_plane - 1);	\
	     (__i) >= 0;						\
	     (__i)--)							\
		for_each_if ((__state)->planes[__i].ptr &&		\
			     ((plane) = (__state)->planes[__i].ptr,	\
			      (new_plane_state) = (__state)->planes[__i].new_state, 1))


#define for_each_old_plane_in_state(__state, plane, old_plane_state, __i) \
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_total_plane;	\
	     (__i)++)							\
		for_each_if ((__state)->planes[__i].ptr &&		\
			     ((plane) = (__state)->planes[__i].ptr,	\
			      (old_plane_state) = (__state)->planes[__i].old_state, 1))

#define for_each_new_plane_in_state(__state, plane, new_plane_state, __i) \
	for ((__i) = 0;							\
	     (__i) < (__state)->dev->mode_config.num_total_plane;	\
	     (__i)++)							\
		for_each_if ((__state)->planes[__i].ptr &&		\
			     ((plane) = (__state)->planes[__i].ptr,	\
			      (void)(plane) , \
			      (new_plane_state) = (__state)->planes[__i].new_state, \
			      (void)(new_plane_state) , 1))


#define for_each_oldnew_private_obj_in_state(__state, obj, old_obj_state, new_obj_state, __i) \
	for ((__i) = 0; \
	     (__i) < (__state)->num_private_objs && \
		     ((obj) = (__state)->private_objs[__i].ptr, \
		      (old_obj_state) = (__state)->private_objs[__i].old_state,	\
		      (new_obj_state) = (__state)->private_objs[__i].new_state, 1); \
	     (__i)++)


#define for_each_old_private_obj_in_state(__state, obj, old_obj_state, __i) \
	for ((__i) = 0; \
	     (__i) < (__state)->num_private_objs && \
		     ((obj) = (__state)->private_objs[__i].ptr, \
		      (old_obj_state) = (__state)->private_objs[__i].old_state, 1); \
	     (__i)++)


#define for_each_new_private_obj_in_state(__state, obj, new_obj_state, __i) \
	for ((__i) = 0; \
	     (__i) < (__state)->num_private_objs && \
		     ((obj) = (__state)->private_objs[__i].ptr, \
		      (void)(obj) , \
		      (new_obj_state) = (__state)->private_objs[__i].new_state, 1); \
	     (__i)++)


static inline bool
drm_atomic_crtc_needs_modeset(const struct drm_crtc_state *state)
{
	return state->mode_changed || state->active_changed ||
	       state->connectors_changed;
}


static inline bool
drm_atomic_crtc_effectively_active(const struct drm_crtc_state *state)
{
	return state->active || state->self_refresh_active;
}


struct drm_bus_cfg {
	
	u32 format;

	
	u32 flags;
};


struct drm_bridge_state {
	
	struct drm_private_state base;

	
	struct drm_bridge *bridge;

	
	struct drm_bus_cfg input_bus_cfg;

	
	struct drm_bus_cfg output_bus_cfg;
};

static inline struct drm_bridge_state *
drm_priv_to_bridge_state(struct drm_private_state *priv)
{
	return container_of(priv, struct drm_bridge_state, base);
}

struct drm_bridge_state *
drm_atomic_get_bridge_state(struct drm_atomic_state *state,
			    struct drm_bridge *bridge);
struct drm_bridge_state *
drm_atomic_get_old_bridge_state(const struct drm_atomic_state *state,
				struct drm_bridge *bridge);
struct drm_bridge_state *
drm_atomic_get_new_bridge_state(const struct drm_atomic_state *state,
				struct drm_bridge *bridge);

#endif 
