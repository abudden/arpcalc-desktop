#!/bin/sh
qmake-qt6 "CONFIG+=console" && make
if [ "$?" == 0 ]
then
	if [ ! -e output/winconsole/libzip.dll ]
	then
		bash get_dlls.sh output/winconsole
	fi

	./output/winconsole/ARPCalc.exe
fi
