#!/bin/bash

XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth
touch $XAUTH
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

docker run -it \
       --volume=$XSOCK:$XSOCK:rw \
       --volume=$XAUTH:$XAUTH:rw \
       --device=/dev/dri:/dev/dri \
       --device=/dev/snd:/dev/snd \
       --env="ALSA_CARD=1" \
       --env="XAUTHORITY=${XAUTH}" \
       --env="DISPLAY" \
       --user="diUser" \
       registry.gitlab.com/civctp2/civctp2/linux:builder-latest $@

rm $XAUTH # remove to avoid accumulation of xauth settings
