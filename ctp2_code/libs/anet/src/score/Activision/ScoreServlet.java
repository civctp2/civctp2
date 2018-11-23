package Activision;

public interface ScoreServlet
{
	// Request an object to fill with data using the ScoreReport interface.
	public ScoreReport getReport(byte SessionId[], short Flags,
		short ReporterID, int ReporterUID, short LeaverID, int LeaverUID,
		int nReported);

	// Handle a session close message.
	public void closeSession(byte SessionId[]);

	// Process any reports that are ready in this servlet.
	// Called periodically by ScoreServletHandler.
	public void poll(long timeNow)
		throws java.sql.SQLException;

	// Does this servlet use the mysql database?
	public boolean needsDatabase();

	// Set the database connection that this servlet will use.
	public void setDatabase(java.sql.Connection dbConn);
}
