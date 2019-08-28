#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then "\(.) -> \($k)" else "" end' \
    | cat <(sed -n '/^ICON_ADVANCE_/s|^.*ICON_\(ADVANCE_[^[:space:]]*\).*FirstFrame[^"]*"CM2_\([^"]*\)".*$|\1 [image="\L\2"]|p' $1) - \
    | cat <(echo 'digraph G {') \
          <(echo 'imagepath="pic555"') \
	  - \
	  <(echo '}')
