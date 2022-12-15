FROM ubuntu:22.04

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get install -y \
	build-essential \
	libgmp-dev \
	libmpfr-dev \
	libgl-dev \
	libzip-dev \
	python-is-python3 \
	python3 \
	mercurial \
	zip \
	qt6-base-dev \
	qmake6 \
	wget

# Requirements for AppImageBuilder
RUN apt update && apt install -y binutils coreutils \
		desktop-file-utils fakeroot fuse \
		libgdk-pixbuf2.0-dev patchelf python3-pip \
		python3-setuptools squashfs-tools strace util-linux zsync

RUN python3 -m pip install appimage-builder

RUN useradd -s /bin/bash -m -u 1000 al
RUN chown -R al /home/al

USER al
WORKDIR /home/al/project/
CMD ["bash", "dockerfiles/linuxqt/build_linux.sh"]
