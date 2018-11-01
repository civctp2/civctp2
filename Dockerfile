################################################################################
# builder
################################################################################
FROM ubuntu:16.04 as builder

RUN echo && echo "Docker libtoolize version BEFORE apt-get:" \
    && libtoolize --version || echo "libtoolize not found" ; echo

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev gtk+-2.0-dev \
        byacc gcc-5 g++-5 automake libtool unzip flex

RUN echo && echo "Docker libtoolize version AFTER apt-get:" \
    && libtoolize --version || echo "libtoolize not found" ; echo

