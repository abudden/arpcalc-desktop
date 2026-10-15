#!/bin/bash
# Builds the Windows version with Fedora's MinGW packages (see Dockerfile)

TARGET=x86_64-w64-mingw32

# qmake disables pkg-config when cross-compiling unless a sysroot is set
export PKG_CONFIG_SYSROOT_DIR=/
export PKG_CONFIG_LIBDIR=/usr/${TARGET}/sys-root/mingw/lib/pkgconfig

# See dockerfiles/winqt/build_dockerwin.sh for the choice of plugins:
# mxedeployqt works with Fedora's MinGW tree too.

# Start from an empty output folder: make clean leaves the DLLs and plugins,
# and mxedeployqt doesn't replace existing ones, so a new ARPCalc.exe could
# be zipped with an older Qt.
rm -rf output/dockerwinfedora

${TARGET}-qmake-qt6 -o Makefile.dockerwinfedora "CONFIG+=dockerwin" \
		"BUILDNAME=dockerwinfedora" "PKG_CONFIG=${TARGET}-pkg-config" && \
	make -f Makefile.dockerwinfedora clean && \
	make -f Makefile.dockerwinfedora && \
	dockerfiles/winqt/mxedeployqt --mxepath=/usr/${TARGET} --mxetarget=${TARGET} \
		--qtplugins="platforms;styles;tls" \
		--skiplibs="qminimal.dll;qoffscreen.dll;qdirect2d.dll" \
		output/dockerwinfedora && \
	bash zip_winrelease.sh dockerfedora
