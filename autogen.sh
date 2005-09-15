#! /bin/sh
echo "Bootstraping Civilization Call to Power"
echo " "
echo "This is no official release by activision."
echo " "
aclocal -I ctp2_code/os/autoconf -I ctp2_code/libs/anet/macros
autoheader
libtoolize --force --copy
automake --foreign --add-missing --copy
autoconf
echo " "
echo "Bootstraping complete."
echo " "
echo "You may run ./configure and then make to compile CTP2."

exit 0
