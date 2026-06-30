#!/bin/bash

qmake6 -o Makefile.linux && \
	make -f Makefile.linux clean && \
	make -f Makefile.linux && \
	make -f Makefile.linux install

echo "Creating AppDir"

# Clean + recreate AppDir

rm -rf output/AppDir
mkdir -p output/AppDir/usr/bin

# Copy binary

cp output/linux/ARPCalc output/AppDir/usr/bin/

# Desktop file

cp dockerfiles/linuxqt/ARPCalc.desktop output/AppDir/

# Icon (fix path to standard location)

mkdir -p output/AppDir/usr/share/icons/hicolor/256x256/apps
cp res/icon_256.png output/AppDir/usr/share/icons/hicolor/256x256/apps/ARPCalc.png

# Download linuxdeploy tools if missing

cd output

if [ ! -f linuxdeploy-x86_64.AppImage ]
then
	wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
	chmod +x linuxdeploy-x86_64.AppImage
fi

if [ ! -f linuxdeploy-plugin-qt-x86_64.AppImage ]
then
	wget -q https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
	chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
fi

# Build AppImage

export APPIMAGE_EXTRACT_AND_RUN=1
export QMAKE=qmake6

./linuxdeploy-x86_64.AppImage \
	--appdir AppDir \
	--desktop-file AppDir/ARPCalc.desktop \
	--icon-file AppDir/usr/share/icons/hicolor/256x256/apps/ARPCalc.png \
	--plugin qt \
	--output appimage

cd ..

# Ensure publish dir exists

mkdir -p publish/linux

# Versioning

changeset=$(hg id -i)
if [[ ${changeset:0-1} == "+" ]]
then
	filename=arpcalc-linux-$(date +%Y%m%d)-${changeset}-${build}
else
	filename=arpcalc-linux-$(hg log -r . --template "{latesttag}-{latesttagdistance}-{node|short}")-$(date +%Y%m%d)-${build}
fi

# Move AppImage to publish

mv output/ARPCalc-*.AppImage publish/linux/${filename}.AppImage

# Make a tarball

mv output/AppDir arpcalc
tar -czf publish/linux/${filename}.tgz arpcalc
mv arpcalc output/AppDir

# Update redirect pages

echo "<html><head><meta http-equiv="refresh" content="0; URL=/linux/${filename}.AppImage"></head></html>" > publish/linux/latest.html
echo "<html><head><meta http-equiv="refresh" content="0; URL=/linux/${filename}.AppImage"></head></html>" > publish/linux/index.html

echo "Build complete: ${filename}.AppImage"

