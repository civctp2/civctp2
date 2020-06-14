# read a Great_Library.txt
# when a  UNIT_.*_PREREQ is found use the info to extract ObsoleteAdvance from Units.txt into variable OA
# insert that info 1 lines after "L:DATABASE_ADVANCES,ADVANCE_" (which is language indep.)

match($0, /.*(UNIT_.*)_PREREQ.*/, s) {
    cmd = sprintf("awk -v var=%s '/{/ {a=$1}; /ObsoleteAdvance/ { if ( a ~ var ) print $2}' ./ctp2_data/default/gamedata/Units.txt", s[1])
    cmd | getline OA
    close(cmd)
    }
/L:DATABASE_ADVANCES,ADVANCE_/ {
    found=1
    }
(found++) == 2 && OA {
    oa=OA
    gsub(/ADVANCE_/, "", oa)
    gsub(/_/, " ", oa)
    printf("Obsolete:\n<L:DATABASE_ADVANCES,%s>%s<e>\n", OA, tolower(oa))
    OA=0
    } 1
