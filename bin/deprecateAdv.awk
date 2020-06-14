# read a Great_Library.txt
# when a  UNIT_.*_PREREQ is found use the info to extract ObsoleteAdvance from Units.txt into variable OA
# insert that info after "Requires:" before "[END]"

match($0, /.*(UNIT_.*)_PREREQ.*/, s) {
    cmd = sprintf("awk -v var=%s '/{/ {a=$1}; /ObsoleteAdvance/ { if ( a ~ var ) print $2}' ./ctp2_data/default/gamedata/Units.txt", s[1])
    cmd | getline OA
    close(cmd)
    }
/Requires:/ && OA {
    found=1
    }
/\[END\]/ && found == 1 {
    printf("Obsolete:\n<L:DATABASE_ADVANCES,%s>%s<e>\n", OA, OA)
    OA=0
    found=0
    } 1
