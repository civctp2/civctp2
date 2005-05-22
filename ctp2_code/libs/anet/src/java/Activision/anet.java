/**
 Game networking package.

 $Log: anet.java $
 Revision 1.24  1997/10/08 21:34:27  lbennett
 Conform to new dpEnumApps.
 Revision 1.23  1997/10/07 02:18:19  anitalee
 fixed value of dialingMethod; should define symbolic constants
 Revision 1.22  1997/10/02 03:24:30  lbennett
 Added dprint.
 Revision 1.21  1997/09/30 23:09:35  anitalee
 added params to create()
 Revision 1.20  1997/09/29 04:45:12  dkegel
 Added definition for (as yet unused) commInitReq class; will be arg to Create
 Revision 1.19  1997/09/29 02:42:15  dkegel
 Added Player.getLatency()
 Revision 1.18  1997/09/27 21:21:31  dkegel
 Pass back dpReceive error return values as empty packets with new err field nonzero.
 Revision 1.17  1997/09/14 02:33:51  dkegel
 Support dpReadyToFreeze
 Revision 1.16  1997/09/13 23:01:26  dkegel
 Added getFlags() in session.
 Revision 1.15  1997/08/12 21:43:45  dkegel
 Allow caller to manipulate installedApp fields.
 Revision 1.14  1997/07/15 21:45:40  dkegel
 Added support for shellOpts on each installed app.
 Revision 1.13  1997/07/15 18:00:57  dkegel
 Search for ANetj.dll in dll subdirectory first.
 Revision 1.12  1997/07/10 22:31:24  dkegel
 Added ANet.InstalledApp.execWithConsole().
 Revision 1.11  1997/07/10 18:58:44  dkegel
 Don't forget to allocate adrMaster and userField in session constructor,
 or native method will fail
 Revision 1.10  1997/06/27 22:05:06  dkegel
 First cut at sending & receiving packets.
 Revision 1.9  1997/06/27 18:15:49  dkegel
 Support writeObject aka dpFreeze.
 Revision 1.8  1997/06/27 02:13:23  dkegel
 Supports player creation.
 Revision 1.7  1997/06/25 23:04:56  dkegel
 Supports enumPlayers; supports Session.getKarma().
 Revision 1.6  1997/06/25 19:31:16  dkegel
 Show values for transport.needs.
 Revision 1.5  1997/06/25 18:37:23  dkegel
 First cut at supporting dpOpen and dpClose.
 Revision 1.4  1997/06/23 23:12:04  dkegel
 1. Use primitive byte array rather than object Byte array.
 2. Add default constructor for Session (else JNI panic).
 Revision 1.3  1997/06/23 22:37:50  dkegel
 Added setGameServer(), initial cut at enumSessions() (not yet tested).
 Revision 1.2  1997/06/22 22:56:34  dkegel
 Added inner class InstalledApp.
 Improved doc.
 Added constants for driver capabilities.
 Revision 1.1  1997/06/20 22:56:32  dkegel
 Initial revision
*/

package Activision;

import java.util.*;		// For Vector

/**
 * Main object for the Activenet networking library. <p>
 * This implementation is a wrapper around the native DLL implementation.
 */
public class ANet
{
	/*---------------------- InstalledApp -----------------------------------*/

	/** Represents an application installed on the user's computer, e.g.
	 * Quake.  Provides enough information to launch the application.
	 */
	public static class InstalledApp {
		String name;
		String path;
		String args;
		String cwd;
		String shellOpts;
		int sessionType;
		short platform;
		short language;
		short majorVersion;
		short minorVersion;
		short majorLatest;
		short minorLatest;

		public String getName() { return name; }
		public void   setName(String name) { this.name = name; }

		public String getPath() { return path; }
		public void   setPath(String path) { this.path = path; }

		public String getArgs() { return args; }
		public void   setArgs(String args) { this.args = args; }

		public String getCwd() { return cwd; }
		public void   setCwd(String cwd) { this.cwd = cwd; }

