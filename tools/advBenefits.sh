#!/bin/bash

for f in ../ctp2_data/default/gamedata/Units.txt \
	     ../ctp2_data/default/gamedata/buildings.txt \
	     ../ctp2_data/default/gamedata/Wonder.txt \
	     ../ctp2_data/default/gamedata/tileimp.txt
	     do
	 awk '/{/ && !/TerrainEffect/ {a=$1; t=0}; /TerrainEffect.*{/ {t++}; $1 ~ /^EnableAdvance$/ && t<2 {print $2, a}' $f \
	     | sort \
	     | awk '{arr[$1] = arr[$1] "," $2} END {for (i in arr) print i, arr[i]}' \
	     | awk -v file=$1 -f ../tools/advBenefits.awk
      done
