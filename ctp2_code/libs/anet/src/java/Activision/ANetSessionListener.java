package Activision;

/** 
 * Interface used to notify user code of session info changes.
 */
public interface ANetSessionListener
{
	/** 
	 * Called when a new session is detected.
	 */
	public void sessionOpened(ANet.Session s);

	/** 
	 * Called when an existing session changes its name or number of
	 * players.
	 */
	public void sessionChanged(ANet.Session s);

	/** 
	 * Called when an existing session is destroyed.
	 */
	public void sessionClosed(ANet.Session s);
}


