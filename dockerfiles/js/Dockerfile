FROM emscripten/emsdk

RUN apt-get update && apt install -y lzip

RUN mkdir -p /opt/lib
WORKDIR /opt/lib
RUN wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz
RUN tar -xf gmp-6.2.1.tar.lz
WORKDIR /opt/lib/gmp-6.2.1
RUN emconfigure ./configure --disable-assembly --host none --enable-cxx --prefix=/opt/lib
RUN make
RUN make install

WORKDIR /opt/lib
RUN wget https://www.mpfr.org/mpfr-current/mpfr-4.1.0.tar.xz
RUN tar -xf mpfr-4.1.0.tar.xz
WORKDIR /opt/lib/mpfr-4.1.0
RUN emconfigure ./configure --host none --prefix=/opt/lib --with-gmp=/opt/lib
RUN make
RUN make install

RUN apt-get update && apt install -y nodejs rsync
RUN npm install terser -g

RUN apt-get install -y mercurial

WORKDIR /src
CMD ["bash", "./dockerfiles/js/dkr_build_js.sh"]
