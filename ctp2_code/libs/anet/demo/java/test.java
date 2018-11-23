import Activision.*;
import java.util.*;
import java.io.*;

class test {
	ANet.ANetTransport[] transports;
	ANet.ANetServer servs[];
	ANet.Player players[];
	Vector apps;		// ANet.InstalledApp[]
	public ANet dp;
	boolean done;
	boolean tabset;		// set in ANet.ANetTableListener if table var is set
	static final int NONE = 0;
	static final int INET = 1;
	static final int NMODM = 2;
	static final int MODM = 3;
	static final int use_kind = INET;		// set to desired transport type
	// SESSIONTYPE non-zero expects table to exists somewhere else --
	// this is used for testing addPublisher, etc.
	static final int SESSIONTYPE = 30;
	static final byte dp_KEY_MYTAB = 0x50;

	ANet.Player the_player;
	ANet.Session the_session;

	/* Pick the first installed app */
    public ANet.InstalledApp pick_app()
	{
		apps = dp.enumInstalledApps();
		if (apps == null) {
			System.out.println("No apps to choose from.");
			return null;
		}
		if (apps.size() == 0) {
			System.out.println("Zero apps to choose from.");
			return null;
		}
		System.out.println("Picking app from:");
		Enumeration e = apps.elements();
		while(e.hasMoreElements()) {
			ANet.InstalledApp app = (ANet.InstalledApp)e.nextElement();
			System.out.println("  * " + app.getName() 
			+ " type:" + app.getSessionType() 
			+ " lang:" + app.getLanguage()
			+ " ver:" + app.getCurrentVersionMajor() + "." + app.getCurrentVersionMinor()
			+ " latest:" + app.getLatestVersionMajor() + "." + app.getLatestVersionMinor()
			);
		}
		return (ANet.InstalledApp) apps.elementAt(0);
	}

	private int getKind(ANet.ANetTransport t)
	{
		if ((t.getCapabilities() & ANet.ANetTransport.DRIVER_NO_BROADCAST) != 0)
			return INET;
		if ((t.getNeeds() & ANet.ANetTransport.DRIVER_NEEDS_PORTNUM) != 0) {
			if ((t.getNeeds() & ANet.ANetTransport.DRIVER_NEEDS_PHONENUM) != 0)
				return MODM;
			else
				return NMODM;
		}
		return NONE;
	}

	/* Return the driver specified by use_kind */
    public ANet.ANetTransport pick_transport()
	{
		int i;
		int len;
		int itrans;

		itrans = -1;
		transports = ANet.enumTransports();
		if (transports == null) {
			System.out.println("transports is null.");
		} else {
			len = transports.length;
			System.out.println("Got len = " + (new Integer(len)).toString());
			for (i=0; i<len; i++) {
				ANet.ANetTransport cur = transports[i];
				if (cur == null) {
					System.out.println("Null entry!");
				} else {
					System.out.println(
					  "name: " + cur.getName()
					+ " path: " + cur.getPath()
					+ " needs: " + Integer.toHexString(cur.getNeeds()) 
					+ " cap: " + Integer.toHexString(cur.getCapabilities()));
					if (use_kind == NONE || getKind(cur) == use_kind) {
						itrans = i;
						System.out.println("Picking " + cur.getName());
					}
				}
			}
		}
		if (itrans == -1)
			return null;
		return transports[itrans];
	}

