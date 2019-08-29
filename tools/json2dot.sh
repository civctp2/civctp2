#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then "\(.) -> \($k)" else "" end' \
    | cat <(sed -n '/^ICON_ADVANCE_/s|^.*ICON_ADVANCE_\([^[:space:]]*\).*FirstFrame[^"]*"\([^"]*\).TGA".*$|  ADVANCE_\1 [shape=box,\nlabel=<<TABLE border="0" cellspacing="0" cellborder="0"><TR><TD><IMG SCALE="TRUE" SRC="\L\2\E.png"/></TD></TR>\n<TR><TD>\1</TD></TR></TABLE>>] |p' $1) - \
    | sed '/^<TR><TD>/s/_/ /g' `# needs label= to be in a different line than node-name (\n above)!` \
    | cat <(echo 'digraph G {') \
          <(echo 'imagepath="pic555"') \
          <(echo 'rankdir=LR') \
          <(echo 'node [fontname="Sans serif" fontsize=7.5]') \
	  - \
	  <(echo '}')