		public String getShellOpts() { return shellOpts; }
		public void   setShellOpts(String shellOpts) { this.shellOpts = shellOpts; }

		public int    getSessionType() { return sessionType; }
		public void   setSessionType(int sessionType) { this.sessionType = sessionType; }

		public short  getPlatform() { return platform; }
		public void   setPlatform(short p) { this.platform = p; }

		public short  getLanguage() { return language; }
		public void   setLanguage(short l) { this.language = l; }

		public short  getCurrentVersionMajor() { return majorVersion; }
		public void   setCurrentVersionMajor(short v) { this.majorVersion = v; }

		public short  getCurrentVersionMinor() { return minorVersion; }
		public void   setCurrentVersionMinor(short v) { this.minorVersion = v; }

		public short  getLatestVersionMajor() { return majorLatest; }
		public void   setLatestVersionMajor(short v) { this.majorLatest = v; }

		public short  getLatestVersionMinor() { return minorLatest; }
		public void   setLatestVersionMinor(short v) { this.minorLatest = v; }

		/** Shut down Java and run this application.  
		 * Only returns on failure.
		 */
		public native int execWithConsole();

		/** Updates latest version info for this application; returns true if
		 * current application version is at least as recent as latest version.
		 */
		public native boolean checkAppVersion();

		/** Download patch for this application; returns true upon success.
		 */
		public native boolean downloadPatch();
	}

	/** Get a list of the applications installed on the user's computer
	 * that support this networking library.
	 * Used to present a list of game programs to the user.
	 * <p> 
	 *   returns Null if error occured
	 *   returns Vector with no elements if no applications are found
	 *   returns Vector of class InstalledApp if applications are found
	 */
    public native Vector enumInstalledApps();


	/*---------------------- Transport ------------------------------------*/

	/** A network driver installed on the user's computer.
	 * Used to present a list of network drivers to the user.
	 */
	public static class ANetTransport
	{
		/* Bits of capabilities */
		public static final int DRIVER_IS_VISIBLE		= 1;		/* Whether to show driver to user */
		public static final int DRIVER_KNOWS_PLAYERLST	= 2;		/* Whether to call dpCommThaw */
		public static final int DRIVER_ALLOWS_GAMELIST	= 4;		/* Whether to call dpEnumSessions */
		public static final int DRIVER_IS_FAST			= 8;		/* Whether to set update rate down */
		public static final int DRIVER_PREFERS_BROADCAST	= 16;	/* Whether to use broadcast for frequent data */
		public static final int DRIVER_NO_BROADCAST	= 32;		/* Whether to avoid broadcast entirely */

		/* Bits of needs */
		public static final int DRIVER_NEEDS_SESSIONID	= 2;		/*unused*/
		public static final int DRIVER_NEEDS_PORTNUM	= 4;		/*0=com1*/
		public static final int DRIVER_NEEDS_BAUD		= 8;
		public static final int DRIVER_NEEDS_BASEADR	= 0x10;		/*unused*/
		public static final int DRIVER_NEEDS_HWIRQ		= 0x20;		/*unused*/
		public static final int DRIVER_NEEDS_SWINT		= 0x40;		/*unused*/
		public static final int DRIVER_NEEDS_PHONENUM	= 0x80;
		public static final int DRIVER_NEEDS_MODEMINISTR= 0x100;

		String name;
		String path;
		int needs;
		int capabilities;

		public String getName() { return name; }
		public String getPath() { return path; }
		public int getNeeds() { return needs; }
		public int getCapabilities() { return capabilities; }
	}

	/** Get a list of the available transports.  
	 * <p>(This would be an
	 * inner method, but javah -jni can't grok those yet.)
	 */
	public static native ANetTransport[] enumTransports();
	
	/*------------------------ Server --------------------------------------*/

	/** A game server somewhere on the Internet.
	 * Used to present a list of game servers to the user.
	 */
	public static class ANetServer
	{
		String hostname;
		int rtt_ms_avg;
		int loss_percent;
		int sesstype_players;

