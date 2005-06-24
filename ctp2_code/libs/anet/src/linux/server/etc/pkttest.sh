#!/bin/sh
# Trivial test to see if packets are allowed through.
# Run this on a new server installation if you're not sure 
# port 21157 (the server port) has been opened up.
# You'll need a copy of gtest from the linux anet sdk.
# Note that this test can't be run if a server is running on this
# machine, since the server needs port 21157.

set -x
./gtestd -n=foo -p=california3.activision.com -Y:21157 <<_EOF_
sleep 10
quit
_EOF_
echo All packets sent or received:
cat output0.0
echo Checking for received packets:
if grep rx output0.0 ; then
        echo packets received.  Test passed.
else
        echo No packets received.  Test failed.
fi
