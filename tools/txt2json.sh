#!/bin/bash

awk '!/^#/ {if (NF == 2) {print ($2 ~/{/) ? "\"" $1 "\" : " $2 : "\"" $1 "\" : \"" $2 "\","} else if ($1 == "}") { print $0} }'  $1 `# convert 2 field records to key-value pair or object, otherwise end object or ignore line (like # or 1 field records like Capitalization) ` \
    | sed ':a;N;$!ba;s/,\n}/\n},/g'    `# replace ,\n} by \n}, ` \
    | cat <( echo "{") -               `# add initial { ` \
    | sed '$s/},/}\n}/'                `# replace last , with final } ` \
