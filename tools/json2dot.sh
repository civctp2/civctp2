#!/bin/bash

jq -r 'to_entries[] | "\(.value | .Prerequisites) -> \(.key)"'
