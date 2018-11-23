# Perl script to extract help messages from gtest.c.
# $Log: help.pl $
# Revision 1.1  1997/02/28 22:19:06  dkegel
# Initial revision

while (<>) {
	if (/--\|(.*)\|--/) {
		printf("\"%s\",\n", $1);
	}
	if (/MATCH\("(.*)",.*"(.*)"/) {
		printf("\"    %-17s -- %s\",\n", $1, $2);
	}
}
