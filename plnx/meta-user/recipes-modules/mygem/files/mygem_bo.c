#include "mygem_drv.h"
#include "mygem_ioctl.h"

void mygem_describe(const struct drm_mygem_bo *obj)
{
	size_t size_in_kb = obj->base.base.size / 1024;
	size_t physical_addr = obj->base.dma_addr;

	DRM_INFO("%p: H[0x%zxKB] D[0x%zx]\n",
			obj, size_in_kb, physical_addr);
}

static struct drm_mygem_bo *mygem_create_bo(struct drm_device *dev,
		uint64_t unaligned_size)
{
	size_t size = PAGE_ALIGN(unaligned_size);
	struct drm_gem_dma_object *dma_obj;

	DRM_DEBUG("%s:%s:%d: %zd\n", __FILE__, __func__, __LINE__, size);

	if (!size)
		return ERR_PTR(-EINVAL);

	dma_obj = drm_gem_dma_create(dev, size);
	if (IS_ERR(dma_obj))
		return ERR_PTR(-ENOMEM);

	return to_mygem_bo(&dma_obj->base);
}

int mygem_create_bo_ioctl(struct drm_device *dev,
			void *data,
			struct drm_file *filp)
{
	int ret;
	struct drm_mygem_create_bo *args = data;
	struct drm_mygem_bo *bo;

	if (((args->flags & DRM_MYGEM_BO_FLAGS_COHERENT) == 0) ||
	    ((args->flags & DRM_MYGEM_BO_FLAGS_CMA) == 0))
		return -EINVAL;

	bo = mygem_create_bo(dev, args->size);
	DRM_DEBUG("%s:%s:%d: %p\n", __FILE__, __func__, __LINE__, bo);
	if (IS_ERR(bo)) {
		DRM_ERROR("object creation failed\n");
		return PTR_ERR(bo);
	}
	bo->flags |= DRM_MYGEM_BO_FLAGS_COHERENT;
	bo->flags |= DRM_MYGEM_BO_FLAGS_CMA;

	ret = drm_gem_handle_create(filp, &bo->base.base, &args->handle);
	if (ret) {
		drm_gem_dma_free(&bo->base);
		DRM_ERROR("handle creation failed\n");
		return ret;
	}

	mygem_describe(bo);
	drm_gem_object_put(&bo->base.base);

	return ret;
}

int mygem_info_bo_ioctl(struct drm_device *dev,
			void *data,
			struct drm_file *filp)
{
	const struct drm_mygem_bo *bo;
	struct drm_mygem_info_bo *args = data;
	struct drm_gem_object *gem_obj = drm_gem_object_lookup(filp,
							args->handle);
	DRM_DEBUG("%s:%s:%d: %p\n", __FILE__, __func__, __LINE__, data);

	if (!gem_obj) {
		DRM_ERROR("Failed to look up GEM BO %d\n", args->handle);
		return -EINVAL;
	}

	bo = to_mygem_bo(gem_obj);

	args->size = bo->base.base.size;
	args->paddr = bo->base.dma_addr;

	drm_gem_object_put(gem_obj);

	return 0;
}

int mygem_map_bo_ioctl(struct drm_device *dev,
			void *data,
			struct drm_file *filp)
{
	struct drm_mygem_map_bo *args = data;
	struct drm_gem_object *gem_obj;

	DRM_DEBUG("%s:%s:%d: %p\n", __FILE__, __func__, __LINE__, data);
	gem_obj = drm_gem_object_lookup(filp, args->handle);
	if (!gem_obj) {
		DRM_ERROR("Failed to look up GEM BO %d\n", args->handle);
		return -EINVAL;
	}

	/* The mmap offset was set up at BO allocation time. */
	args->offset = drm_vma_node_offset_addr(&gem_obj->vma_node);
	drm_gem_object_put(gem_obj);

	return 0;
}
