# Windows build using Fedora's pre-built MinGW cross-compiler and Qt
# packages: much quicker to build than the MXE image (dockerfiles/winqt).
FROM fedora:43

RUN dnf -y install \
		mingw64-gcc-c++ \
		mingw64-qt6-qtbase \
		mingw64-gmp \
		mingw64-mpfr \
		mingw64-libzip \
		python3 \
		make \
		git \
		mercurial \
		zip \
	&& dnf clean all

RUN useradd -s /bin/bash -m -u 1000 al
USER al
WORKDIR /home/al/project/

CMD ["bash", "dockerfiles/winfedora/build_fedorawin.sh"]