	/** enumerate the available servers and pick the fastest one 
	 */
	public ANet.ANetServer pick_server()
	{
		int i;
		int len, fastest;

		System.out.println("Created; pinging servers.");
		dp.pingServers();
		System.out.println("Pinged....");
		spin_one_sec();
		System.out.println("Calling enumServers....");
		servs = dp.enumServers();
		System.out.println("Done with enumServers....");
		if (servs == null)
			return null;

		len = servs.length;
		System.out.println("Got len = " + (new Integer(len)).toString());
		fastest = -1;
		for (i=0; i<len; i++) {
			ANet.ANetServer cur = servs[i];
			if (cur != null) {
				System.out.println(
				  "name: " + cur.getHostname()
				+ " ms: " + Integer.toString(cur.getRtt_ms_avg())
				+ " loss: " + Integer.toString(cur.getLoss_percent()) + "%");
				if ((fastest < 0) 
				|| (cur.getRtt_ms_avg() < servs[fastest].getRtt_ms_avg()))
					fastest = i;
				if (SESSIONTYPE != 0) {
					if ((cur.getHostname()).startsWith("california3")) {
						fastest = i;
						break;
					}
				}
			}
		}
		if (fastest < 0)
			return null;
		return servs[fastest];
    }

	/** enumerate the available sessions and pick the first one.
	 * If lobby is true, pick the first one with the low bit of dwUser1 set.
	 */
	public ANet.Session pick_session(boolean lobby)
	{
		the_session = null;
		while (null == the_session) {
			long start = System.currentTimeMillis();
			while (true) {
				handle_pkts();
				long now = System.currentTimeMillis();
				if ((now - start) > 1200)
					break;
			}
			System.out.println("Calling enumSessions");
			ANet.Session sessions[] = dp.enumSessions(0);
			if (sessions == null)
				continue;

			int len = sessions.length;
			if (len == 0)
				continue;

			System.out.println("Got len = " + (new Integer(len)).toString());
			int j;
			ANet.Session thelobby = null;
			for (j=0; j<len; j++) {
				ANet.Session cur = sessions[j];
				if (cur == null) {
					System.out.println("Null entry!");
				} else {
					System.out.println(
					  "name: " + cur.getName()
					+ " cur: " + Integer.toString(cur.getCurPlayers())
					+ " max: " + Integer.toString(cur.getMaxPlayers())
					+ " dw1: " + Integer.toString(cur.getDwUser1())
					+ " type: " + Integer.toString(cur.getType()));
					if (((cur.getDwUser1() & 1) != 0)
					&& (cur.getCurPlayers() > 0))
						thelobby = cur;
					if (cur.getType() == SESSIONTYPE) {
						thelobby = cur;
						break;
					}
				}
			}
			if (lobby)
				the_session = thelobby;
			else
				the_session = sessions[0];
		}
		return the_session;
	}

	/** Join the given session.  Returns true on success.
	 */
	public boolean join_session(ANet.Session the_sess)
	{
		/* Join the first session that comes to mind */
		done = false;
		int err = dp.openSession(the_sess, false, 
			new ANetSessionListener() {
				public void sessionOpened(ANet.Session s)
				{ 
					if (s == null)
						System.out.println("sessionOpen Failed");
					else
						System.out.println("sessionOpened");
					done = true;
					the_session = s;
				}
				public void sessionChanged(ANet.Session s)
				{ System.out.println("sessionChanged"); }
				public void sessionClosed(ANet.Session s)
				{ System.out.println("sessionClosed"); }
			});
		if (err != ANetResult.OK) {
			System.out.println("Can't open session.  Error " +
			Integer.toString(err));
			return false;
		}
		System.out.println("Waiting for callback");
		long start = System.currentTimeMillis();
		while (true && !done) {
			handle_pkts();
			long now = System.currentTimeMillis();
			if ((now - start) > 2200)
				break;
		}
		System.out.println("Done waiting");

		return done && (the_session != null);
	}

	/** Create a session.  Returns true on success.
	 */
	public boolean create_session()
	{

		/* Join the first session that comes to mind */
		done = false;
		int err = dp.openSession(the_session, true, 
			new ANetSessionListener() {
				public void sessionOpened(ANet.Session s)
				{ 
					if (s == null)
						System.out.println("sessionCreate Failed");
					else
						System.out.println("sessionCreated");
					done = true;
					the_session = s;
				}
				public void sessionChanged(ANet.Session s)
				{ System.out.println("sessionChanged"); }
				public void sessionClosed(ANet.Session s)
				{ System.out.println("sessionClosed"); }
			});
		if (err != ANetResult.OK) {
			System.out.println("Can't create session.  Error " +
			Integer.toString(err));
			return false;
		}
		System.out.println("Waiting for callback");
		long start = System.currentTimeMillis();
		while (true && !done) {
			handle_pkts();
			long now = System.currentTimeMillis();
			if ((now - start) > 2200)
				break;
		}
		System.out.println("Done waiting");

		return done && (the_session != null);
	}

