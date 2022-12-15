#!/bin/bash
PATH=$PATH:/opt/mxe/usr/bin

MXETARGET=x86_64-w64-mingw32.shared

/opt/mxe/usr/${MXETARGET}/qt6/bin/qmake -o Makefile.dockerwin "CONFIG+=dockerwin" && \
	make -f Makefile.dockerwin clean && \
	make -f Makefile.dockerwin && \
	dockerfiles/winqt/mxedeployqt --mxetarget=${MXETARGET} output/dockerwin && \
	bash zip_winrelease.sh docker
