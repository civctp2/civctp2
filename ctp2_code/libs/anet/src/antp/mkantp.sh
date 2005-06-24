#!/bin/sh
gcc -DUNIX -I. -I../score -I../../h antpclnt.c antpget.c ../score/wmqfile.c -o antpclnt
gcc -g -D_DEBUG -DUNIX -I. -I../score -I../../h antpclnt.c antpget.c ../score/wmqfile.c -o antpclntd
gcc -DUNIX -I. -I../score -I../../h antpserv.c ../score/wmqfile.c -o antpserv
gcc -g -D_DEBUG -DUNIX -I. -I../score -I../../h antpserv.c ../score/wmqfile.c -o antpservd
