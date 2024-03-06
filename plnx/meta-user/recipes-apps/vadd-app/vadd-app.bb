#
# This is the vadd-app application recipe
#
#

SUMMARY = "vadd-app application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS = "mygem libdrm"

SRC_URI = "\
	file://CMakeLists.txt \
	file://main.c \
	"

S = "${WORKDIR}"

RDEPENDS:${PN} += " \
		vadd-fw \
		"

inherit pkgconfig cmake
