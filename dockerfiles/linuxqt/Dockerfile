FROM ubuntu:22.04

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get install -y \
		binutils \
		build-essential \
		coreutils \
		desktop-file-utils \
		file \
		fuse \
		libgdk-pixbuf2.0-dev \
		libgl-dev \
		libgmp-dev \
		libmpfr-dev \
		libzip-dev \
		mercurial \
		patchelf \
		python-is-python3 \
		python3 \
		qmake6 \
		qt6-base-dev \
		squashfs-tools \
		ubuntu-keyring \
		util-linux \
		wget \
		zip

RUN useradd -s /bin/bash -m -u 1000 al
RUN chown -R al /home/al

USER al
WORKDIR /home/al/project/
CMD ["bash", "dockerfiles/linuxqt/build_linux.sh"]
