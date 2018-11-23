#! /bin/sh

find . \
  -iregex ".*/\([^/]*\(eula\|readme\)[^/]*\.\(htm\|html\|rtf\|txt\)\|Makefile\(\|\.am\|\.in\)\|[^/]+\.\(c\|cpp\|h\|hpp\|htm\|l\|m4\|y\)\)$" \
  -not -regex ".*/CVS\($\|/.*$\)" \
  -exec dos2unix {} \;

