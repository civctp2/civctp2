#!/bin/sh

## shell script to build debian package from within GitLab CI
## expects ctp2 to be installed under /opt in the GL-CI docker image (DI)
## needs dpkg-deb installed in the DI
## saves deb-file under /deb/, which is expected to be mounted as docker volume

BTYP=$1

mkdir -p /debian/opt/ctp2/
cp -r /opt/ctp2/ /debian/opt/

find ./debian -type d | xargs chmod 755 # to avoid error: control directory has bad permissions 777 (must be >=0755 and <=0775)
dpkg-deb --build /debian # uses debian/ to built debian.deb
mv /debian.deb /deb/ctp2-$BTYP.deb
