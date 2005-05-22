# Makefile for the ActiveNet SDK and game server under Unix.
# To build the SDK under Windows or MacOS, see the src/win or src/mac directories.

all:
	cd src/3rdparty; make -f makefile.lin
	cd src/linux; ./build

clean:
	rm -f *.tgz *.zip
	cd src/linux; ./clean
