#!/bin/bash

qmake6 -o Makefile.linux && \
	make -f Makefile.linux clean && \
	make -f Makefile.linux && \
	make -f Makefile.linux install

echo "Creating AppDir"
echo "In case of issues, might need to connect to docker manually and run"
echo "$ cd output"
echo "$ appimage-builder --generate"
echo "$ cp -f AppImageBuilder.yml ../dockerfiles/linuxqt/"
echo "Note that appimage-builder seems to ignore most of its arguments"

if [ "$?" == "0" ]
then
	if [ ! -e output/AppDir/usr/share/icons ]
	then
		mkdir -p output/AppDir/usr/share/icons
	fi

	cp -f dockerfiles/linuxqt/AppImageBuilder.yml output
	cp -f res/icon_256.png output/AppDir/usr/share/icons/ARPCalc.png
	cp -f dockerfiles/linuxqt/ARPCalc.desktop output/AppDir/

	pushd output
	appimage-builder || exit 5
	popd
else
	exit 3
fi

if [ ! -e publish/linux ]
then
	mkdir -p publish/linux
fi

changeset=$(hg id -i)
if [[ ${changeset:0-1} == "+" ]]
then
	filename=arpcalc-linux-$(date +%Y%m%d)-${changeset}-${build}
else
	filename=arpcalc-linux-$(hg log -r . --template "{latesttag}-{latesttagdistance}-{node|short}")-$(date +%Y%m%d)-${build}
fi

mv output/AppDir arpcalc
tar -czf publish/linux/${filename}.tgz arpcalc || exit 6
mv arpcalc output/AppDir

echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/linux/${filename}.tgz\"></head></html>" > publish/linux/latest.html
echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/linux/${filename}.tgz\"></head></html>" > publish/linux/index.html
