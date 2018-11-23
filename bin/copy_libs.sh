#! /bin/sh

DIRECTX_DIR="C:\DX90SDK"
DXMEDIA_DIR="C:\DXMedia"

if [ ! cygpath --unix . > /dev/null 2>&1 ]; then
	echo "Please make sure cygpath.exe is in your PATH."
	exit 1
fi

CDKDIR_UNX=`cygpath --unix $CDKDIR`
CP="cp"
MKDIR="mkdir"

if [ -z "$CDKDIR" ]; then
	echo "CDKDIR not set."
	exit 1
fi
if [ ! -d $DIRECTX_DIR ]; then
	echo "DIRECTX_DIR=$DIRECTX_DIR does not exist."
	exit 2
fi
if [ ! -d $DXMEDIA_DIR ]; then
	echo "DXMEDIA_DIR=$DXMEDIA_DIR does not exist."
	exit 3
fi

$MKDIR -p "$CDKDIR_UNX/../ctp2_code/libs/directx/lib"
$MKDIR -p "$CDKDIR_UNX/../ctp2_code/libs/DXMedia/lib"
$CP -av "`cygpath --unix \"$DIRECTX_DIR\Lib\"`"/* "$CDKDIR_UNX/../ctp2_code/libs/directx/lib"
$CP -av "`cygpath --unix \"$DXMEDIA_DIR\Lib\"`"/* "$CDKDIR_UNX/../ctp2_code/libs/DXMedia/lib"

