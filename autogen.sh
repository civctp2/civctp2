#! /bin/sh
echo "Bootstrapping Civilization Call to Power"
echo " "
echo "This is no official release by activision."
echo " "
echo "Running aclocal..."
aclocal -I ctp2_code/os/autoconf -I ctp2_code/libs/anet/macros
echo "Running autoheader..."
autoheader
echo "Running libtoolize..."
libtoolize --force --copy
echo "Running automake..."
automake --foreign --add-missing --copy
echo "Running autoconf..."
autoconf
echo " "
echo "Bootstraping complete."
echo " "
echo "You may run ./configure and then make to compile CTP2."

exit 0
