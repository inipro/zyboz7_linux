#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <xf86drm.h>
#include <mygem/mygem_ioctl.h>

#define USE_INTERRUPT_ONLY 0

#define UIO_DEV		"/dev/uio0"
#define UIO_MMAP_SZ	4096

#define DATA_SIZE 	1000000
#define IP_START 	0x1
#define IP_DONE 	0x2
#define IP_IDLE 	0x4
#define AP_CTRL 	(0x00>>2)
#define GIE		(0x04>>2)
#define IER		(0x08>>2)
#define ISR		(0x0c>>2)
#define IN1		(0x10>>2)
#define IN2 		(0x1c>>2)
#define OUT_R 		(0x28>>2)
#define SIZE 		(0x34>>2)

int main()
{
	int ret = 0;
	int fd_gem, fd_uio;
	uint32_t addr_A, addr_B, addr_C;
	volatile int *bo_map_A, *bo_map_B, *bo_map_C;
	volatile uint32_t *map_ctrl;
	int *bufReference;
	uint32_t axi_ctrl;
	uint32_t done, idle;
	uint32_t unmask=1, dummy;
	int i;

	fd_gem = drmOpenWithType("mygem", NULL, DRM_NODE_RENDER);
	if (fd_gem < 0) {
		fprintf(stderr, "mygem open failed: %s(%d)\n", strerror(fd_gem), fd_gem);
		return -1;
	}

	struct drm_mygem_create_bo createInfo_A = {DATA_SIZE*sizeof(int), 0xffffffff, DRM_MYGEM_BO_FLAGS_COHERENT | DRM_MYGEM_BO_FLAGS_CMA};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_CREATE_BO, &createInfo_A);
	if (ret) {
		fprintf(stderr, "mygem create bo A failed: %s(%d)\n", strerror(ret), ret);
		goto close_fd_gem;
	}

	struct drm_mygem_info_bo infoInfo_A = {createInfo_A.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_INFO_BO, &infoInfo_A);
	if (ret) {
		fprintf(stderr, "mygem info bo A failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_A;
	}

	addr_A = infoInfo_A.paddr;

	struct drm_mygem_map_bo mapInfo_A = {createInfo_A.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_MAP_BO, &mapInfo_A);
	if (ret) {
		fprintf(stderr, "mygem map bo A failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_A;
	}

	bo_map_A = (int *)mmap(0, infoInfo_A.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_gem, mapInfo_A.offset);

	if (bo_map_A == MAP_FAILED) {
		fprintf(stderr, "mygem map A failed\n");
		ret = -1;
		goto close_bo_A;
	}

	struct drm_mygem_create_bo createInfo_B = {DATA_SIZE*sizeof(int), 0xffffffff, DRM_MYGEM_BO_FLAGS_COHERENT | DRM_MYGEM_BO_FLAGS_CMA};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_CREATE_BO, &createInfo_B);
	if (ret) {
		fprintf(stderr, "mygem create bo B failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_A;
	}

	struct drm_mygem_info_bo infoInfo_B = {createInfo_B.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_INFO_BO, &infoInfo_B);
	if (ret) {
		fprintf(stderr, "mygem info bo B failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_B;
	}

	addr_B = infoInfo_B.paddr;

	struct drm_mygem_map_bo mapInfo_B = {createInfo_B.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_MAP_BO, &mapInfo_B);
	if (ret) {
		fprintf(stderr, "mygem map bo B failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_B;
	}

	bo_map_B = (int *)mmap(0, infoInfo_B.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_gem, mapInfo_B.offset);

	if (bo_map_B == MAP_FAILED) {
		fprintf(stderr, "mygem map B failed\n");
		ret = -1;
		goto close_bo_B;
	}

	struct drm_mygem_create_bo createInfo_C = {DATA_SIZE*sizeof(int), 0xffffffff, DRM_MYGEM_BO_FLAGS_COHERENT | DRM_MYGEM_BO_FLAGS_CMA};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_CREATE_BO, &createInfo_C);
	if (ret) {
		fprintf(stderr, "mygem create bo C failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_B;
	}

	struct drm_mygem_info_bo infoInfo_C = {createInfo_C.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_INFO_BO, &infoInfo_C);
	if (ret) {
		fprintf(stderr, "mygem info bo C failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_C;
	}

	addr_C = infoInfo_C.paddr;

	struct drm_mygem_map_bo mapInfo_C = {createInfo_C.handle, 0, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_MYGEM_MAP_BO, &mapInfo_C);
	if (ret) {
		fprintf(stderr, "mygem map bo C failed: %s(%d)\n", strerror(ret), ret);
		goto close_bo_C;
	}

	bo_map_C = (int *)mmap(0, infoInfo_C.size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd_gem, mapInfo_C.offset);

	if (bo_map_C == MAP_FAILED) {
		fprintf(stderr, "mygem map C failed\n");
		ret = -1;
		goto close_bo_C;
	}

	fd_uio = open(UIO_DEV, O_RDWR);
	if (fd_uio < 0) {
		fprintf(stderr, "uio open failed: %s(%d)\n", strerror(fd_uio), fd_uio);
		ret = fd_uio;
		goto close_bo_C;
	}

	map_ctrl = (uint32_t *)mmap(NULL, UIO_MMAP_SZ, PROT_READ | PROT_WRITE, MAP_SHARED, fd_uio, 0);
	if (map_ctrl == MAP_FAILED) {
		fprintf(stderr, "uio map failed\n");
		ret = -1;
		goto close_fd_uio;
	}

	bufReference = (int *)malloc(DATA_SIZE*sizeof(int));
	for (i = 0; i < DATA_SIZE; ++i) {
		bo_map_A[i] = i;
		bo_map_B[i] = i;
		bo_map_C[i] = 0;
		bufReference[i] = bo_map_A[i] + bo_map_B[i];
	}

	map_ctrl[IN1] = addr_A;
	map_ctrl[IN2] = addr_B;
	map_ctrl[OUT_R] = addr_C;
	map_ctrl[SIZE] = DATA_SIZE;

	map_ctrl[GIE] = 0x01;
	map_ctrl[IER] = 0x01;
	axi_ctrl = map_ctrl[AP_CTRL] & 0x80;
	map_ctrl[AP_CTRL] = axi_ctrl | IP_START;

#if USE_INTERRUPT_ONLY
	write(fd_uio, &unmask, sizeof(unmask));
	read(fd_uio, &dummy, sizeof(dummy));
#else
	write(fd_uio, &unmask, sizeof(unmask));
	read(fd_uio, &dummy, sizeof(dummy));
	i = 0;
	do {
		axi_ctrl = map_ctrl[AP_CTRL];
		done = axi_ctrl & 0xfffffff2;
		idle = axi_ctrl & 0xfffffff4;
		i = i + 1;
		if (i > 1)
			printf("Read Loop iteration: %d Kernel Done: 0x%x Kernel Idle: 0x%x\n", i, done, idle);
	} while (done != IP_DONE);
#endif
	printf("INFO: IP Done\n");

	if (memcmp(bo_map_C, bufReference, DATA_SIZE*sizeof(int))) {
		printf("Value read back does not match reference\n");
	} else {
		printf("Value read back matches reference\n");
	}
	free(bufReference);

close_fd_uio:
	if (map_ctrl != MAP_FAILED) munmap((void *)map_ctrl, UIO_MMAP_SZ); 
	close(fd_uio);
close_bo_C:
	if (bo_map_C != MAP_FAILED) munmap((void *)bo_map_C, infoInfo_C.size); 
	struct drm_gem_close closeInfo_C = {createInfo_C.handle, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_GEM_CLOSE, &closeInfo_C);
	if (ret) {
		fprintf(stderr, "mygem bo C close filed: %s(%d)\n", strerror(ret), ret);
	}
close_bo_B:
	if (bo_map_B != MAP_FAILED) munmap((void *)bo_map_B, infoInfo_B.size); 
	struct drm_gem_close closeInfo_B = {createInfo_B.handle, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_GEM_CLOSE, &closeInfo_B);
	if (ret) {
		fprintf(stderr, "mygem bo B close filed: %s(%d)\n", strerror(ret), ret);
	}
close_bo_A:
	if (bo_map_A != MAP_FAILED) munmap((void *)bo_map_A, infoInfo_A.size); 
	struct drm_gem_close closeInfo_A = {createInfo_A.handle, 0};
	ret = ioctl(fd_gem, DRM_IOCTL_GEM_CLOSE, &closeInfo_A);
	if (ret) {
		fprintf(stderr, "mygem bo A close filed: %s(%d)\n", strerror(ret), ret);
	}
close_fd_gem:
	close(fd_gem);

	return ret;
}