		public String getHostname() { return hostname; }
		public int getRtt_ms_avg() { return rtt_ms_avg; }
		public int getLoss_percent() { return loss_percent; }
		public int getPlayers() { return sesstype_players; }
	}

	/** 
	 * Start measuring network roundtrip time to all known game servers
	 *  and retrieving player info about the given session type. 
	 * <p>Supported only for Internet transport.
	 * <br>Call enumServers() about one second after calling pingServers().
	 */
	public native int pingServers(int type);

	/** 
	 * List known game servers, and the network roundtrip time to them. 
	 * <p>Supported only for Internet transport.
	 * <br>Call pingServers() first.
	 */
	public native ANetServer[] enumServers();	/* call 1 second later */

	/** 
	 * Log in to a game server.  Start downloading information about the
	 * given session type (use 0 to get info about all session types).
	 * <p>Supported only for Internet transport.
	 * <br>Bug: this should provide some notification that the login succeeded
	 * or failed, but currently doesn't!
	 */
	public native int setGameServer(ANetServer s, int type);
	public int setGameServer(ANetServer s)
	{
		return setGameServer(s, 0);
	}

	/*------------------------ Session ------------------------------------*/

	/** A game session somewhere on the Internet.
	 * Used to create game sessions, and to present a list of game sessions
	 * to the user.
	 */
	public static class Session
	{
		String name;
		String password;
		int type;
		int maxPlayers;
		int curPlayers;

		int karma;
		byte[] adrMaster;
		byte[] reserved2;
		byte[] userField;
		int dwUser1;
		int flags;

		ANetPlayerListener listener;

		public Session() {} ;
		public Session(String name, String password, int type, int maxPlayers,
		int dwUser1)
		{
			this.name = name;
			this.password = password;
			this.type = type;
			this.maxPlayers = maxPlayers;
			this.curPlayers = 0;
			this.karma = 0;
			this.dwUser1 = dwUser1;
			this.flags = 0;
			this.adrMaster = new byte[10];
			this.reserved2 = new byte[12];	// dptab_KEY_MAXLEN
			this.userField = new byte[10];
		}

		public String getName() { return name; }
		public String getPassword() { return password; }
		public int getType() { return type; }
		public int getMaxPlayers() { return maxPlayers; }
		public int getCurPlayers() { return curPlayers; }
		public int getDwUser1() { return dwUser1; }
		public int getFlags() { return flags; }
		public int getKarma() { return karma; }

		public void setName(String v) { name = v; }
		public void setPassword(String v) { password = v; }
		public void setMaxPlayers(int v) { maxPlayers = v; }

		/** 
		 * Create a Player in the current session.
		 */
		public native int createPlayer(String name);

		/** 
		 * Watch Player changes in this session.  
		 * Initially, only the player created by createPlayer() is noticed.
		 */
		public void addPlayerListener(ANetPlayerListener listener)
		{
			this.listener = listener;
		}
		public void removePlayerListener(ANetPlayerListener listener)
		{
			if (this.listener == listener)	// will this work?
				this.listener = null;
		}
	}

	/** 
	 * List known game sessions. 
	 * Only returns sessions with a matching type.  0 matches all types.
	 */
	public native Session[] enumSessions(int type);

	/** 
	 * Return the current game session. 
	 */
	public native Session getSessionDesc();

	/** 
	 * Modify the current game session. 
	 */
	public native int setSessionDesc(Session s);

	/** 
	 * Set the current session to be open or closed to new players.
     * This method may only be called by the host, and returns an
     * error otherwise.
	 */
	public native int enableNewPlayers(boolean b);

	/**
	 * Join a session found by enumSessions, or if create is true,
	 * create a new session.
	 * On completion, the sessionOpened method of cb will be called;
	 * on failure, the s argument will be null.
	 */
	public native int openSession(Session s, boolean create, ANetSessionListener cb);

	/**
	 * Close a session opened by openSession.
	 * Currently, only one session may be open at a time.
	 */
	public native int closeSession();

