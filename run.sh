#!/bin/sh

XSOCK=/tmp/.X11-unix
if [ -z ${XVFBtmp+x} ]; then 
    XAUTH=/tmp/.docker.xauth
    touch $XAUTH
    xauth nlist $DISPLAY < /dev/null | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -

    DOCKERARGS="--volume $XSOCK:$XSOCK:rw --volume $XAUTH:$XAUTH:rw --env XAUTHORITY=$XAUTH"
    DOCKERARGS="${DOCKERARGS} --device /dev/dri:/dev/dri --device /dev/snd:/dev/snd --env ALSA_CARD=0 "
else
    DOCKERARGS="--volume $XVFBtmp:$XSOCK:rw"
fi

mkdir -p $HOME/.civctp2/save/
mkdir -p $HOME/.civctp2/logs/ # if not existent created by docker then owned by root
mkdir -p $HOME/.civctp2/maps/ # if not existent created by docker then owned by root
mkdir -p $HOME/.civctp2/Scenarios/ # if not existent created by docker then owned by root
touch $HOME/.civctp2/userprofile.txt # file must exist for docker file-vol
touch $HOME/.civctp2/userkeymap.txt # file must exist for docker file-vol

if test -s $HOME/.civctp2/Const.txt # file must exist for docker file-vol
then CON="-v $HOME/.civctp2/Const.txt:/opt/ctp2/ctp2_program/ctp/Const.txt"
fi     

## use -v to specify the folder with OGGs (TrackXX.ogg) for the game music, e.g.:
## ./run.sh -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro registry.gitlab.com/civctp2/civctp2/master:latest ./ctp2 fullscreen
docker run \
       --rm \
       $DOCKERARGS \
       --env "DISPLAY" \
       -v $HOME/.civctp2/userprofile.txt:/opt/ctp2/ctp2_program/ctp/userprofile.txt \
       -v $HOME/.civctp2/userkeymap.txt:/opt/ctp2/ctp2_program/ctp/userkeymap.txt \
       $CON \
       -v $HOME/.civctp2/save/:/opt/ctp2/ctp2_program/ctp/save \
       -v $HOME/.civctp2/logs/:/opt/ctp2/ctp2_program/ctp/logs \
       -v $HOME/.civctp2/maps/:/opt/ctp2/ctp2_program/ctp/maps \
       -v $HOME/.civctp2/Scenarios/:/opt/ctp2/Scenarios \
       $@

if [ -z ${XVFBtmp+x} ]; then 
    rm $XAUTH # remove to avoid accumulation of xauth settings
fi
