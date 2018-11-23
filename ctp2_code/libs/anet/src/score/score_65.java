import java.sql.SQLException;
import java.sql.Connection;
import java.util.*;

public class score_65 implements Activision.ScoreServlet
{
	private Hashtable sessions;  // session_65's indexed by SessionId byte[]'s

	public score_65()
	{
		sessions = new Hashtable();
	}

	// Request an object to fill with data using the ScoreReport interface.
	public Activision.ScoreReport getReport(byte SessionId[], short Flags,
		short ReporterID, int ReporterUID, short LeaverID, int LeaverUID,
		int nReported)
	{
		if (SessionId == null) {
			System.out.println("session_65[sessid:NULL].getReport");
			return null;
		}
		System.out.println("session_65[sessid:"+
			(int)((SessionId[0]+256)%256) +"."+
			(int)((SessionId[1]+256)%256) +"."+
			(int)((SessionId[2]+256)%256) +"."+
			(int)((SessionId[3]+256)%256) +"."+
			(int)((SessionId[4]+256)%256) +"."+
			(int)((SessionId[5]+256)%256) +"."+
			(int)((SessionId[6]+256)%256) +"."+
			(int)((SessionId[7]+256)%256) +"].getReport()");
		long timeNow = System.currentTimeMillis();
		session_65 session = (session_65)sessions.get(SessionId);
		if (session == null) {
			System.out.println("getReport: New sessionid.");
			session = new session_65(timeNow);
			sessions.put(SessionId, session);
		}
		if (session.isClosed()) {
			System.out.println("getReport: Session is closed, ignoring report");
			return null;
		}

		return session.getReport(timeNow, Flags, ReporterID, ReporterUID,
			LeaverID, LeaverUID, nReported);
	}

	// Handle a session close message.
	public void closeSession(byte SessionId[])
	{
		if (SessionId == null)
			return;
		session_65 session = (session_65)sessions.get(SessionId);
		if (session == null)
			return;
		System.out.println("session_65[sessid:"+
			(int)((SessionId[0]+256)%256) +"."+
			(int)((SessionId[1]+256)%256) +"."+
			(int)((SessionId[2]+256)%256) +"."+
			(int)((SessionId[3]+256)%256) +"."+
			(int)((SessionId[4]+256)%256) +"."+
			(int)((SessionId[5]+256)%256) +"."+
			(int)((SessionId[6]+256)%256) +"."+
			(int)((SessionId[7]+256)%256) +"].closeSession()");
		session.close();
		sessions.remove(SessionId);
	}

	// Process any reports that are ready in this servlet.
	// Called periodically by ScoreServletHandler.
	public void poll(long timeNow)
		throws java.sql.SQLException
	{
		Enumeration SessionIds = sessions.keys();
		while (SessionIds.hasMoreElements()) {
			byte[] key = (byte[])SessionIds.nextElement();
			session_65 session = (session_65)sessions.get(key);
			System.out.println("session_65[sessid:"+
				(int)((key[0]+256)%256) +"."+
				(int)((key[1]+256)%256) +"."+
				(int)((key[2]+256)%256) +"."+
				(int)((key[3]+256)%256) +"."+
				(int)((key[4]+256)%256) +"."+
				(int)((key[5]+256)%256) +"."+
				(int)((key[6]+256)%256) +"."+
				(int)((key[7]+256)%256) +"].poll()");
			session.poll(timeNow);
			if (session.isClosed()) {
				sessions.remove(key);
			}
		}
	}

	// Does this servlet use the mysql database?
	public boolean needsDatabase()
	{
		return scoreset_65.needsDatabase();
	}

	// Set the database connection that this servlet will use.
	public void setDatabase(java.sql.Connection dbConn)
	{
		scoreset_65.setDatabase(dbConn);
	}
}
