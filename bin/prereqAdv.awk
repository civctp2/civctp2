# read a Great_Library.txt
# when a  ADVANCE_.*_PREREQ is found use the info to extract prerequisites from Advance.txt into variable PR
# insert that info 1 lines afterwards

match($0, /.*(ADVANCE_.*)_PREREQ.*/, s) {
    PRS="" # essential for advances without prerequisites
    cmd = sprintf("awk -v var=%s '{if($1 ~ var) found=$1}; /Prerequisites/ && found { printf($2\"|\")}; /}/ && found {exit;}' ../../default/gamedata/Advance.txt", s[1])
    cmd | getline PRS
    close(cmd)
    found=1
    }
/\[END\]/ && found {
    n= split(PRS, PRs, "|")
    delete PRs[n] # remove last element, empty due to tailing "|"
    for (i in PRs){
	PR=PRs[i]
	cmd = sprintf("awk -v var=%s -v FPAT='([^ \t]+)|(\"[^\"]+\")' ' $1 ~ var { print $2 }' gl_str.txt", PR)
	cmd | getline pr
	close(cmd)
	gsub("\"","", pr) # remove quotes around quoted fields
	printf("<L:DATABASE_ADVANCES,%s>%s<e>\n", PR, pr)
	PR=0
	}
    found=0
    }
!/<L:DATABASE_ADVANCES,ADVANCE_/ || !found {
    print
    }
