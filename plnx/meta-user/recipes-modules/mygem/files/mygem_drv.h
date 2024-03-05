#ifndef _MYGEM_DRV_H_
#define _MYGEM_DRV_H_

#include <drm/drm_device.h>
#include <drm/drm_gem_dma_helper.h>

struct drm_mygem_bo {
	struct drm_gem_dma_object base;
	uint32_t flags;
};

struct drm_mygem_dev {
	struct drm_device *ddev;
};

static inline struct drm_mygem_bo *to_mygem_bo(struct drm_gem_object *bo)
{
	return (struct drm_mygem_bo *) bo;
}

int mygem_create_bo_ioctl(struct drm_device *dev, void *data,
			struct drm_file *filp);
int mygem_info_bo_ioctl(struct drm_device *dev, void *data,
			struct drm_file *filp);
int mygem_map_bo_ioctl(struct drm_device *dev, void *data,
			struct drm_file *filp);
void mygem_describe(const struct drm_mygem_bo *obj);

#endif