	/** List the players in the current session.
	 */
	public void list_players()
	{
		/* Now list the players in the session */
		System.out.println("Calling enumPlayers");
		players = dp.enumPlayers();
		System.out.println("enumPlayers returned");
		if (players == null) {
			System.out.println("No players?");
			return;
		}
		int len = players.length;
		System.out.println("Got len = " + len); //(Integer.toString(len));
		int j;
		for (j=0; j<len; j++) {
			ANet.Player cur = players[j];
			if (cur == null) {
				System.out.println("Null entry!");
			} else {
				System.out.println(
				  "name: " + cur.getName()
				+ "  id: " + cur.getId()
				+ " local: " + cur.getLocal()
				+ " lag: " + cur.getLatency());
			}
		}
	}

	public void handle_pkts()
	{
		ANet.Packet[] pkts;
		pkts = dp.receivePackets();
		if (pkts != null) {
			int i;
			for (i=0; i<pkts.length; i++) {
				ANet.Packet p = pkts[i];
				String body = " (empty)";
				if (p.getBody().length > 1) {
					body = " body" + p.getBody()[0] + "," + p.getBody()[1];
				}
				System.out.println("pkt src:"+p.getSrc()+" dest:"+p.getDest()+" len:"+p.getBody().length + " err:"+p.getErr() + body);
			}
		}
	}

	/* An evil routine.  Only useful in test code. */
	public void spin_one_sec()
	{
		long start = System.currentTimeMillis();
		while (true) {
			handle_pkts();
			long now = System.currentTimeMillis();
			if ((now - start) > 1000) {
				return;
			}
		}
	}

	/** Add a player listener in the current session.
	 */
	public void listen_for_players()
	{
		int err;
		ANetPlayerListener listener = new ANetPlayerListener() {
				public void playerEnters(ANet.Session s, ANet.Player p)
				{ 
					if (p == null)
						System.out.println("CreatePlayer Failed");
					else {
						System.out.println("Player " + p.getName() + " Enters");
						the_player = p;
					}
					done = true;
				}
				public void playerChanged(ANet.Session s, ANet.Player p)
				{ System.out.println("player Changed"); }
				public void playerLeaves(ANet.Session s, ANet.Player p)
				{ System.out.println("player Closed"); }
			};
		the_session.addPlayerListener(listener);
	}

	/* Print a table key to string */
	private static String key2str(byte[] key)
	{
		int i;
		String str = "";
		for (i = 0; i < key.length - 1; i++)
			str = str + ((int)((key[i])&0xff)) + ".";
		str = str + ((int)((key[i])&0xff));
		return str;
	}

	/** Add a table variable listener in the current session.
	 */
	public void listen_for_table_vars(ANet.ANetTable tab)
	{
		int err;
		tabset = false;
		ANetTableListener listener = new ANetTableListener() {
			public void tableVarSet(ANet.ANetTable t, byte[] subkey) {
				System.out.println("table variable key:" + key2str(t.getKey()) + " subkey:" + key2str(subkey) + " set");
				tabset = true;
			}
			public void tableVarDeleted(ANet.ANetTable t, byte[] subkey)
			{	System.out.println("table variable key:" + key2str(t.getKey()) + " subkey:" + key2str(subkey) + " deleted"); }
			public void tableVarSent(ANet.ANetTable t, byte[] subkey)
			{	System.out.println("table variable key:" + key2str(t.getKey()) + " subkey:" + key2str(subkey) + " sent"); }
		};
		tab.addTableListener(listener);
	}

