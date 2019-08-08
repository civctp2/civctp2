#!/bin/bash

jq -r 'to_entries[] | "\(.value) -> \(.key)"'
