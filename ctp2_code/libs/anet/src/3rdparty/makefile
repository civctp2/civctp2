# Makefile for third party code used by tca

clean: .SYMBOLIC
	-deltree /y d3des
	-deltree /y md5

unpack: d3des/unpacked md5/unpacked

d3des/unpacked: d3des.zip
	unzip d3des
	touch d3des/unpacked

md5/unpacked: md5.zip
	unzip md5
	touch md5/unpacked
