#
# This file is the vadd-fw recipe.
#

SUMMARY = "Simple vadd-fw to use fpgamanager class"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit dfx_dtg_zynq_full

COMPATIBLE_MACHINE:zynq = ".*"

SRC_URI = "file://vadd_wrapper.xsa \
	   file://pl-custom.dtsi \
           "
RDEPENDS:${PN} += "mygem"

inherit bitfile_info
