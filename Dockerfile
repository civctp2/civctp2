################################################################################
# base system
################################################################################
ARG TIMESTAMP=20220531

FROM ubuntu:jammy-"$TIMESTAMP" as system
ARG TIMESTAMP # has to be redeclared for stage: https://docs.docker.com/build/building/variables/#scoping

ENV USERNAME diUser
RUN useradd -m $USERNAME && \
    echo "$USERNAME:$USERNAME" | chpasswd && \
    usermod --shell /bin/bash $USERNAME && \
    usermod -aG video,audio $USERNAME

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && \
    apt-get install -y --no-install-recommends ca-certificates # essential to avoid error for snapshot repos: "No system certificates available."

ENV SNAPSHOT_ID=${TIMESTAMP}T000000Z
RUN sed -i 's/^deb /deb [snapshot=yes] /' /etc/apt/sources.list # needed for snapshots in ubuntu:22.04 : https://documentation.ubuntu.com/server/how-to/software/snapshot-service/#prerequisites

RUN echo "APT::Snapshot $SNAPSHOT_ID;" > /etc/apt/apt.conf.d/50snapshot && \
    cat /etc/apt/apt.conf.d/50snapshot && \
    apt-get --version # apt >= 2.4.11 needed for snapshots in ubuntu:22.04 

################################################################################
# builder
################################################################################
FROM system as builder

ARG COMPILER_VERSION=11
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libtiff-dev libavcodec-dev libavformat-dev libswscale-dev \
    byacc gcc-${COMPILER_VERSION} g++-${COMPILER_VERSION} automake make libtool unzip flex git ca-certificates

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${COMPILER_VERSION} 10 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${COMPILER_VERSION} 10 && \
    update-alternatives --install /usr/bin/cc  cc  /usr/bin/gcc 30 && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30 && \
    update-alternatives --remove cpp /usr/bin/cpp && \
    update-alternatives --install /usr/bin/cpp cpp /usr/bin/g++ 30

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
    CFLAGS="$CFLAGS -Wno-misleading-indentation $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -ggdb -rdynamic ) -fuse-ld=gold" \
    CXXFLAGS="$CXXFLAGS -Wno-misleading-indentation -fpermissive $( [ "${BTYP##*debug*}" ] && echo -O3 || echo -ggdb -rdynamic ) -fuse-ld=gold" \
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

## ctp2CD/ copy done in install stage such that stages before are compatible with travis docker build, results in one additional layer in the final DI (incr. DI download size)
COPY ctp2CD/ /opt/ctp2/

## ctp2 install has to be after ctp2CD/ to overwrite with newer versions from civctp2
## deb-file has to be copied first, sadly this adds a layer (which is not necessary with COPY --from=builder): https://stackoverflow.com/questions/52211895/docker-build-avoid-adding-files-only-needed-at-build-time
COPY deb/ /deb/

## apt install installs local deb-file with its dependencies: https://unix.stackexchange.com/questions/159094/how-to-install-a-deb-file-by-dpkg-i-or-by-apt#159114
RUN apt-get update && apt-get install -y --no-install-recommends \
    gdb libstdc++-11-dev \
    /deb/ctp2-${BTYP}.deb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

USER $USERNAME

WORKDIR /opt/ctp2/ctp2_program/ctp/

CMD ["./ctp2"]
