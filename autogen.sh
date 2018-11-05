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
echo "Preparing freetype 1.3"
cd ctp2_code/libs/freetype-1.3.1
echo "Running aclocal..."
aclocal
echo "Running autoheader..."
autoheader
echo "Running libtoolize..."
libtoolize --force --copy
#echo "Running automake..."
#automake --foreign --add-missing --copy
echo "Running autoconf..."
autoconf
#hack hack hack
cp ft_conf.h.am ft_conf.h.in
cd ../../..
echo "Bootstraping anet now"
cd ctp2_code/libs/anet
make bootstrap
echo " "
cd ../../..
echo "Bootstraping complete."
echo " "
echo "You may run ./configure and then make to compile CTP2."

exit 0
