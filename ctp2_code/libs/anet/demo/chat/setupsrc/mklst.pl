# makes setup.lst
# should be run by ../build.bat

printf("1;\n");
while (<>) {
	if (/\\setup\\chat\\(.*)\\[\_\-\.\$0-9a-zA-z]*/) {
		printf("..$& %s\n", $1);
	}
	elsif (/\\setup\\chat\\[.a-zA-z0-9]*/) {
		printf("..$&\n");
	}
}