	/** Create and destroy a player in the current session.
	 */
	public void toy_with_players()
	{
		int err;
		System.out.println("Calling createPlayer(Doughboy)");
		done = false;
		err = the_session.createPlayer("Doughboy");
		if (err != ANetResult.OK) {
			System.out.println("Can't create player.  Error " +
			Integer.toString(err));
		}
		System.out.println("createPlayer(Doughboy) returns.");

		long start = System.currentTimeMillis();
		while (!done) {
			spin_one_sec();
			list_players();
			long now = System.currentTimeMillis();
			if ((now - start) > 5000) {
				System.out.println("Can't create player.");
				break;
			}
		}

		if (the_player == null) {
			System.out.println("No player, so not calling Doughboy.destroyPlayer()");
			return;
		}
		/* Send a messages from this player to the world */
		ByteArrayOutputStream pktbuf = new ByteArrayOutputStream(256);
		DataOutputStream pkt = new DataOutputStream(pktbuf);
		try {
			pkt.writeUTF("CHHello, World!");
		} catch (Throwable t) {
			System.out.println("Can't write string to buf");
		}
			
		// 0 = everybody, 1 = reliable
		dp.sendPacket(the_player.getId(), 0, 1, pktbuf.toByteArray(), 2);

		ANet.ANetTable mytab = null;
		int pIdHost = the_player.getId();
		if (true) {
			// try creating a new table and write/read from it
			if (SESSIONTYPE != 0) {
				byte[] key = {(byte)(SESSIONTYPE&0xff), (byte)((SESSIONTYPE>>8)&0xff), (byte)1};
				System.out.println("setting table key to " + key2str(key));
				try {
					mytab = new ANet.ANetTable(key, true);
				} catch (ANet.ANetTable.createException e) {
					System.out.println("Couldn't create table; quitting");
					return;
				}
				int i;
				for (i = 0; i < players.length; i++)
					 if (players[i].getId() < pIdHost)
					 	pIdHost = players[i].getId();
				mytab.addPublisher(mytab.getKey(), pIdHost);
				mytab.requestSubscription(mytab.getKey(), pIdHost);
				listen_for_table_vars(mytab);
				int karma = the_session.getKarma();
				byte[] subkey = {(byte)(karma&0xff), (byte)((karma>>8)&0xff)};
				while (mytab.getValByKey(subkey) == null) {
					spin_one_sec();
					long now = System.currentTimeMillis();
					if ((now - start) > 5000) {
						System.out.println("Can't get table record.");
						break;
					}
				}
				if (!tabset) {
					System.out.println("err:table var set callback not called");
					return;
				}
			} else {
				byte[] key = {dp_KEY_MYTAB};
				byte[] subkey = {1};
				byte[] buf = {2};
				byte[] chkbuf = {0};
				byte[] chkbuf1 = {0};
				try {
					mytab = new ANet.ANetTable(key, true);
				} catch (ANet.ANetTable.createException e) {
					System.out.println("Couldn't create table; quitting");
					return;
				}
				listen_for_table_vars(mytab);
				mytab.setVal(subkey, buf, 2);
				if (mytab.getTableSize() != 1) {
					System.out.println("Couldn't set table value; quitting");
					return;
				}
				chkbuf = (byte[])mytab.getValByKey(subkey);
				if (chkbuf != null) {
					if (chkbuf.length != 1 || chkbuf[0] != buf[0]) {
						System.out.println("Couldn't get table value by subkey; quitting");
						return;
					}
				}
				chkbuf = (byte[])mytab.getValByIndex(0, chkbuf1);
				if (chkbuf != null && chkbuf1 != null) {
					if (chkbuf.length != 1 || chkbuf[0] != buf[0] ||
						chkbuf1.length != 1 || chkbuf1[0] != subkey[0]) {
						System.out.println("Couldn't get table value by index; quitting");
						return;
					}
				}
				if (!tabset) {
					System.out.println("err:table var set callback not called");
					return;
				}
				System.out.println("Table succesfully created and written/read");
			}
		}

		if (SESSIONTYPE == 0) {
			// Now, if you're brave, pick an app at random and launch it.
			int status;
			ANet.InstalledApp the_app;
			the_app = pick_app();
			if (the_app == null) {
				System.out.println("Can't pick app.  Exiting.");
				return;
			}
			System.out.println("Picked app " + the_app.getName());
			System.out.println("shellOpts:" + the_app.getShellOpts());

			String path = the_app.getCwd() + "/freeze.dat";
			System.out.println("Freezing to " + path);
			err = ANetResult.BUSY;
			while (err != ANetResult.OK) {
				err = dp.writeObject(path);
				if (err != ANetResult.OK && err != ANetResult.BUSY) {
					System.out.println("Error " + err + " writing freeze data.");
					return;
				}
				spin_one_sec();
			}

			System.out.println("Launching...");
			//ExecApp.launchGame(the_app);
			status = the_app.execWithConsole();
			System.out.println("Exec returns " + status);
			System.exit(0);
		}


		System.out.println("Calling Doughboy.destroyPlayer()");
		err = the_player.destroyPlayer();
		if (err != ANetResult.OK) {
			System.out.println("Can't destroy player.  Error " +
			Integer.toString(err));
		}
		start = System.currentTimeMillis();
		while (true) {
			handle_pkts();
			long now = System.currentTimeMillis();
			if ((now - start) > 1200)
				break;
		}
		if (SESSIONTYPE != 0)
			mytab.deletePublisher(mytab.getKey(), pIdHost);
	}

