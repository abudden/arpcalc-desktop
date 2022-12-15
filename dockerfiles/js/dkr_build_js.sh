#!/bin/bash
mkdir -p output/js

echo "Compiling to wasm"
emcc --bind -O3 --std=c++20 -x c++ \
	-o output/js/calclib.js \
	-Iinc \
	-I/opt/lib/include \
	/opt/lib/lib/libmpfr.a \
	/opt/lib/lib/libgmp.a \
	js/jsinterface.cpp \
	src/arpfloat.cpp \
	src/commands.cpp \
	src/conversion.cpp \
	src/grids.cpp \
	src/keys.cpp \
	src/ops.cpp \
	src/si.cpp \
	src/stack.cpp \
	src/strutils.cpp

if [ "$?" == "0" ]
then
	for f in webix jquery images zip
	do
		if [ ! -d publish/$f ]
		then
			mkdir -p publish/$f
		fi
		echo "rsyncing $f"
		rsync -arp --delete web/$f/ publish/$f/
	done

	for f in web/*.js
	do
		echo "Compressing and mangling $f"
		terser --compress --mangle --output publish/$(basename $f) -- $f || exit 4
	done

	echo "Rsyncing compiled code"
	rsync -arp output/js/ publish/

	echo "Adding favicon"
	cp -f res/icon.ico publish/favicon.ico

	echo "Adding index.html"
	sed 's@../output/js@.@' web/arpcalc.html > publish/index.html
else
	exit 3
fi

echo "Build complete.  To publish on the web, run:"
echo "$ cd publish"
echo "$ git commit -am \"Updated to hg revision \"(hg id -i)\"\""
echo "$ git push origin master"
echo "(to push to git@github.com:abudden/arpcalcpublish.git)"
