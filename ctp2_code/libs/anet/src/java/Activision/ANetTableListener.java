package Activision;

/** 
 * Interface used to notify user code of table variable changes.
 */
public interface ANetTableListener
{
	/** 
	 * Called when a table variable is set.
	 */
	public void tableVarSet(ANet.ANetTable t, byte[] subkey);

	/** 
	 * Called when a table variable is deleted.
	 */
	public void tableVarDeleted(ANet.ANetTable t, byte[] subkey);

	/** 
	 * Called when a table variable is sent.
	 */
	public void tableVarSent(ANet.ANetTable t, byte[] subkey);
}


