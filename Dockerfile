################################################################################
# builder
################################################################################
FROM ubuntu:16.04 as builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev gcc-5 g++-5 \
    automake libtool unzip flex

COPY ctp2/ /ctp2/

RUN cd ctp2 && \
    ./autogen.sh && \
    CC=/usr/bin/gcc-5 \
    CXX=/usr/bin/g++-5 \
    CFLAGS="$CFLAGS -w -fuse-ld=gold" \
    CXXFLAGS="$CXXFLAGS -w -fuse-ld=gold" \
    ./configure --enable-silent-rules && \
    make


COPY ctp2CD/ /opt/ctp2/

RUN cp -r /ctp2/ctp2_data/ /opt/ctp2/

RUN cp -v /ctp2/ctp2_code/ctp2 /opt/ctp2/

RUN cp -v /ctp2/ctp2_code/mapgen/.libs/*.so /opt/ctp2/ctp2_program/ctp/dll/map/
