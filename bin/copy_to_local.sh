#! /bin/sh

CP_ARGS="-uv"

if [ "$1" = "--force" ];
then
	CP_ARGS="-v";
fi

if ! [ -d ctp2_code ];
then
	echo ctp2_code not found.  Please execute from the main source directory.
	exit 1;
fi

LOCAL_SOURCE=etc/local-install

if ! [ -f $LOCAL_SOURCE ];
then
	echo $LOCAL_SOURCE not found.  Please create it and enter the path
	echo of your local CTP2 installation to which you would like files copied.
	exit 1;
fi

LOCAL=`cat etc/local-install`

if [ "$LOCAL" = "" ];
then
	echo $LOCAL_SOURCE is empty.  Please enter into it the path of your local
	echo CTP2 installation to which you would like files copied.
	exit 1;
fi

if ! [ -d $LOCAL ];
then
	echo $LOCAL not found or not a directory.  Please check that this is the
	echo correct path of your local CTP2 installation.
	exit 1;
fi

if ! [ -d $LOCAL/ctp2_program/ctp ];
then
	echo $LOCAL/ctp2_program/ctp not found or not a directory.  Please check
	echo that $LOCAL is the correct path of your local CTP2 installation.
	exit 1;
fi

for DLL_DIR in map net util;
do
	cp $CP_ARGS ctp2_code/ctp/dll/$DLL_DIR/* \
		$LOCAL/ctp2_program/ctp/dll/$DLL_DIR ;
done

for FILE in 'anet*' 'ctp2.*' 'CivCTP_dbg.*' 'ctp2r.*' appstr.txt;
do
	cp $CP_ARGS ctp2_code/ctp/$FILE $LOCAL/ctp2_program/ctp/;
done

for DIR in Scenarios ctp2_data
do
	find $DIR -wholename '*.svn*' -prune -o -type d \
		-exec mkdir -pv "$LOCAL/{}" ";"
	find $DIR -wholename '*.svn*' -prune -o -type f \
		-exec cp $CP_ARGS "{}" "$LOCAL/{}" ";"
done

