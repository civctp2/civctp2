# prepend advance benefits before table end
# put in this file to avoid extra escaping of single and double quotes
# used by advBenefits.sh
    {
    sub(/^,/, "", $2)
    sub(/ADVANCE_/, "node_", $1)
    cmd = sprintf("awk -i inplace '/%s/ {sub(/<[/]TABLE>/, \"<TR><TD>%s</TD></TR></TABLE>\", $0)} 1 ' %s", $1, $2, file)
    system(cmd)
    }
