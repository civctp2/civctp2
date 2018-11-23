package Activision;

// Constants for result codes
// e.g. ANetResult.OK as a return value indicates the function succeeded.
// We should probably use typesafe constants here, a la 
// http://www.javaworld.com/javaworld/jw-07-1997/jw-07-enumerated.html
// but for now, static final ints will have to do.

public interface ANetResult {
	public static final int OK = 0;
	public static final int NOMEM = 1;
	public static final int EMPTY = 2;
	public static final int FULL  = 3;
	public static final int BADSIZE = 4;
	public static final int BAD = 5;
	public static final int BUSY = 6;
	public static final int ALREADY = 7;
	public static final int BUG = 8;
	public static final int MODEM_BUSY = 9;
	public static final int MODEM_NOANSWER = 10;
	public static final int MODEM_NODIALTONE = 11;
	public static final int MODEM_NORESPONSE = 12;
	public static final int NETWORK_NOT_PRESENT = 13;
	public static final int COMMDLL_BAD_VERSION = 14;
	public static final int NETWORK_NOT_RESPONDING = 15;
	public static final int HOST_NOT_RESPONDING = 16;
	public static final int OPENED = 17;
	public static final int CLOSED = 18;
	public static final int STARTED = 19;
	public static final int FINISHED = 20;
	public static final int DELETED = 21;
};

