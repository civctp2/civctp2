#!/bin/bash

XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth
touch $XAUTH
xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

mkdir -p $HOME/.civctp2/save/
touch $HOME/.civctp2/userprofile.txt # file must exist for docker file-vol
docker run -it \
       --volume=$XSOCK:$XSOCK:rw \
       --volume=$XAUTH:$XAUTH:rw \
       --device=/dev/dri:/dev/dri \
       --device=/dev/snd:/dev/snd \
       --env="ALSA_CARD=1" \
       --env="XAUTHORITY=${XAUTH}" \
       --env="DISPLAY" \
       --user="diUser" \
       -v $HOME/.civctp2/userprofile.txt:/opt/ctp2/ctp2_program/ctp/userprofile.txt \
       -v $HOME/.civctp2/save/:/opt/ctp2/ctp2_program/ctp/save \
       registry.gitlab.com/lynxabraxas/ctp2df/master:builder-latest $@

rm $XAUTH # remove to avoid accumulation of xauth settings
