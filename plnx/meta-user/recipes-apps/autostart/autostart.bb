DESCRIPTION = "Init script to start at boot"
SUMMARY = "Init script to start at boot"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://autostart.sh \
"

inherit update-rc.d

RDEPENDS:${PN} += " \
	pcam5c-to-hdmitx \
	"
INSANE_SKIP:${PN} += "installed-vs-shipped"

INITSCRIPT_NAME = "autostart.sh"
INITSCRIPT_PARAMS = "start 99 5 ."

do_install () {
	install -d ${D}${sysconfdir}/init.d/
	install -m 0755 ${WORKDIR}/autostart.sh ${D}${sysconfdir}/init.d/
}