	/*-------------------------- Player ------------------------------------*/

	public static class Player {
		String name;
		int id;
		boolean local;

		public String  getName()  { return name; }
		public int     getId()    { return id; }
		public boolean getLocal() { return local; }

		/** 
		 * Destroy the given Player.
		 */
		public native int destroyPlayer();

		/** 
		 * Retrieve the given Player's round-trip latency
		 * as measured by round-trip time of last few reliable packets.
		 * Units are milliseconds.
		 */
		public native int getLatency();

        public String toString() {return name + " (" + id + ")"; }
	}

	/** 
	 * List Players in the current session.
	 */
	public native Player[] enumPlayers();


	/*-------------------------- Packet -------------------------------------*/

	/** A message between two players in a session.
	*/
	public static class Packet {
		int src;
		int dest;
		int err;
		byte[] body;

		public byte[]  getBody()  { return body; }
		public int     getSrc()   { return src; }
		public int     getDest()  { return dest; }
		public int     getErr()   { return err; }

		public Packet() {} ;
		public Packet(int src, int dest, byte[] body) {
			this.src = src;
			this.dest = dest;
			this.body = body;
			this.err = 0;
		}
	}

	/** Check for incoming packets.  Call this about five times/second.
	 */
	public native Packet[] receivePackets();

	/** Send a packet.  The src and dest parameters are the id fields
	 * of the Players.  Should probably be a member function of Player.
	 * Flags is 0 for unreliable and 1 for reliable.
	 * Offset is how far into the byte array to start sending at;
	 * this is a kludge for the moment...
	 */
	public native int sendPacket(int src, int dest, int flags, byte[] body,
		int offset);

	public int sendPacket(int src, int dest, int flags, byte[] body) {
		return sendPacket(src, dest, flags, body, 0);
	};

	/*---------------------- commInitReq --------------------------*/

	/** Tells how to configure the communications.
	 * Needed only for modem and null modem.
	 * Consult the C documentation for meanings of fields.
	 */
	public static class commInitReq {
		int sessionId;		/* random number - must be different from peer's */
		int portnum;		/* 0 for com1, 1 for com2 */
		int baud;			/* used only by modem driver; null modem fixed */
		int dialingMethod;	/* 0 for pulse, 1 for tone */
		String phonenum;
		String modeministr;

		public int    getSessionId() { return sessionId; }
		public void   setSessionId(int sessionId) { this.sessionId = sessionId; }

		public int    getPortnum() { return portnum; }
		public void   setPortnum(int portnum) { this.portnum = portnum; }

		public int    getBaud() { return baud; }
		public void   setBaud(int baud) { this.baud = baud; }

		public int    getDialingMethod() { return dialingMethod; }
		public void   setDialingMethod(int dialingMethod) { this.dialingMethod = dialingMethod; }

		public String getPhonenum() { return phonenum; }
		public void   setPhonenum(String phonenum) { this.phonenum = phonenum; }

		public String getModeministr() { return modeministr; }
		public void   setModeministr(String modeministr) { this.modeministr = modeministr; }

	}

	public static class commPortName {
		int portnum;
		String name;

		public int    getPortnum() { return portnum; }
		public String getName() { return name; }
	}

	public static native Vector enumPorts(String transportname);

	/*---------------------- Table --------------------------*/

	/** 
	 * Table management.  Mostly used by server.
	 * Tables are specified by a byte array key. Entries in the table are
	 * specifed by a byte array subkey; entries are byte arrays.  Tables
	 * can be subscribed to from another player (the publisher) to get
	 * the table entries from that player; this can be done two ways:
	 * 	1. The subscriber calls addPublisher then requestSubscription.
	 *  2. The subscriber calls addPublisher and the publisher calls
	 *  addSubscriber. 
	 * The former is probably easier since the latter assumes the publisher
	 * knows that the subscriber is subscribing.
	 */
	public static class ANetTable {
		byte[] key;
		ANetTableListener listener;

