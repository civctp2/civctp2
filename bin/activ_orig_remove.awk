BEGIN \
	{
		stage = 0
		unix = 1
		changed = 0
	}
/\r$/ { unix = 0 }

{
#	dissonance = 0
	oldstage = stage
	if (stage == 4)
	{
		stage = 0
		changed = 1
	}
}
/^\/\/ ACTIVISION_ORIGINAL[\t ]*\r?$/ \
	{
		if (stage==0)
			stage++
#		else
#			dissonance = 1
	}
/^\/\/ - When defined, generates the original Activision code.[\t ]*\r?$/ \
	{
		if (stage==1)
			stage++
#		else
#			dissonance = 1
	}
/^\/\/ - When not defined, generates the modified Apolyton code.[\t ]*\r?$/ \
	{
		if (stage==2)
			stage++
#		else
#			dissonance = 1
	}
/^\/\/[\t ]*\r?$/ \
	{
		if (stage==3)
			stage++
#		else
#			dissonance = 1
	}

{
	#print "stage=", stage, ", oldstage=", oldstage
	if (stage == 0)
		print
	else if (oldstage == stage)
	{
		if (unix == 0)
		{
			print "// ACTIVISION_ORIGINAL\r"
			if (stage >= 2)
				print "// - When defined, generates the original Activision code.\r"
			if (stage >= 3)
				print "// - When not defined, generates the modified Apolyton code.\r"
		}
		else
		{
			print "// ACTIVISION_ORIGINAL"
			if (stage >= 2)
				print "// - When defined, generates the original Activision code."
			if (stage >= 3)
				print "// - When not defined, generates the modified Apolyton code."
		}
		print
		stage = 0
	}
}

END \
	{
		if (changed == 1)
			exit 1;
		else
			exit 0;
	}
