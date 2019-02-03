#!/bin/bash

XSOCK=/tmp/.X11-unix
XAUTH=/tmp/.docker.xauth
touch $XAUTH
xauth nlist $DISPLAY < /dev/null | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

mkdir -p $HOME/.civctp2/save/
touch $HOME/.civctp2/userprofile.txt # file must exist for docker file-vol
touch $HOME/.civctp2/userkeymap.txt # file must exist for docker file-vol
## use -v to specify the folder with OGGs (TrackXX.ogg) for the game music, e.g.:
## ./run.sh -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro registry.gitlab.com/civctp2/ctp2df/test-gui:test ./ctp2
docker run \
       --rm \
       --volume=$XSOCK:$XSOCK:rw \
       --volume=$XAUTH:$XAUTH:rw \
       --device=/dev/dri:/dev/dri \
       --device=/dev/snd:/dev/snd \
       --env="ALSA_CARD=1" \
       --env="XAUTHORITY=${XAUTH}" \
       --env="DISPLAY" \
       --user="diUser" \
       -v $HOME/.civctp2/userprofile.txt:/opt/ctp2/ctp2_program/ctp/userprofile.txt \
       -v $HOME/.civctp2/userkeymap.txt:/opt/ctp2/ctp2_program/ctp/userkeymap.txt \
       -v $HOME/.civctp2/save/:/opt/ctp2/ctp2_program/ctp/save \
       $@

rm $XAUTH # remove to avoid accumulation of xauth settings
