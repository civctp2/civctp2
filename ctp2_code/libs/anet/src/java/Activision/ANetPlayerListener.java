
package Activision;

/** 
 * Interface used to notify user code of player info changes.
 */
public interface ANetPlayerListener
{
	/** 
	 * Called when a new player is detected.
	 */
	public void playerEnters(ANet.Session s, ANet.Player p);

	/** 
	 * Called when an existing player changes its name.
	 */
	public void playerChanged(ANet.Session s, ANet.Player p);

	/** 
	 * Called when an existing player is destroyed.
	 */
	public void playerLeaves(ANet.Session s, ANet.Player p);
}
