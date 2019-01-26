################################################################################
# builder
################################################################################
FROM ubuntu:16.04 as builder

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev gtk+-2.0-dev \
        byacc gcc-5 g++-5 automake libtool unzip flex

ENV USERNAME builder

RUN useradd -m $USERNAME \
    && echo "$USERNAME:$USERNAME" | chpasswd \
    && usermod --shell /bin/bash $USERNAME \
    && usermod -aG video,audio $USERNAME

ENV HOME /opt
RUN chown -R $USERNAME:$USERNAME /opt/
USER $USERNAME

# Unfortunately EVN variables expansion for the COPY command is not supported
# at the moment (see https://github.com/moby/moby/issues/35018),
# so the username needs to be hardcoded for now.
COPY --chown=builder:builder ./ /ctp2/

RUN cd /ctp2 \
    && ./autogen.sh \
    && CC=/usr/bin/gcc-5 CXX=/usr/bin/g++-5 \
        CFLAGS="$CFLAGS -w -fuse-ld=gold" CXXFLAGS="$CXXFLAGS -fpermissive -w -fuse-ld=gold" \
        ./configure --prefix=/opt/ctp2 \
        --bindir=/opt/ctp2/ctp2_program/ctp --enable-silent-rules \
    && make \
    && make install