    public static void test_all(test t) {
		ANet.ANetTransport the_transport;
		ANet.ANetServer the_server;

		the_transport = t.pick_transport();
		if (the_transport == null) {
			System.out.println("Can't pick transport.  Exiting.");
			return;
		}
		System.out.println("Creating....");

		int err = 0;
 		if (use_kind == NMODM) {
			ANet.commInitReq params = new ANet.commInitReq();
			params.setSessionId(5678);
			params.setPortnum(1);
			params.setBaud(0);
			params.setDialingMethod(0);
			params.setPhonenum("");
			params.setModeministr("");
			err = t.dp.create(the_transport, params, null);
		} else if (use_kind == MODM) {
			ANet.commInitReq params = new ANet.commInitReq();
			params.setSessionId(5678);
			params.setPortnum(1);
			params.setBaud(19200);
			params.setDialingMethod(1);
			params.setPhonenum("2137");
			params.setModeministr("");
			err = t.dp.create(the_transport, params, null);
		} else {
			err = t.dp.create(the_transport, null, null);
		}
		if (err != ANetResult.OK) {
			System.out.println("Can't create, error " + err);
			System.exit(1);
		}

		t.pick_app();

 		if (use_kind == INET) {
			the_server = t.pick_server();
			if (the_server == null) {
				System.out.println("Can't pick server.  Exiting.");
				return;
			}
			System.out.println("Picked server " + the_server.getHostname());

			err = t.dp.setGameServer(the_server);
			if (err != ANetResult.OK) {
				System.out.println("Can't set server.  Exiting.");
				return;
			}
			System.out.println("Set server " + the_server.getHostname());
			System.out.println("Doing a few enumServers after connecting to server.");
			int i;
			for (i=0; i<3; i++) {
				t.pick_server();
			}
		}

		t.pick_session(true);
		if (t.the_session == null) {
			System.out.println("Can't pick session.  Exiting.");
			return;
		}
		System.out.println("Picked session " + t.the_session.getName());
		System.out.println("readyToFreeze returns " + t.dp.readyToFreeze());

		t.listen_for_players();

		if (!t.join_session(t.the_session)) {
			System.out.println("Can't join session.  Exiting.");
			return;
		}
		System.out.println("Joined session " + t.the_session.getName());

		t.list_players();

		t.toy_with_players();

		/* Pull network cable.  Sit here until session times out. */
/*
		int i;
		for (i=0; i<60; i++) {
			t.spin_one_sec();
		}
*/

		System.out.println("Calling closeSessions");
		err = t.dp.closeSession();
		if (err != ANetResult.OK) {
			System.out.println("Can't close session.  Error " +
			Integer.toString(err));
		}

		t.dp.destroy(1);		/* don't hang up */
		System.out.println("readyToFreeze after destroy returns " + t.dp.readyToFreeze());
		System.out.println("test_all done.");
    }

