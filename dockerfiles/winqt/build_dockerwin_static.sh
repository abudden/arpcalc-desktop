#!/bin/bash
# Builds a single, statically-linked ARPCalc.exe against MXE's static Qt.
# This avoids loading ~30 DLLs at start-up (Qt, ICU, freetype, harfbuzz,
# glib, ...) and searching the Qt plugin folders.
PATH=$PATH:/opt/mxe/usr/bin

MXETARGET=x86_64-w64-mingw32.static

# Move any debug information into ARPCalc.exe.debug (which isn't included in
# the zip file): qmake's separate_debug_info doesn't work for static builds.
split_debug_info()
{
	local objcopy=${MXETARGET}-objcopy
	${objcopy} --only-keep-debug "$1" "$1.debug" && \
		${objcopy} --strip-debug "$1" && \
		${objcopy} --add-gnu-debuglink="$1.debug" "$1"
}

# Start from an empty output folder: make clean leaves the DLLs and plugins,
# and mxedeployqt doesn't replace existing ones, so a new ARPCalc.exe could
# be zipped with an older Qt.
rm -rf output/dockerwinstatic

/opt/mxe/usr/${MXETARGET}/qt6/bin/qmake -o Makefile.dockerwinstatic "CONFIG+=dockerwin" && \
	make -f Makefile.dockerwinstatic clean && \
	make -f Makefile.dockerwinstatic && \
	split_debug_info output/dockerwinstatic/ARPCalc.exe && \
	bash zip_winrelease.sh dockerstatic
