#!/bin/sh
set -x
set -e
die() {
	echo "Test failed: $0"
	exit 1
}

case $1 in 
"" )
	sh test.sh normal || die
	sh test.sh debug || die
	sh test.sh profile || die
	sh test.sh log || die
	exit 0
	;;
normal | NORMAL )
	folder='normal'
	suffix=''
	;;
debug | DEBUG )
	folder='debug'
	suffix='d'
	;;
profile | PROFILE )
	folder='profile'
	suffix='p'
	;;
log | LOG )
	folder='log'
	suffix='l'
	;;
esac

(
	cd dptab
	echo -n "building dptabt1${suffix}..."
	./build $1 || die
	echo -n "dptabt1${suffix} regression test..."
	./$folder/dptabt1$suffix || die
	echo "passed"
)
