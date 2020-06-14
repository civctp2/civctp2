# read a Great_Library.txt
# when a  UNIT_.*_PREREQ is found use the info to extract ObsoleteAdvance from Units.txt into variable OA
# insert that info 2 lines after "Requires:"

match($0, /.*(UNIT_.*)_PREREQ.*/, s) {
    cmd = sprintf("awk -v var=%s '/{/ {a=$1}; /ObsoleteAdvance/ { if ( a ~ var ) print $2}' ./ctp2_data/default/gamedata/Units.txt", s[1])
    cmd | getline OA
    close(cmd)
    }
/Requires:/ {
    found=1
    }
(found++) == 3 && OA {
    printf("Obsolete:\n<L:DATABASE_ADVANCES,%s>%s<e>\n", OA, OA)
    OA=0
    } 1
