# A fixed version, so that rebuilding the image doesn't pick up changes (newer
# versions have needed em++ rather than emcc, and a writable cache)
FROM emscripten/emsdk:6.0.12

RUN apt-get update && apt install -y lzip

RUN mkdir -p /opt/lib
WORKDIR /opt/lib
COPY gmp-6.2.1.tar.lz /opt/lib/
RUN tar -xf gmp-6.2.1.tar.lz
WORKDIR /opt/lib/gmp-6.2.1
RUN emconfigure ./configure --disable-assembly --host none --enable-cxx --prefix=/opt/lib
RUN make
RUN make install

WORKDIR /opt/lib
COPY mpfr-4.1.0.tar.xz /opt/lib/
RUN tar -xf mpfr-4.1.0.tar.xz
WORKDIR /opt/lib/mpfr-4.1.0
RUN emconfigure ./configure --host none --prefix=/opt/lib --with-gmp=/opt/lib
RUN make
RUN make install

RUN apt-get update && apt install -y nodejs rsync
RUN npm install terser -g

RUN apt-get install -y mercurial

# The build runs as uid 1000 (see docker-compose.yml), which in recent emsdk
# images can't write to Emscripten's cache
RUN chmod -R a+rwX /emsdk/upstream/emscripten/cache

WORKDIR /src
CMD ["bash", "./dockerfiles/js/dkr_build_js.sh"]
