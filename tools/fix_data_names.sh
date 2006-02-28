#!/bin/bash
#
# THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
#
# Fix case of Civ:CTP2 data file names on case sensitive file systems.
#
# Just cd into the ctp2_data directory and call this script or pass the path
# to the ctp2_data directory as argument to this script.
#
# Written by Marc Haisenko <marc@darkdust.net>

if [ -n "$1" -a -d "$1" ] ; then
	cd "$1"
fi

if [ ! \( -d "default" -a -d "english" \) ] ; then
	echo -n "You have to cd into the ctp2_data directory before calling "
	echo " this script !"
	exit 1
fi

# List of language directories
LANGUAGES="english french german italian japanese spanish"

# List of files to rename. Each string consists of the relativ path to the
# file, followed by a colon and the new file name. If the path starts with
# "LANGUAGE", that file is renamed in all directories defined in the LANGUAGES
# variable.
NAMES=( "LANGUAGE/uidata/layouts/SpriteEditor.ldl:spriteeditor.ldl"
	"LANGUAGE/gamedata/Strings.txt:strings.txt"
	"default/gamedata/Advance.txt:advance.txt"
	"default/gamedata/Units.txt:units.txt"
	"default/gamedata/Wonder.txt:wonder.txt"
	"default/gamedata/Orders.txt:orders.txt"
	"default/gamedata/EndGameObjects.txt:endgameobjects.txt"
	"default/aidata/Goals.txt:goals.txt"
	"default/aidata/WonderBuildLists.txt:wonderbuildlists.txt"
	"default/aidata/BuildingBuildLists.txt:buildingbuildlists.txt"
	"default/aidata/ImprovementLists.txt:improvementlists.txt"
	"default/graphics/cursors/UC001.tga:uc001.tga"
	"default/graphics/cursors/UC002.tga:uc002.tga"
	"default/graphics/cursors/UC003.tga:uc003.tga"
	"default/graphics/cursors/UC004.tga:uc004.tga"
	"default/graphics/cursors/UC005.tga:uc005.tga"
	"default/graphics/cursors/UC006.tga:uc006.tga"
	"default/graphics/cursors/UC014.tga:UC014.TGA"
	"default/graphics/cursors/UC015.tga:UC015.TGA"
	"default/graphics/cursors/UC016.tga:UC016.TGA"
	"default/graphics/cursors/Uc030.tga:UC030.TGA"
	"default/graphics/cursors/Uc031.tga:UC031.TGA"
	"default/graphics/cursors/UC032.tga:UC032.TGA"
	"default/graphics/cursors/UC033.tga:UC033.TGA"
	"default/graphics/cursors/UC034.tga:UC034.TGA"
	"default/graphics/cursors/UC035.tga:UC035.TGA"
	"default/graphics/cursors/UC036.tga:UC036.TGA"
	"default/graphics/cursors/UC037.tga:UC037.TGA"
	"default/graphics/cursors/UC038.tga:UC038.TGA"
	"default/graphics/cursors/UC039.tga:UC039.TGA"
	"default/graphics/cursors/UC040.tga:UC040.TGA"
	"default/graphics/cursors/UC041.tga:UC041.TGA"
	"default/graphics/cursors/UC042.tga:UC042.TGA"
	"default/graphics/cursors/UC043.tga:UC043.TGA"
	"default/graphics/cursors/UC044.tga:UC044.TGA"
	"default/graphics/cursors/UC045.tga:UC045.TGA"
	"default/graphics/cursors/UC046.tga:UC046.TGA"
	"default/graphics/cursors/UC047.tga:UC047.TGA"
	"default/graphics/cursors/UC048.tga:UC048.TGA"
	"default/graphics/cursors/UC049.tga:UC049.TGA"
	"default/graphics/cursors/UC050.tga:UC050.TGA"
	"default/graphics/cursors/UC051.tga:UC051.TGA"
	"default/graphics/cursors/UC052.tga:UC052.TGA"
	"default/graphics/cursors/UC053.tga:UC053.TGA"
	"default/graphics/cursors/UC054.tga:UC054.TGA"
	"default/graphics/cursors/UC055.tga:UC055.TGA"
	"default/graphics/cursors/UC056.tga:UC056.TGA"
	"default/graphics/cursors/UC057.tga:UC057.TGA"
	"default/graphics/cursors/UC058.tga:UC058.TGA"
	"default/graphics/cursors/UC059.tga:UC059.TGA"
	"default/graphics/cursors/UC060.tga:UC060.TGA"
	"default/graphics/cursors/UC061.tga:UC061.TGA"
	"default/graphics/cursors/UC062.tga:UC062.TGA"
	"default/graphics/cursors/UC063.tga:UC063.TGA"
	"default/graphics/cursors/UC064.tga:UC064.TGA"
	"default/graphics/cursors/UC065.tga:UC065.TGA"
	"default/graphics/cursors/UC066.tga:UC066.TGA"
	"default/graphics/cursors/UC067.tga:UC067.TGA"
	"default/graphics/cursors/UC068.tga:UC068.TGA"
	"default/graphics/cursors/UC069.tga:UC069.TGA"
	"default/graphics/cursors/UC070.tga:UC070.TGA"
	"default/graphics/cursors/UC071.tga:UC071.TGA"
	"default/graphics/cursors/UC072.tga:UC072.TGA"
	"default/graphics/cursors/UC073.tga:UC073.TGA"
	"default/graphics/cursors/UC074.tga:UC074.TGA"
	"default/graphics/cursors/UC075.tga:UC075.TGA"
	"default/graphics/cursors/UC076.tga:UC076.TGA"
	"default/graphics/cursors/UC077.tga:UC077.TGA"
	"default/graphics/cursors/UC078.tga:UC078.TGA"
	"default/graphics/cursors/UC079.tga:UC079.TGA"
	"default/graphics/cursors/UC080.tga:UC080.TGA"
	"default/graphics/cursors/UC081.tga:UC081.TGA"
	"default/graphics/cursors/UC082.tga:UC082.TGA"
	"default/graphics/cursors/UC083.tga:UC083.TGA"
	"default/graphics/cursors/UC084.tga:UC084.TGA"
	"default/graphics/cursors/UC085.tga:UC085.TGA"
	"default/graphics/cursors/UC086.tga:UC086.TGA"
	"default/graphics/cursors/UC087.tga:UC087.TGA"
	"default/graphics/pictures/UG026.TGA:ug026.tga"
	"default/graphics/pictures/UG027.TGA:ug027.tga"
)

# Renames a file if it exists.
function rename_file() {
		dir="$1"
		oldname="$2"
		newname="$3"

		if [ ! -e "$dir/$oldname" ] ; then
			return
		fi
		
		echo "Renaming \"$oldname\" to \"$newname\" in \"$dir\""
		mv -f "$dir/$oldname" "$dir/$newname"
}

##############################################################################

# Iterate over all entries in the NAMES array
for entry in "${NAMES[@]}" ; do
	# Split the entry into smaller pieces.
	tmpname="${entry%%:*}"
	dir="${tmpname%/*}"
	oldname="${tmpname##*/}"
	newname="${entry##*:}"

	if [ "${dir%%/*}" = "LANGUAGE" ] ; then
		# If the directory name has the string "LANGUAGE" in it then
		# iterate over the LANGUAGES variable and insert its entries
		# at the place of "LANGUAGE" in the directory name.
		for language in $LANGUAGES ; do
			rename_file "${dir/LANGUAGE/$language}" "$oldname" \
				"$newname"
		done
	else
		# Normal directory name, just rename
		rename_file "$dir" "$oldname" "$newname"
	fi
done

