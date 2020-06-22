#!/bin/bash

d=$(dirname $1)
f=$(basename $1)
pwd=$(pwd)
echo "$d $f"

cd $d
awk -i inplace -f $pwd/bin/deprecateAdv.awk $f
sed -i 's/\r//g' $f
