################################################################################
# builder
################################################################################
FROM ubuntu:14.04 as builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev gcc-5 g++-5

COPY ctp2/ /ctp2/

RUN cd ctp2 && \
    ./autogen.sh && \
    CFLAGS="$CFLAGS -w -fuse-ld=gold" \
    CXXFLAGS="$CXXFLAGS -w -fuse-ld=gold" \
    ./configure --enable-silent-rules && \
    make -j"$(nproc)" && \
    make -j"$(nproc)" install
