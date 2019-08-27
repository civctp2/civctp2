#!/bin/bash

awk '!/^#/ {if (NF == 2) {print ($2 ~/{/) ? "\"" $1 "\" : " $2 : "\"" $1 "\" : \"" $2 "\","} else if ($1 == "}") { print $0} }'  $1 `# convert 2 field records to key-value pair or object, otherwise end object or ignore line (like # or 1 field records like Capitalization) ` \
    | egrep 'Prerequisites|{|}' \
    | sed ':a;N;$!ba;s/,\n}/\n},/g'    `# replace ,\n} by \n}, ` \
    | sed ':a;N;$!ba;s/{\n}/[],/g'     `# replace {} by empty array (no-prerequisite) ` \
    | cat <( echo "{") -               `# add initial { ` \
    | sed '$s/},/}\n}/'                `# replace last , with final } ` \
    | jq -s --stream 'reduce (.[] | select(length==2)) as $kv ({};
      $kv[0][0] as $k
      |$kv[1] as $v
      | (.[$k]|type) as $t
      | if $t == "null" then .[$k] = $v
        elif $t == "array" then .[$k] += [$v]
        else .[$k] = [ .[$k], $v ]
        end)' # convert duplicate keys to array # https://stackoverflow.com/questions/36956590/json-fields-have-the-same-name#36974355
