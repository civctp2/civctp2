#! /bin/sh

echo "Removing CVS dirs recursively from $PWD."
echo "Press enter to continue"
echo "or press Ctrl-C to abort"

read enter
unset enter

find . \
  -type d \
  -name "CVS" \
  -exec sh -c 'echo "rm -r {}" ; rm -r {}' \; 2> /dev/null

