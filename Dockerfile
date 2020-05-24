################################################################################
# base system
################################################################################
FROM ubuntu:18.04 as system

ENV USERNAME diUser
RUN useradd -m $USERNAME && \
    echo "$USERNAME:$USERNAME" | chpasswd && \
    usermod --shell /bin/bash $USERNAME && \
    usermod -aG video,audio $USERNAME


################################################################################
# builder
################################################################################
FROM system as builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libtiff-dev libavcodec-dev libavformat-dev libswscale-dev \
    byacc gcc-5 g++-5 automake libtool unzip flex git ca-certificates

### set default compilers
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 100 && \
    update-alternatives --install /usr/bin/cpp cpp-bin /usr/bin/cpp-5 30 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30 && \
    update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30 && \
    cc --version && \
    c++ --version && \
    cpp --version

ENV LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:/usr/local/lib"

## ctp2CD/ copy not done in builder stage such that stages before are compatible with travis docker build
## not using `COPY  ./ /ctp2/` to avoid cache out-dating when ctp2CD/ is populated for 3rd stage
COPY autogen.sh configure.ac GNUmakefile.am   /ctp2/
COPY ctp2_data/  /ctp2/ctp2_data/
## done after copying ctp2_data/ because ctp2_code/ more likely to change
COPY ctp2_code/  /ctp2/ctp2_code/

ARG BTYP

RUN cd /ctp2 \
    && ./autogen.sh \
    && CC=/usr/bin/gcc-5 \
    CXX=/usr/bin/g++-5 \
    CFLAGS="$CFLAGS -w $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -g -rdynamic ) -fuse-ld=gold" \
    CXXFLAGS="$CXXFLAGS -fpermissive -w $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -g -rdynamic ) -fuse-ld=gold" \
    ./configure --prefix=/opt/ctp2 --bindir=/opt/ctp2/ctp2_program/ctp --enable-silent-rules $( [ "${BTYP##*debug*}" ] || echo --enable-debug ) \
    && make -j"$(nproc)" \
    && make -j"$(nproc)" install \
    && cp -r /ctp2/ctp2_data/ /opt/ctp2/ \
    && mkdir -p /opt/ctp2/ctp2_program/ctp/dll/map/ \
    && cp -v /ctp2/ctp2_code/mapgen/.libs/*.so /opt/ctp2/ctp2_program/ctp/dll/map/


################################################################################
# merge
################################################################################
FROM system as install

ARG BTYP

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl2-2.0 libsdl2-mixer-2.0 libsdl2-image-2.0 libavcodec57 libavformat57 libswscale4 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

## ctp2CD/ copy done in install stage such that stages before are compatible with travis docker build, results in one additional layer in the final DI (incr. DI download size)
COPY ctp2CD/ /opt/ctp2/
## ctp2/ copy has to be after ctp2CD/ to overwrite with newer versions from civctp2
COPY --from=builder /opt/ctp2/ /opt/ctp2/

USER $USERNAME

WORKDIR /opt/ctp2/ctp2_program/ctp/

CMD ["./ctp2"]
