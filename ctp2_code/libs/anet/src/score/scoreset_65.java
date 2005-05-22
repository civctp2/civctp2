import java.sql.*;
import java.util.*;

public class scoreset_65
{
	public static final long TIMEOUT = 600000;  // ms after creation to close
												// this scoreset, regardless.
	private static java.sql.Connection dbConn;
	private Hashtable scorereps;  // scorerep_65's indexed by reporterID
	private long timeCreated;
	private boolean bClosed;
	private int nReports;
	private int nExpected;

	public scoreset_65(long timeCreated)
	{
		scorereps = new Hashtable();
		this.timeCreated = timeCreated;
		bClosed = false;
		nReports = 0;
		nExpected = 0;
	}

	// Request an object to fill with data using the ScoreReport interface.
	public Activision.ScoreReport getReport(short Flags, short ReporterID,
		int ReporterUID, short LeaverID, int LeaverUID, int nReported)
	{
		System.out.println("scoreset_65[Rid:"+ ReporterID +"].getReport()"); 
		Integer iReporterID = new Integer(ReporterID);
		if (bClosed) {
			System.out.println("getReport: Scoreset closed, ignoring");
			return null;	// This score set is not accepting any more reports
		}
		if (scorereps.containsKey(iReporterID)) {
			System.out.println("getReport: duplicate ReporterID, ignoring");
			return null;	// This id has already reported on this event
		}
		if (nReported <= 0) {
			System.out.println("getReport: invalid nReported "+ nReported);
			return null;
		}
		Activision.ScoreReport scorerep = new scorerep_65(Flags, ReporterID,
			ReporterUID, LeaverID, LeaverUID, nReported);
		scorereps.put(iReporterID, scorerep);

		nReports++;
		// It would be better to check ReporterIDs against the IDs in the
		// leaver's report, but this isn't bad in most cases...
		if ((Flags & scorerep.FLAGS_SELFEXIT) != 0)
			nExpected = nReported;

		return scorerep;
	}

	public void close(long timeNow)
		throws java.sql.SQLException
	{
		if (bClosed)
			return;
		bClosed = true;

		// Crosscheck the scoreset for inconsistancies.
		// Submit the final results to the database.

		// for the moment, just print out what we got:
		System.out.println("--- Closed a score set ---");
		System.out.println("Created:"+ timeCreated +"  Closed:"+ timeNow +
			"  nReports:"+ scorereps.size());
		Enumeration ScoreReports = scorereps.elements();
		while (ScoreReports.hasMoreElements()) {
			scorerep_65 scorerep = (scorerep_65)ScoreReports.nextElement();
			scorerep.print();
		}
		System.out.println("--------------------------");
	}

	public void poll(long timeNow)
		throws java.sql.SQLException
	{
		if (bClosed)
			return;
		if (nExpected > 0 && nReports >= nExpected) {
			// We have a complete set of scores, more or less
			System.out.println("poll: scoreset complete ("+ nReports +"/"+
				nExpected +") in "+ (timeNow-timeCreated) +"ms");
			close(timeNow);
		}
		if ((timeNow - timeCreated) > TIMEOUT) {
			// This scoreset has timed out
			if (nExpected > 0)
				System.out.println("poll: scoreset timed out ("+ nReports +"/"+
					nExpected +") in "+ (timeNow-timeCreated) +"ms");
			else
				System.out.println("poll: scoreset timed out ("+ nReports +
					", no SELFEXIT) in "+ (timeNow-timeCreated) +"ms");
			close(timeNow);
		}
	}

	public boolean isClosed()
	{
		return bClosed;
	}

	public static boolean needsDatabase()
	{
		return false;
	}

	public static void setDatabase(java.sql.Connection dbConn)
	{
		scoreset_65.dbConn = dbConn;
	}
}
