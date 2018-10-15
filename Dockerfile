################################################################################
# builder
################################################################################
FROM ubuntu:14.04 as builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev build-essential \
    automake libtool unzip flex libbsd-dev \
    libltdl-dev \
    wget ca-certificates

### build freetype-1.3.1
RUN wget http://sourceforge.net/projects/freetype/files/freetype/1.3.1/freetype-1.3.1.tar.gz && \
    tar xvf freetype-1.3.1.tar.gz && \
    cd freetype-1.3.1 && \
    wget 'http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD' -O config.sub `# support for newer CPUs`  && \
    ./configure && \
    make && \
    make install

COPY ctp2/ /ctp2/

RUN cd /ctp2 && \
    make bootstrap && \
    LD_LIBRARY_PATH="/usr/lib/x86_64-linux-gnu/" \
    CFLAGS="-fpermissive" \
    CXXFLAGS="-fpermissive" \
    ./configure --prefix=/opt/ctp2 --bindir=/opt/ctp2/ctp2_program/ctp --enable-silent-rules && \
    make && \
    make install


COPY ctp2CD/ /opt/ctp2/

RUN cp -r /ctp2/ctp2_data/ /opt/ctp2/

RUN cp -v /ctp2/ctp2_code/mapgen/.libs/*.so /opt/ctp2/ctp2_program/ctp/dll/map/

WORKDIR /opt/ctp2/ctp2_program/ctp/

CMD ["./ctp2"]

ENV USERNAME diUser
RUN useradd -m $USERNAME && \
    echo "$USERNAME:$USERNAME" | chpasswd && \
    usermod --shell /bin/bash $USERNAME && \
    usermod -aG video,audio $USERNAME
