#!/bin/bash

if [ $# -eq 0 ]
then
	source_dir=output/winrelease
	build=nb
elif [ "$1" == "native" ]
then
	source_dir=output/winrelease
	build=nb
elif [ "$1" == "docker" ]
then
	source_dir=output/dockerwin
	build=dk
else
	echo "Unrecognised build type"
	exit 1
fi


mv $source_dir arpcalc
if [ ! -e publish/windows ]
then
	mkdir -p publish/windows
fi

changeset=$(hg id -i)
if [[ ${changeset:0-1} == "+" ]]
then
	filename=arpcalc-windows-$(date +%Y%m%d)-${changeset}-${build}
else
	filename=arpcalc-windows-$(hg log -r . --template "{latesttag}-{latesttagdistance}-{node|short}")-$(date +%Y%m%d)-${build}
fi

zip -r publish/windows/${filename} arpcalc
echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}\"></head></html>" > publish/windows/latest.html
echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}\"></head></html>" > publish/windows/index.html
mv arpcalc $source_dir

echo "Zip file created as publish/windows/${filename}"
echo "To publish on web, run:"
echo "$ cd publish"
echo "$ git pull origin master"
echo "$ git add windows"
echo '$ git commit -am "Added latest windows executable"'
echo "$ git push origin master"
echo "Also consider removing old versions..."
