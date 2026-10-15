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
elif [ "$1" == "dockerstatic" ]
then
	source_dir=output/dockerwinstatic
	build=dks
elif [ "$1" == "dockerfedora" ]
then
	source_dir=output/dockerwinfedora
	build=dkf
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

changeset=$(hg id -i)
if [[ ${changeset:0-1} == "+" ]]
then
	filename=arpcalc-windows-$(date +%Y%m%d)-${changeset}-${build}
else
	filename=arpcalc-windows-$(hg log -r . --template "{latesttag}-{latesttagdistance}-{node|short}")-$(date +%Y%m%d)-${build}
fi

(cd $staging && zip -r ../publish/windows/${filename}.zip arpcalc -x "*.debug")
echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}.zip\"></head></html>" > publish/windows/latest.html
echo "<html><head><meta http-equiv=\"refresh\" content=\"0; URL=/windows/${filename}.zip\"></head></html>" > publish/windows/index.html
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
