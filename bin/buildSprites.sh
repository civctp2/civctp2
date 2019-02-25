#!/bin/bash

mkdir -p newSprites/

SPRITES=$( ls -1 sprites/???.blend | grep -o '[0-9]\{3\}' )
for RES_FILE in $SPRITES ; do
    echo "Rendering ${RES_FILE}"
    
    ## render larger TGA for the great libraray
    mkdir -p TGAs/ # dir must exist for docker bind
    (
	docker run --rm -v $(pwd)/sprites/:/media/ -v $(pwd)/TGAs/:/TGAs/ ikester/blender /media/${RES_FILE}.blend \
	       --python-expr '
## space matters for the following python code!
import bpy
for scene in bpy.data.scenes:
    scene.render.resolution_x = 160
    scene.render.resolution_y = 120
' \
	       -o /TGAs/MGGP${RES_FILE}L.tga -F TGA -x 0 -f 1 -noaudio || exit 1
    ) | grep Saved
    mv TGAs/MGGP${RES_FILE}L.tga0* TGAs/MGGP${RES_FILE}L.tga

    mogrify -background white -depth 5 -compress None TGAs/MGGP${RES_FILE}L.tga || exit 9

    ## render sprite animation series
    (
	docker run --rm -v $(pwd)/sprites/:/media/ ikester/blender /media/${RES_FILE}.blend -o //${RES_FILE}/GG${RES_FILE}A.### -a -noaudio || exit 1
    ) | grep Saved

    for f in sprites/${RES_FILE}/GG${RES_FILE}A.*.tif ; do
	echo -n "$f "
	mogrify -background black -alpha Background -type TrueColorMatte $f || exit 2
	convert  $f -alpha set -fill '#FFFFFFFF' -draw 'color 0,0 reset' -type TrueColorMatte ${f/A/S} || exit 3
    done

    ## tiny TGA for the listing in the trade manager
    convert sprites/${RES_FILE}/GG${RES_FILE}A.001.tif -trim -resize 23x18 -gravity center -background none -extent 23x18  -channel RGBA -fx 'a==0 ? #FF00FFFF : u' -background white  -alpha remove -depth 5 TGAs/MGGI${RES_FILE}.tga || exit 10

    awk -f sprites/bin/spriteFileGen.awk -v nf=$(ls -1 sprites/${RES_FILE}/*A*.tif | wc -l) > GG${RES_FILE}.txt

    touch newSprites/GG${RES_FILE}.spr # file must exist for docker bind of file
    chmod a+rw GG${RES_FILE}.txt newSprites/GG${RES_FILE}.spr # user inside docker must have write permission!

    ## append sprite series folder, import TXT and export SPR to DOCKER_PARAMS
    DOCKER_PARAMS="${DOCKER_PARAMS} -v $(pwd)/sprites/${RES_FILE}/:/opt/ctp2/ctp2_program/ctp/${RES_FILE}/ "
    DOCKER_PARAMS="${DOCKER_PARAMS} -v $(pwd)/GG${RES_FILE}.txt:/opt/ctp2/ctp2_data/default/graphics/sprites/GG${RES_FILE}.txt "
    DOCKER_PARAMS="${DOCKER_PARAMS} -v $(pwd)/newSprites/GG${RES_FILE}.spr:/opt/ctp2/ctp2_data/default/graphics/sprites/GG${RES_FILE}_.spr "

    echo "${RES_FILE} done."
done

## $(pwd) needs to be evaluated in DOCKER_PARAMS
DOCKER_PARAMS=$(eval echo\ $DOCKER_PARAMS)
echo $DOCKER_PARAMS

docker run \
       --env DISPLAY \
       -v ${SHARED_PATH}/.X11-unix/:/tmp/.X11-unix/ \
       -v $(pwd)/.civctp2/save/:/opt/ctp2/ctp2_program/ctp/save/ \
       $DOCKER_PARAMS \
       $IMAGE_TAG:testSprite \
       ./ctp2 $CTP_PARAMS &

pidD=$!
       
java -cp "/opt/sikulixapi.jar:/opt/jython-standalone-2.7.1.jar" org.python.util.jython tests/create-sprite.sikuli/create-sprite.py $SPRITES || exit $?

kill $pidD

ls newSprites/ || true

## rename new sprites to the corresponding case used in ctp2 or ctp2CD (expecting ctp2CD to equal ctp2 in case of overlap)
cd newSprites/ &&
    paste -d ' ' \
	  <(ls -1 GG*.spr \
		| sort -f) \
	  <(find ../ctp2/ctp2_data/ ../ctp2CD/ctp2_data/ -type f \
		| grep -i -oFf <(ls -1 GG*.spr) \
		| sort -f) \
	| xargs -t -n 2  mv -n

ls

## fail if any SPR is dublicated (i.e. sikuli click event was not effective)

res=$(md5sum newSprites/* 2>/dev/null | cut -d " " -f1 | sort | uniq -d)
test -z "$res" && exit 0

echo $res
md5sum newSprites/*
exit 99
