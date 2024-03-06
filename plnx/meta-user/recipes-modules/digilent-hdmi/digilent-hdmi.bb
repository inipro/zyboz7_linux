SUMMARY = "Recipe for build an external Digilent Hdmi DRM Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = "file://Makefile \
           file://digilent_hdmi.c \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
