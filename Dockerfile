################################################################################
# builder
################################################################################
FROM i386/ubuntu:14.04 as builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    libsdl1.2-dev libsdl-mixer1.2-dev libsdl-image1.2-dev byacc gtk+-2.0-dev build-essential \
    automake libtool unzip flex libbsd-dev \
    libltdl-dev \
    wget ca-certificates

### build freetype-1.3.1
COPY misc/ftdump-newer-GCC.patch /root/

RUN wget http://sourceforge.net/projects/freetype/files/freetype/1.3.1/freetype-1.3.1.tar.gz && \
    tar xvf freetype-1.3.1.tar.gz && \
    cd freetype-1.3.1 && \
    wget 'http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD' -O config.sub `# support for newer CPUs`  && \
    patch -d test -i /root/ftdump-newer-GCC.patch && \
    ./configure && \
    make && \
    make install
    
ENV LD_LIBRARY_PATH "${LD_LIBRARY_PATH}:/usr/local/lib"
### freetype-1.3.1 built


COPY ./ /ctp2/

RUN cd /ctp2 && \
    make bootstrap && \
    LD_LIBRARY_PATH="/usr/lib/i386-linux-gnu/" \
    CFLAGS="-fpermissive -Wl,--no-as-needed -m32" \
    CXXFLAGS="-fpermissive -Wl,--no-as-needed -m32" \
    ./configure --prefix=/opt/ctp2 --bindir=/opt/ctp2/ctp2_program/ctp --enable-silent-rules && \
    make && \
    make install


COPY ctp2CD/ /opt/ctp2/

RUN cp -r /ctp2/ctp2_data/ /opt/ctp2/

RUN cp -v /ctp2/ctp2_code/mapgen/.libs/*.so /opt/ctp2/ctp2_program/ctp/dll/map/ && \
    cp -v /ctp2/ctp2_code/mapgen/.libs/*.la /opt/ctp2/ctp2_program/ctp/dll/map/

WORKDIR /opt/ctp2/ctp2_program/ctp/

CMD ["./ctp2"]

ENV USERNAME diUser
RUN useradd -m $USERNAME && \
    echo "$USERNAME:$USERNAME" | chpasswd && \
    usermod --shell /bin/bash $USERNAME && \
    usermod -aG video,audio $USERNAME
