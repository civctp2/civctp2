#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then "\(.) -> \($k)" else "" end' \
    | cat <(sed -n '/^ICON_ADVANCE_/s|^.*ICON_ADVANCE_\([^[:space:]]*\).*FirstFrame[^"]*"[CM2_]*\([^"]*\).TGA".*$|  ADVANCE_\1 [label=<<TABLE border="0" cellspacing="0" cellborder="0"><TR><TD><IMG SCALE="TRUE" SRC="\L\2\E.png"/></TD></TR><TR><TD>\1</TD></TR></TABLE>>, shape=box] |p' $1) - \
    | cat <(echo 'digraph G {') \
          <(echo 'imagepath="pic555"') \
          <(echo 'rankdir=LR') \
	  - \
	  <(echo '}')
