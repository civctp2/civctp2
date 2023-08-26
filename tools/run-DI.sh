#!/bin/sh

## shell script to run docker image (DI) created by GitLab CI (or built locally)
## used in .gitlab-ci.yml for running smoke tests in GitLab CI
## also ment for playing ctp2 directly from DI (needs no local built, only docker)
##
## saves games, maps, Scenarios, scores, user settinges etc in $HOME/.civctp2/
## use -v to specify the folder with OGGs (TrackXX.ogg) for the game music and videos, e.g.:
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     registry.gitlab.com/civctp2/civctp2/master:latest ./ctp2 fullscreen
## 
## start a saved game directly with e.g.:
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     registry.gitlab.com/civctp2/civctp2/master:latest ./ctp2 -lsave/games/Julius/QUICKSAVE-Julius
## 
## specify environment vars for the docker environment (e.g. use different sound card):
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     --env="ALSA_CARD=1" \
##     registry.gitlab.com/civctp2/civctp2/master:latest ./ctp2 fullscreen
## 
## some sound cards apparently cannot be chosesn by ALSA env-vars (e.g. --env="ALSA_PCM_CARD=0" --env="ALSA_PCM_DEVICE=3) but work with SDL env-var
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     --env="AUDIODEV=hw:0,3" \
##     registry.gitlab.com/civctp2/civctp2/master:latest ./ctp2 fullscreen
##
## run ctp2 through gdb for interactive debugging:
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     --env="ALSA_CARD=1" \
##     --security-opt seccomp=unconfined \
##     -it \
##     registry.gitlab.com/myname/civctp2/master4testing:test-debug \
##     gdb --args ./ctp2 -lsave/games/Cyrus/QUICKSAVE-Cyrus # then just type run in gdb prompt
##
## run ctp2 through gdb with no-interactive debugging:
## ./run-DI.sh \
##     -v $HOME/ctp2CD/ctp2_program/ctp/music/:/opt/ctp2/ctp2_program/ctp/music/:ro \
##     -v $HOME/ctp2CD/ctp2_program/ctp/videos/:opt/ctp2/ctp2_data/default/videos/:ro \
##     --env="ALSA_CARD=1" \
##     --security-opt seccomp=unconfined \
##     -it \
##     registry.gitlab.com/myname/civctp2/master4testing:test-debug \
##     gdb -quiet -batch -x gdb.cmds --args ./ctp2 -lsave/games/Cyrus/QUICKSAVE-Cyrus # catching assertions is only possible with debug version, i.e. DI:test-debug, with DI:test-plain only SIGSEGV can be caught



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
touch $HOME/.civctp2/hscore.txt # file must exist for docker file-vol

if test -s $HOME/.civctp2/Const.txt # file must exist for docker file-vol
then CON="-v $HOME/.civctp2/Const.txt:/opt/ctp2/ctp2_program/ctp/Const.txt"
fi     

docker run \
       --rm \
       $DOCKERARGS \
       --env "DISPLAY" \
       -v $HOME/.civctp2/userprofile.txt:/opt/ctp2/ctp2_program/ctp/userprofile.txt \
       -v $HOME/.civctp2/userkeymap.txt:/opt/ctp2/ctp2_program/ctp/userkeymap.txt \
       -v $HOME/.civctp2/hscore.txt:/opt/ctp2/ctp2_data/default/gamedata/hscore.txt \
       $CON \
       -v $HOME/.civctp2/save/:/opt/ctp2/ctp2_program/ctp/save \
       -v $HOME/.civctp2/logs/:/opt/ctp2/ctp2_program/ctp/logs \
       -v $HOME/.civctp2/maps/:/opt/ctp2/ctp2_program/ctp/maps \
       -v $HOME/.civctp2/Scenarios/:/opt/ctp2/Scenarios \
       $@

if [ -z ${XVFBtmp+x} ]; then 
    rm $XAUTH # remove to avoid accumulation of xauth settings
fi