    public static void test_host(test t) {
		ANet.ANetTransport the_transport;
		ANet.ANetServer the_server;

		the_transport = t.pick_transport();
		if (the_transport == null) {
			System.out.println("Can't pick transport.  Exiting.");
			return;
		}
		System.out.println("Creating....");

		int err;
 		if (use_kind == NMODM) {
			ANet.commInitReq params = new ANet.commInitReq();
			params.setSessionId(5678);
			params.setPortnum(1);
			params.setBaud(0);
			params.setDialingMethod(0);
			params.setPhonenum("");
			params.setModeministr("");
			err = t.dp.create(the_transport, params, null);
		} else {
			err = t.dp.create(the_transport, null, null);
		}
		if (err != ANetResult.OK) {
			System.out.println("Can't create, error "+err);
			System.exit(1);
		}

 		if (use_kind == INET) {
			the_server = t.pick_server();
			if (the_server == null) {
				System.out.println("Can't pick server.  Exiting.");
				return;
			}
			System.out.println("Picked server " + the_server.getHostname());

			err = t.dp.setGameServer(the_server);
			if (err != ANetResult.OK) {
				System.out.println("Can't set server.  Exiting.");
				return;
			}
			System.out.println("Set server " + the_server.getHostname());
		}


		// Kludge: wait for a session to appear - this should
		// ensure that the game server connection finishes coming up
		// (create_session will fail if connection not up yet).
		t.pick_session(false);

		if (SESSIONTYPE != 0)
			t.the_session = new ANet.Session("demo session", "", SESSIONTYPE, 8, 0);
		else
			t.the_session = new ANet.Session("demo session", "", 1234, 8, 0);
		t.listen_for_players();
		if (!t.create_session()) {
			System.out.println("Can't create session.  Exiting.");
			return;
		}

		System.out.println("Created session " + t.the_session.getName());

		t.list_players();

		t.toy_with_players();

		System.out.println("Calling closeSessions");
		err = t.dp.closeSession();
		if (err != ANetResult.OK) {
			System.out.println("Can't close session.  Error " +
			Integer.toString(err));
		}

		t.dp.destroy(1);		/* don't hang up */
		System.out.println("test_host done.");
    }

	// Simple standalone test of execWithConsole().
	public static void test_launch()
	{
			int status;
			ANet.InstalledApp the_app = new ANet.InstalledApp();
			
			the_app.setPath("c:\\windows\\notepad.exe");
			the_app.setCwd("c:\\temp");
			the_app.setArgs("");
			the_app.setShellOpts("");
			the_app.setSessionType(0);
			System.out.println("Picked app " + the_app.getPath());
			System.out.println("Launching...");
			status = the_app.execWithConsole();
			System.out.println("Exec returns " + status);
	}
	/* Automatically bring in the native method on startup. */
    //static {
		//try {
			//System.loadLibrary("dll/ANetj");
		//} catch (UnsatisfiedLinkError e) {
			//System.loadLibrary("ANetj");
		//}
    //}

    public static void main(String[] args) {
		//test_launch();
		//System.exit(0);

		/*try {
			System.loadLibrary("updatej");
		} catch (UnsatisfiedLinkError e) {
			System.out.println("Can't load updatej");
		}*/

		test t = new test();
		t.dp = new ANet();

		int i;
		for (i=0; i<1; i++) {
			System.out.println("rep " + i);
			test_all(t);
			//test_host(t);
		}


	}
}