		public byte[]  getKey() { return key; }

		public class createException extends Exception {
			public createException() {}
			public createException(String msg) {
				super(msg);
			}
		}
		/* create/get dptab_table_t table. */
		private native int getTable(boolean fCreate);

		/**
		 * Constructor for ANetTable creates/gets the dptab_table_t 
		 *  for the specified key.
		 * Set fCreate true if the dptab_table_t for this table already
		 * exists on this machine; otherwise set fCreate false.
		 */
		public ANetTable(byte[] key, boolean fCreate) 
		throws createException {
			this.key = key;
			int err = this.getTable(fCreate);
			if ((err != ANetResult.OK) && (err != ANetResult.ALREADY)) {
				this.key = null;
				throw new createException("dptab_createTable error " + err);
			}
		}

		/** 
		 * Watch table variable changes.
		 */
		public void addTableListener(ANetTableListener listener)
		{
			this.listener = listener;
		}
		public void removeTableListener(ANetTableListener listener)
		{
			if (this.listener == listener)	// will this work?
				this.listener = null;
		}

		/** Destroy table.
		*/
		public native int destroyTable();

		/** Add player p with id pId as subscriber.
		*/
		public native int addSubscriber(int pId);

		/** Add player with id pId as publisher where pubKey is
		* the key of the table on the publisher's machine.
		*/
		public native int addPublisher(byte[] pubKey, int pId);

		/** Request subscription from player p with id pId where pubKey is
		* the key of the table on the publisher's machine.  Need to call
		* addPublisher separately to accept entries from that publisher.
		*/
		public native int requestSubscription(byte[] pubKey, int pId);

		/** Delete player p with id pId as publisher where pubKey is
		* the key of the table on the publisher's machine.
		*/
		public native int deletePublisher(byte[] pubKey, int pId);

		/** Delete player p with id pId as subscriber.
		*/
		public native int deleteSubscriber(int pId);

		/** Get table size (number of entries in table).
		*/
		public native int getTableSize();

		/** Set table value to buf for subkey where buf must be a byte array.
		* Returns error.
		*/
		public native int setVal(byte[] subkey, Object buf, int hops);

		/** Get table value by subkey. Returns value.
		*/
		public native Object getValByKey(byte[] subkey);

		/** Get table value by index. Returns value and subkey.
		*/
		public native Object getValByIndex(int index, byte[] subkey);

		/** Delete table value for subkey. Returns error.
		*/
		public native int deleteVal(byte[] subkey);
	}

	/*-------------------------- ANet -------------------------------------*/

	/** Once you've picked a transport, call this to load it.
	* Normally, thawFilename should be null.  'Params' may be null
	* except for modem and null modem connections.
	* If you are being launched, then 'params' is null, and thawFilename
	* should be "freeze.dat".
	*/
	public native int create(ANetTransport transport, commInitReq params, String thawFilename);
	public int create(long dpAdr) {
		if (dpAdr == 0) return ANetResult.BAD;
		setDpPtr(dpAdr); return ANetResult.OK;
	}
	private native void setDpPtr(long dpAdr);

 	/** Returns dp_RES_BUSY if dp can't freeze due to ongoing reliable 
	* transmission.
	*/
	public native int readyToFreeze();
	
	/** Call this before destroy if you want to save the state of dp
	* in a file for reconstruction in a subprocess in a call to create().
	* Otherwise known as dpFreeze().
	*/
	public native int writeObject(String thawFilename);
	
	/** Unload the transport
	* flags is zero to hang up, 1 to keep phone line open 
	* (e.g. when launching a game).
	*/
	public native int destroy(int flags);

	/** Stop modem dialing or answering.
	*/
	public native void stopDialAnswer();

	/* Automatically bring in the native method on startup. */
    static {
		System.loadLibrary("anet/ANetj2");
    }

	/** Call this to write to the library's log file.
	*/
	public static native int dprint(String message);

	/** Call this to find windows directory
	*/
	public static native String getWindowsDir();
}
