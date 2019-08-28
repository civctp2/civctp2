#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | if $k != . then "\(.) -> \($k)" else "" end' \
    | cat <(echo 'digraph G {') - <(echo '}')
