<HTML>
<HEAD>
<TITLE>Score Demo</TITLE>
</HEAD>
<?

/* Script Name:    score.php3
Dan Kegel, 30 August 1998.
*/

/* First let's set some global variables.  Edit these to fit your installation. */
$hostname = "localhost";	// the Internet host running the MySQL database
$database = "alink";		// The MySQL database name
$user = "dank";				// the MySQL user who owns the database
$password = "";				// and his MySQL password.

/* Now, connect to the database. */
$err = mysql_connect($hosthame,$user,$password);
//echo "mysql_connect($hostname, $user, $password) returns err $err\n";
//echo "errno0:". mysql_errno().": ".mysql_error()."<BR>";

/* Now to define functions. */
/* The main screen.  Printed when $state is empty. */
Function Main_Menu() {
	//echo "Entering Main_Menu\n";
	echo "<H1><CENTER> Welcome To The Score Demo </CENTER></H1>"; 
	echo "<FORM METHOD = \"POST\" ACTION=\"score.php3\">";
	echo "<INPUT CHECKED TYPE=\"radio\" NAME=\"state\" VALUE=\"List\"><B> Show Users</B><BR>";
	echo "<INPUT TYPE=\"radio\" NAME=\"state\" VALUE=\"List_byscore\"><B>Show Top Scores</B><P>";
	echo "<INPUT TYPE=\"submit\" VALUE=\"Continue\">";
	echo "</FORM>";
}

/* List the users in the database */
Function xList($sortfield) {
	global $database;

	//echo "Entering xList\n";
	echo "<CENTER><H2>User Scores</CENTER></H2><P>";
	$result = mysql_db_query($database,"SELECT users.uid, users.username, score_65.max_score, score_65.sum_score FROM users,score_65 where users.uid = score_65.uid ORDER BY $sortfield");
	echo "<TABLE BORDER = 10 CELLPADDING = 2>";
	$total_rows = mysql_num_rows($result); $counter = 0;
	echo "<TR><TD><B>Username</B></TD><TD><B>max score</B></TD><TD><B>total score</B></TD></TR>";
	while($counter < $total_rows) {
		$uid = mysql_result($result,$counter,"uid"); echo "<TR><TD>\n";
		//echo "<A HREF=score.php3?uid=$uid&state=Print_Contact>"; 
		echo mysql_result($result,$counter,"username");
		//echo "</A>\n";
		echo "</TD><TD>\n";
		echo mysql_result($result,$counter,"max_score"); echo "</TD><TD>\n";
		echo mysql_result($result,$counter,"sum_score"); echo "</TD></TR>\n";
		$counter = $counter + 1;
	}
	echo "</TABLE>";
}

//echo "Entering main dispatch:\n";

/* The main loop.  Call functions based on the value of $state, which 
gets set via a hidden INPUT TYPE. */
switch($state) {
	case "";
	Main_Menu();
	break;

	case "List";
	xList("username");
	break;

	case "List_byscore";
	xList("sum_score desc");
	break;
}
?>
</BODY>
</HTML>
