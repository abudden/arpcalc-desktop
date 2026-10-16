#!/bin/bash

# Zip files are named arpcalc-windows-<variant>-<version>-<date>-<build>,
# where <build> is "nb" for builds done directly on Windows and "dk" for
# those done with docker.  latest.html points at the native build or, for
# docker, the static build.
if [ $# -eq 0 ] || [ "$1" == "native" ]
then
	source_dir=output/winrelease
	variant=
	build=nb
	update_latest=yes
elif [ "$1" == "docker" ]
then
	source_dir=output/dockerwin
	variant=shared-
	build=dk
	update_latest=no
elif [ "$1" == "dockerstatic" ]
then
	source_dir=output/dockerwinstatic
	variant=static-
	build=dk
	update_latest=yes
else
	echo "Unrecognised build type"
	exit 1
fi

# Stage in a folder of its own so that builds running at the same time
# don't zip each other's files
staging=$(mktemp -d staging.XXXXXX)
mv $source_dir $staging/arpcalc
if [ ! -e publish/windows ]
then
	mkdir -p publish/windows
fi

if [ "$variant" == "static-" ]
then
    printf '%s\r\n' \
        "If updating from the shared build (with lots of DLLs) to the (single file) static build," \
        "Delete all the old files (except settings.ini) as they're no longer required." \
        > "${staging}/arpcalc/README.txt"
fi

changeset=$(hg id -i)
if [[ ${changeset:0-1} == "+" ]]
then
	filename=arpcalc-windows-${variant}$(date +%Y%m%d)-${changeset}-${build}
else
	filename=arpcalc-windows-${variant}$(hg log -r . --template "{latesttag}-{latesttagdistance}-{node|short}")-$(date +%Y%m%d)-${build}
fi

(cd $staging && zip -r ../publish/windows/${filename}.zip arpcalc -x "*.debug")
# Keep the debug information for this build, for resolving crash.log addresses
if [ -e $staging/arpcalc/ARPCalc.exe.debug ]
then
	mkdir -p symbols
	cp $staging/arpcalc/ARPCalc.exe.debug symbols/${filename}.exe.debug
fi
if [ $update_latest == yes ]
then
	echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}.zip\"></head></html>" > publish/windows/latest.html
	echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}.zip\"></head></html>" > publish/windows/index.html
fi
mv $staging/arpcalc $source_dir
rmdir $staging

echo "Zip file created as publish/windows/${filename}.zip"
echo "To publish on web, run:"
echo "$ cd publish"
echo "$ git pull origin master"
echo "$ git add windows"
echo '$ git commit -am "Added latest windows executable"'
echo "$ git push origin master"
echo "Also consider removing old versions..."
