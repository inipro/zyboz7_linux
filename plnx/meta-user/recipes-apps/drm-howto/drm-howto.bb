#
# This is the drm-howto application recipe
#
#

SUMMARY = "drm-howto application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "libdrm"

SRC_URI = "\
	file://CMakeLists.txt \
	file://modeset-simple.c \
	file://modeset.c \
	file://modeset-double-buffered.c \
	file://modeset-vsync.c \
	file://modeset-atomic.c \
	"

S = "${WORKDIR}"

inherit pkgconfig cmake
