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
(found++) == 3 && length(PRS) {
    n= split(PRS, PRs, "|")
    delete PRs[n] # remove last element, empty due to tailing "|"
    for (i in PRs){
	PR=PRs[i]
	cmd = sprintf("awk -v var=%s '$1 ~ var { gsub(var, \"\", $0); print }' gl_str.txt", PR)
	cmd | getline pr
	close(cmd)
	printf("<L:DATABASE_ADVANCES,%s>%s<e>\n", PR, pr)
	PR=0
	}
    } 1
# !/[END]/ && found {
#     } 1
