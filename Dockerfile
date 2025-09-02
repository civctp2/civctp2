################################################################################
# base system
################################################################################
FROM debian as system

ENV USERNAME diUser
RUN useradd -m $USERNAME && \
    echo "$USERNAME:$USERNAME" | chpasswd && \
    usermod --shell /bin/bash $USERNAME && \
    usermod -aG video,audio $USERNAME


################################################################################
# builder
################################################################################
FROM system as builder

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y --no-install-recommends \
    libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libtiff-dev libavcodec-dev libavformat-dev libswscale-dev \
    byacc gcc g++ automake make libtool unzip flex git ca-certificates

### set default compilers
RUN cc --version && \
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
    && ./autogen.sh && \
    CFLAGS="$CFLAGS -Wno-misleading-indentation $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -g -rdynamic ) -fuse-ld=gold" \
    CXXFLAGS="$CXXFLAGS -Wno-misleading-indentation -fpermissive $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -g -rdynamic ) -fuse-ld=gold" \
    ./configure --prefix=/opt/ctp2 --bindir=/opt/ctp2/ctp2_program/ctp --enable-silent-rules --enable-precisetraderoutecalc $( [ "${BTYP##*debug*}" ] || echo --enable-debug ) \
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

## ctp2CD/ copy done in install stage such that stages before are compatible with travis docker build, results in one additional layer in the final DI (incr. DI download size)
COPY ctp2CD/ /opt/ctp2/

## ctp2 install has to be after ctp2CD/ to overwrite with newer versions from civctp2
## deb-file has to be copied first, sadly this adds a layer (which is not necessary with COPY --from=builder): https://stackoverflow.com/questions/52211895/docker-build-avoid-adding-files-only-needed-at-build-time
COPY deb/ /deb/

## apt install installs local deb-file with its dependencies: https://unix.stackexchange.com/questions/159094/how-to-install-a-deb-file-by-dpkg-i-or-by-apt#159114
RUN apt-get update && apt install -y --no-install-recommends \
    /deb/ctp2-${BTYP}.deb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

USER $USERNAME

WORKDIR /opt/ctp2/ctp2_program/ctp/

CMD ["./ctp2"]
