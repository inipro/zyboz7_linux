#ifndef _MYGMEM_IOCTL_
#define _MYGMEM_IOCTL_

enum {
	DRM_MYGEM_CREATE_BO = 0,
	DRM_MYGEM_INFO_BO,
	DRM_MYGEM_MAP_BO,
};

#define DRM_MYGEM_BO_FLAGS_COHERENT	0x00000001
#define DRM_MYGEM_BO_FLAGS_CMA		0x00000002

struct drm_mygem_create_bo {
	uint64_t size;
	uint32_t handle;
	uint32_t flags;
};

struct drm_mygem_info_bo {
	uint32_t handle;
	uint64_t size;
	uint64_t paddr;
};

struct drm_mygem_map_bo {
	uint32_t handle;
	uint32_t pad;
	uint64_t offset;
};

#define DRM_IOCTL_MYGEM_CREATE_BO	DRM_IOWR(DRM_COMMAND_BASE + \
					DRM_MYGEM_CREATE_BO, \
					struct drm_mygem_create_bo)

#define DRM_IOCTL_MYGEM_INFO_BO		DRM_IOWR(DRM_COMMAND_BASE + \
					DRM_MYGEM_INFO_BO, \
					struct drm_mygem_info_bo)

#define DRM_IOCTL_MYGEM_MAP_BO		DRM_IOWR(DRM_COMMAND_BASE + \
					DRM_MYGEM_MAP_BO, \
					struct drm_mygem_map_bo)

#endif
