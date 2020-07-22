#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then if . | startswith("AGE") then "\($k) [group=\(.)]" else "\(.) -> \($k)" end else "" end' \
    | sed 's/ADVANCE_/node_/g' \
    | cat <(sed -n '/^ICON_ADVANCE_/s|^.*ICON_ADVANCE_\([^[:space:]]*\).*FirstFrame[^"]*"\([^"]*\).TGA".*$|  node_\1 [shape=box,\nlabel=<<TABLE border="0" cellspacing="0" cellborder="0"><TR><TD><IMG SCALE="TRUE" SRC="\L\2\E.png"/></TD></TR><TR><TD><B>ADVANCE_\1</B></TD></TR></TABLE>>] |p' ../ctp2_data/default/gamedata/uniticon.txt ) \
          <(awk '/{/ && !/TerrainEffect/ {a=$1; t=0}; /TerrainEffect.*{/ {t++}; $1 ~ /^EnableAdvance$/ && t<2 {print $2, a}' ../ctp2_data/default/gamedata/Units.txt ../ctp2_data/default/gamedata/buildings.txt ../ctp2_data/default/gamedata/Wonder.txt ../ctp2_data/default/gamedata/tileimp.txt | sort | awk '{arr[$1] = arr[$1] "," $2} END {for (i in arr) print i, arr[i]}' | awk '{sub(/^,/, "", $2); sub(/ADVANCE_/, "node_", $1); printf("%s [xlabel=\"%s\"]\n", $1, $2)}' ) \
          - \
    | cat <(echo 'digraph G {') \
          <(echo 'imagepath="pic555"') \
          <(echo 'rankdir=LR') \
          <(echo 'node [fontname="Sans serif" fontsize=8.0]') \
          <(echo 'forcelabels=true;') \
	  - \
	  <(echo '}')
