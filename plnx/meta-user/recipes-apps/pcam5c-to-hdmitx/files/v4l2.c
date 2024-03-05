#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <poll.h>

#include <linux/videodev2.h>
#include "v4l2.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define PCLEAR(x) memset(x, 0, sizeof(*x))

struct buffer *buffers;
static unsigned int n_buffers;
static enum v4l2_memory memory_type;

void v4l2_queue_buffer(int fd, int index, int dmabuf_fd)
{
	struct v4l2_buffer buf;
	struct v4l2_plane mplanes[1];

	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf.memory = memory_type;
	buf.index = index;
	buf.m.planes = mplanes;
	buf.length = 1;

	CLEAR(mplanes);

	if (memory_type == V4L2_MEMORY_DMABUF)
		buf.m.planes[0].m.fd = dmabuf_fd;
	if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
		errno_print("VIDIOC_QBUF");
}

int v4l2_dequeue_buffer(int fd, struct v4l2_buffer *buf)
{	
	struct v4l2_plane mplanes[1];

	PCLEAR(buf);

	buf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	buf->memory = memory_type;
	buf->m.planes = mplanes;
	buf->length = 1;

	CLEAR(mplanes);

	if (-1 == xioctl(fd, VIDIOC_DQBUF, buf)) {
		switch (errno) {
		case EAGAIN:
			return 0;
		case EIO:
			/* Could ignore EIO, see spec. */
			/* fall through */
		default:
			errno_print("VIDIOC_DQBUF");
		}
	}

	assert(buf->index < n_buffers);
	return 1;
}

void v4l2_stop_capturing(int fd)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_print("VIDIOC_STREAMOFF");
}

void v4l2_start_capturing_dmabuf(int fd)
{
	enum v4l2_buf_type type;
	unsigned int i;

	/* One buffer held by DRM, the rest queued to video4linux */
	for (i = 1; i < n_buffers; ++i)
		v4l2_queue_buffer(fd, i, buffers[i].dmabuf_fd);

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_print("VIDIOC_STREAMON");
}

void v4l2_start_capturing_mmap(int fd)
{
	enum v4l2_buf_type type;
	unsigned int i;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;
		struct v4l2_plane mplanes[1];

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		buf.m.planes = mplanes;
		buf.length = 1;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_print("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_print("VIDIOC_STREAMON");
}

void v4l2_uninit_device(void)
{
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(buffers[i].start, buffers[i].length))
			errno_print("munmap");
	free(buffers);
}

void v4l2_init_dmabuf(int fd, int *dmabufs, int count)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	req.memory = V4L2_MEMORY_DMABUF;
	memory_type = req.memory;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "does not support dmabuf\n");
			exit(EXIT_FAILURE);
		} else {
			errno_print("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory\n");
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		struct v4l2_plane mplanes[1];

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory      = V4L2_MEMORY_DMABUF;
		buf.index       = n_buffers;
		buf.m.planes	= mplanes;
		buf.length	= 1;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_print("VIDIOC_QUERYBUF");
		buffers[n_buffers].index = buf.index;
		buffers[n_buffers].dmabuf_fd = dmabufs[n_buffers];
	}
}

void v4l2_init_mmap(int fd, int count)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	req.memory = V4L2_MEMORY_MMAP;
	memory_type = req.memory;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "does not support mmap\n");
			exit(EXIT_FAILURE);
		} else {
			errno_print("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory\n");
		exit(EXIT_FAILURE);
	}

	buffers = calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		struct v4l2_plane mplanes[1];

		CLEAR(buf);

		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		buf.memory      = V4L2_MEMORY_MMAP;
		buf.index       = n_buffers;
		buf.m.planes	= mplanes;
		buf.length	= 1;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_print("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.m.planes[0].length;
		buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
					buf.m.planes[0].length,
					PROT_READ | PROT_WRITE /* required */,
					MAP_SHARED /* recommended */,
					fd, buf.m.planes[0].m.mem_offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			errno_print("mmap");
	}
}

void v4l2_init(int fd, int width, int height, int pitch)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "not a V4L2 device\n");
			exit(EXIT_FAILURE);
		} else {
			errno_print("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE)) {
		fprintf(stderr, "not a video capture device\n");
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}

	CLEAR(cropcap);
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;

	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
				case EINVAL:
					/* Cropping not supported. */
					break;
				default:
					/* Errors ignored. */
					break;
			}
		}
	}

	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	fmt.fmt.pix_mp.width       = width;
	fmt.fmt.pix_mp.height      = height;
	fmt.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix_mp.field       = V4L2_FIELD_NONE;
	fmt.fmt.pix_mp.num_planes = 1;
	fmt.fmt.pix_mp.plane_fmt[0].bytesperline = pitch;
	fmt.fmt.pix_mp.plane_fmt[0].sizeimage = width * height * 3;

	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_print("VIDIOC_S_FMT");

	printf("v4l2 negotiated format: ");
	printf("size = %dx%d, ", fmt.fmt.pix_mp.width, fmt.fmt.pix_mp.height);
	printf("pitch = %d bytes\n", fmt.fmt.pix_mp.plane_fmt[0].bytesperline);

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix_mp.width * 3;
	if (fmt.fmt.pix_mp.plane_fmt[0].bytesperline < min)
		fmt.fmt.pix_mp.plane_fmt[0].bytesperline = min;
	min = fmt.fmt.pix_mp.plane_fmt[0].bytesperline * fmt.fmt.pix_mp.height;
	if (fmt.fmt.pix_mp.plane_fmt[0].sizeimage < min)
		fmt.fmt.pix_mp.plane_fmt[0].sizeimage = min;
}

int v4l2_open(const char *dev_name)
{
	struct stat st;
	int fd;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
				dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
				dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	return fd;
}
