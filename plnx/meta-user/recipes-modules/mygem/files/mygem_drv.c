#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>

#include <drm/drm.h>
#include <drm/drm_drv.h>

#include "mygem_drv.h"
#include "mygem_ioctl.h"

#define MYGEM_DRIVER_NAME	"mygem"
#define MYGEM_DRIVER_DESC	"GEM BO manager"
#define MYGEM_DRIVER_DATE	"20230923"
#define MYGEM_DRIVER_MAJOR	2023
#define MYGEM_DRIVER_MINOR	1
#define MYGEM_DRIVER_PATCHLEVEL	0

/* This should be the same as DRM_FILE_PAGE_OFFSET_START in drm_gem.c */
#define MYGEM_FILE_PAGE_OFFSET (DRM_FILE_PAGE_OFFSET_START)

#ifndef VM_RESERVED
#define VM_RESERVED (VM_DONTEXPAND | VM_DONTHUP)
#endif

static const struct vm_operations_struct reg_physical_vm_ops = {
#ifdef CONIG_HAVE_IOREMAP_PROT
	.access = generic_access_phys,
#endif
};


static int mygem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	/*
	unsigned long vsize;
	unsigned long res_start = 0x0;
	unsigned long res_len = 0x1000;
	int rc;
	 */
	/* A GEM buffer object has a fake mmap offset start from page offset
	 * DRM_FILE_PAGE_OFFSET_START. See drm_gem_init().
	 * MYGEM_FILE_PAGE_OFFSET should equal to DRM_FILE_PAGE_OFFSET_START.
	 * MYGEM_FILE_PAGE_OFFSET is 4GB for 64 bits system.
	 */
	DRM_INFO("%s:%s:%d: %zd\n", __FILE__, __func__, __LINE__, vma->vm_pgoff);
	if (likely(vma->vm_pgoff >= MYGEM_FILE_PAGE_OFFSET))
		return drm_gem_mmap(filp, vma);

	/*
	if (vma->vm_pgoff != 0)
		return -EINVAL;

	vsize = vma->vm_end - vma->vm_start;
	if (vsize > res_len)
		return -EINVAL;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_IO;
	vma->vm_flags |= VM_RESERVED;

	vma->vm_ops = &reg_physical_vm_ops;
	rc = io_remap_pfn_range(vma, vma->vm_start,
				res_start >> PAGE_SHIFT,
				vsize, vma->vm_page_prot);
	return rc;
	*/
	return -EINVAL;
}

static const struct file_operations mygem_fops = {
	.owner = THIS_MODULE,
	.open = drm_open,
	//.mmap = drm_gem_mmap,
	.mmap = mygem_mmap,
	.poll = drm_poll,
	.read = drm_read,
	.unlocked_ioctl = drm_ioctl,
	.release = drm_release,
};

static const struct drm_ioctl_desc mygem_ioctls[] = {
	DRM_IOCTL_DEF_DRV(MYGEM_CREATE_BO, mygem_create_bo_ioctl,
			DRM_AUTH|DRM_UNLOCKED|DRM_RENDER_ALLOW),
	DRM_IOCTL_DEF_DRV(MYGEM_INFO_BO, mygem_info_bo_ioctl,
			DRM_AUTH|DRM_UNLOCKED|DRM_RENDER_ALLOW),
	DRM_IOCTL_DEF_DRV(MYGEM_MAP_BO, mygem_map_bo_ioctl,
			DRM_AUTH|DRM_UNLOCKED|DRM_RENDER_ALLOW),
};

static struct  drm_driver mygem_driver = {
	.driver_features = DRIVER_GEM | DRIVER_RENDER,
	.prime_handle_to_fd = drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle = drm_gem_prime_fd_to_handle,
	.ioctls = mygem_ioctls,
	.num_ioctls = ARRAY_SIZE(mygem_ioctls),
	.fops = &mygem_fops,
	.name = MYGEM_DRIVER_NAME,
	.desc = MYGEM_DRIVER_DESC,
	.date = MYGEM_DRIVER_DATE,
	.major = MYGEM_DRIVER_MAJOR,
	.minor = MYGEM_DRIVER_MINOR,
	.patchlevel = MYGEM_DRIVER_PATCHLEVEL,
};

static int mygem_drm_platform_probe(struct platform_device *pdev)
{
	struct drm_mygem_dev *gdev;
	struct drm_device *dev;
	int ret;

	gdev = devm_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
	if (!gdev)
		return -ENOMEM;

	dev = drm_dev_alloc(&mygem_driver, &pdev->dev);
	if (IS_ERR(dev))
		return PTR_ERR(dev);

	ret = drm_dev_register(dev, 0);
	if (ret < 0)
		goto put;

	gdev->ddev = dev;
	dev->dev_private = gdev;
	platform_set_drvdata(pdev, gdev);

	return 0;

put:
	drm_dev_put(dev);
	return ret;
}

static int mygem_drm_platform_remove(struct platform_device *pdev)
{
	struct drm_mygem_dev *gdev = platform_get_drvdata(pdev);

	if (gdev->ddev) {
		drm_dev_unregister(gdev->ddev);
		drm_dev_put(gdev->ddev);
	}
	return 0;
}

static const struct of_device_id mygem_drm_of_match[] = {
	{ .compatible = "inipro,mygem", },
	{ },
};
MODULE_DEVICE_TABLE(of, mygem_drm_of_match);

static struct platform_driver mygem_drm_driver = {
	.probe = mygem_drm_platform_probe,
	.remove = mygem_drm_platform_remove,
	.driver = {
		.name = MYGEM_DRIVER_NAME,
		.of_match_table = mygem_drm_of_match,
	},
};

module_platform_driver(mygem_drm_driver);

MODULE_VERSION(__stringify(MYGEM_DRIVER_MAJOR) "."
	       __stringify(MYGEM_DRIVER_MINOR) "."
	       __stringify(MYGEM_DRIVER_PATCHLEVEL));
MODULE_DESCRIPTION(MYGEM_DRIVER_DESC);
MODULE_AUTHOR("Hyunok Kim <hokim@inipro.net>");
MODULE_LICENSE("GPL");
