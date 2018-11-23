package Activision;
import java.sql.*;
import java.util.*;

public class ScoreServletHandler
{
	private Hashtable servlets;  // ServletEntry's indexed by SessionType
	private myLoader classLoader;
	private String dbAddr;
	private java.sql.Connection dbConn;
	public static final int NO_SERVLET = -1;

	private class ServletEntry
	{
		public boolean valid;
		public ScoreServlet servlet;

		ServletEntry(ScoreServlet servlet)
		{
			if (servlet == null) {
				valid = false;
			} else {
				valid = true;
				this.servlet = servlet;
			}
		}
	}

	public ScoreServletHandler(String dbAddr)
	{
		servlets = new Hashtable();
		classLoader = new myLoader();
		dbConn = null;
		if (dbAddr == null) {
			this.dbAddr = null;
			System.out.println("ScoreServletHandler: no database specified");
		} else {
			this.dbAddr = new String("jdbc:mysql://"+ dbAddr);
		}
	}

	// Called by JNI code to get an appropriate ScoreReport to fill.
	// If the servlet associated with this SessionType has not been loaded,
	// it is created now.
	public Activision.ScoreReport getReport(short SessionType, byte SessionId[],
		short Flags, short ReporterID, int ReporterUID, short LeaverID,
		int LeaverUID, int nPlayers)
	{
		System.out.println("ScoreServletHandler.getReport(sessType:"+
			SessionType +")");
		// It might be nice not to create a new Integer on every get...
		Integer iSessionType = new Integer(SessionType);
		ServletEntry entry = (ServletEntry)servlets.get(iSessionType);
		if (entry == null) {
			Class servletClass;
			System.out.println("getReport: new sesstype "+ SessionType +
				".  Loading servlet score_"+ SessionType +"...");
			try {
				servletClass = classLoader.loadClass("score_"+ SessionType,
					false);
			} catch (ClassNotFoundException e) {
				System.out.println("getReport: Error: score_"+ SessionType +
					" not found.");
				entry = new ServletEntry(null);
				servlets.put(iSessionType, entry);
				return null;
			}

			ScoreServlet servlet = null;
			try {
				servlet = (ScoreServlet)servletClass.newInstance();
			}
			catch (InstantiationException ie) { System.out.println(ie); }
			catch (IllegalAccessException ae) { System.out.println(ae); }
			if (servlet == null) {
				System.out.println("getReport: Error: can't instantiate score_"+
					SessionType);
				entry = new ServletEntry(null);
				servlets.put(iSessionType, entry);
				return null;
			}

			if (servlet.needsDatabase()) {
				if (dbAddr == null) {
					System.out.println("getReport: Error: No database "+
						"available for score_"+ SessionType);
					entry = new ServletEntry(null);
					servlets.put(iSessionType, entry);
					return null;
				}
				if (dbConn == null) {
					System.out.println("getReport: Loading mysql driver...");
					try {
						Class.forName("org.gjt.mm.mysql.Driver");
					}
					catch(Exception e) { System.out.println(e); }

					System.out.println("getReport: Connecting to "+ dbAddr);
					try {
						dbConn = DriverManager.getConnection(dbAddr);
					}
					catch(SQLException se) { System.out.println(se); }

					if (dbConn == null) {
						System.out.println("getReport: Error: Database "+
							"connect failed for score_"+ SessionType);
						entry = new ServletEntry(null);
						servlets.put(iSessionType, entry);
						return null;
					}
					System.out.println("getReport: Connected to "+ dbAddr +
						" for score_"+ SessionType);
				}
				servlet.setDatabase(dbConn);
			}
			entry = new ServletEntry(servlet);
			servlets.put(iSessionType, entry);
		} else if (!entry.valid) {
			System.out.println("getReport: ScoreReport from sesstype "+
				SessionType +" ignored.");
			return null;
		}

		return entry.servlet.getReport(SessionId, Flags, ReporterID,
			ReporterUID, LeaverID, LeaverUID, nPlayers);
	}

	// Called by JNI code to report a session close message
	public void closeSession(short SessionType, byte SessionId[])
	{
		System.out.println("ScoreServletHandler.closeSession(sessType:"+
			SessionType +")");
		// It might be nice not to be creating a new Integer on every call...
		Integer iSessionType = new Integer(SessionType);
		ServletEntry entry = (ServletEntry)servlets.get(iSessionType);
		if (entry == null)
			return;  // haven't gotten any reports, so just ignore the close
		entry.servlet.closeSession(SessionId);	
	}

	// Called by JNI code periodically to process score reports that are ready
	public void poll()
		throws java.sql.SQLException
	{
		System.out.println("ScoreServletHandler.poll()");
		long timeNow = System.currentTimeMillis();
		Enumeration SessionIds = servlets.keys();
		while (SessionIds.hasMoreElements()) {
			Integer key = (Integer)SessionIds.nextElement();
			ServletEntry entry = (ServletEntry)servlets.get(key);
			if (entry.valid) {
				System.out.println("score_"+ key.intValue() +".poll(t:"+
					timeNow +")");
				entry.servlet.poll(timeNow);
			}
		}
	}
}
