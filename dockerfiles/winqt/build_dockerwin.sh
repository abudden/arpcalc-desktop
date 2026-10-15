#!/bin/bash
PATH=$PATH:/opt/mxe/usr/bin

MXETARGET=x86_64-w64-mingw32.shared

# Only deploy the Qt plugins that ARPCalc uses: Qt reads every plugin in a
# plugin folder when it first needs one of that type (e.g. imageformats for
# the button icons), and each extra DLL is more for antivirus software to
# scan at start-up.  PNG support is built into Qt itself.
#   platforms: qwindows (the others are skipped)
#   styles: the native Windows style
#   tls: for downloading the currency data over https


# Start from an empty output folder: make clean leaves the DLLs and plugins,
# and mxedeployqt doesn't replace existing ones, so a new ARPCalc.exe could
# be zipped with an older Qt.
rm -rf output/dockerwin

/opt/mxe/usr/${MXETARGET}/qt6/bin/qmake -o Makefile.dockerwin "CONFIG+=dockerwin" && \
	make -f Makefile.dockerwin clean && \
	make -f Makefile.dockerwin && \
	dockerfiles/winqt/mxedeployqt --mxetarget=${MXETARGET} \
		--qtplugins="platforms;styles;tls" \
		--skiplibs="qminimal.dll;qoffscreen.dll;qdirect2d.dll" \
		output/dockerwin && \
	bash zip_winrelease.sh docker
