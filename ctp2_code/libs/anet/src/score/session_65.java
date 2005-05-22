import java.sql.SQLException;
import java.util.*;

public class session_65
{
	private Hashtable scoresets;  // scoreset_65's indexed by leaverID
	private long timeCreated;
	private long timeLastModified;
	private boolean bClosed;

	public session_65(long timeCreated)
	{
		scoresets = new Hashtable();
		this.timeCreated = timeCreated;
		this.timeLastModified = timeCreated;
		bClosed = false;
	}

	// Request an object to fill with data using the ScoreReport interface.
	public Activision.ScoreReport getReport(long timeNow, short Flags,
		short ReporterID, int ReporterUID, short LeaverID, int LeaverUID,
		int nReported)
	{
		if (bClosed)
			return null;
		System.out.println("scoreset_65[Lid:"+ LeaverID +"].getReport()"); 
		Integer iLeaverID = new Integer(LeaverID);
		scoreset_65 scoreset = (scoreset_65)scoresets.get(iLeaverID);
		if (scoreset == null) {
			System.out.println("getReport: new LeaverID "+ LeaverID);
			scoreset = new scoreset_65(timeNow);
			scoresets.put(iLeaverID, scoreset);
		}
		if (scoreset.isClosed()) {
			System.out.println("getReport: Scoreset is closed, ignoring");
			return null;
		}

		Activision.ScoreReport rep = scoreset.getReport(Flags,
			ReporterID, ReporterUID, LeaverID, LeaverUID, nReported);
		if (rep != null)
			timeLastModified = timeNow;
		return rep;
	}

	public void close()
	{
		if (bClosed)
			return;
		bClosed = true;

		// Do any processing necessary at closing time
		long timeNow = System.currentTimeMillis();
		Enumeration leaverIDs = scoresets.keys();
		while (leaverIDs.hasMoreElements()) {
			Integer key = (Integer)leaverIDs.nextElement();
			scoreset_65 scoreset = (scoreset_65)scoresets.get(key);
			System.out.println("scoreset_65[Lid:"+ key.intValue() +"].close()");
			try {
				scoreset.close(timeNow);
			}
			catch (java.sql.SQLException se) { System.out.println(se); }
		}
	}

	public void poll(long timeNow)
		throws java.sql.SQLException
	{
		if (bClosed)
			return;
		Enumeration leaverIDs = scoresets.keys();
		while (leaverIDs.hasMoreElements()) {
			Integer key = (Integer)leaverIDs.nextElement();
			System.out.println("scoreset_65[Lid:"+ key.intValue() +"].poll()");
			scoreset_65 scoreset = (scoreset_65)scoresets.get(key);
			scoreset.poll(timeNow);
		}
	}

	public boolean isClosed()
	{
		return bClosed;
	}
}
