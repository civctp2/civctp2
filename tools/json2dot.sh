#!/bin/bash

jq -r 'to_entries[] | .key as $k | .value[]? | "\(.) -> \($k)"' \
    | cat <(echo 'digraph G {') - <(echo '}')
