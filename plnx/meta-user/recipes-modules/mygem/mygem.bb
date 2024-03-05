SUMMARY = "Recipe for build an external mygem Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = "file://Makefile \
           file://mygem_drv.h \
           file://mygem_ioctl.h \
           file://mygem_drv.c \
           file://mygem_bo.c \
          "

S = "${WORKDIR}"

do_install:append() {
    install -d ${D}/usr/include/mygem
    install -m 0755 ${S}/mygem_ioctl.h ${D}/usr/include/mygem
}

FILES:${PN}-dev += "/usr/include/mygem/mygem_ioctl.h"


# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
