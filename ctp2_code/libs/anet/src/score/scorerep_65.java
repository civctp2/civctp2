
public class scorerep_65 implements Activision.ScoreReport
{
	public static class player {
		public short id;
		public int uid;
		// Blob fields:
		public short score;
		public byte won;

		public player(short ID, int UID, short Score, byte Won)
		{
			id = ID;
			uid = UID;
			score = Score;
			won = Won;
		}

		public player(short ID, int UID)
		{
			this(ID, UID, (short)0, (byte)0);
		}

		public player()
		{
			this((short)0, (int)0, (short)0, (byte)0);
		}

		public void print()
		{
			System.out.println(" id:"+ id +" uid:"+ uid +
				" score:"+ score +" won:"+ (int)won);
		}
	}

	public short flags;
	public short id;
	public int uid;
	public short leaverid;
	public int leaveruid;
	public player players[];

	private int nPlayersAdded;

	public scorerep_65(short Flags, short ReporterID, int ReporterUID,
		short LeaverID, int LeaverUID, int nReported)
	{
		flags = Flags;
		id = ReporterID;
		uid = ReporterUID;
		leaverid = LeaverID;
		leaveruid = LeaverUID;
		players = new player[nReported];

		nPlayersAdded = 0;
	}

	// Adds a player to this report using the data given.
	// This is called nPlayersReported times by the JNI caller after
	// startReport(), once per player in the report, before the report
	// is considered complete and is sent to
	// ScoreSessionHandler.handleScoreReport().
	// Warning: the arguments passed in are reused by the JNI caller.
	// Sub-classes should not keep references to them around.
	// Use bloblen rather than blob.length.
	public void addPlayer(short ID, int UID, byte blob[], short bloblen)
	{
		if (players == null) {
			System.out.println("scorerep_65[Rid:"+ id +", Lid:"+ leaverid +
				"].addPlayer(id:"+ ID +"): players null");
			return;  // throw NullPointerException ?
		}
		if (nPlayersAdded == players.length) {
			System.out.println("scorerep_65[Rid:"+ id +", Lid:"+ leaverid +
				"].addPlayer(id:"+ ID +"): already have "+ players.length +
				" players");
			return;  // throw TooManyPlayersException ?
		}
		if (bloblen < 3) {
			System.out.println("scorerep_65[Rid:"+ id +", Lid:"+ leaverid +
				"].addPlayer(id:"+ ID +"): invalid bloblen "+ bloblen);
			return;  // throw InvalidBlobException ?
		}
		// For more complicated blobs, it might make sense to read using a 
		// new DataInputStream(new ByteArrayInputStream(blob, 0, bloblen));
		players[nPlayersAdded] = new player(ID, UID,
			(short)((((short)blob[1] + 256) & 0xff) * 256 +
					(((short)blob[0] + 256) & 0xff)), blob[2]);
		nPlayersAdded++;
	}

	public void print()
	{
		System.out.println("scorerep_65 - flags:"+ flags +
			" id:"+ id +" uid:"+ uid +" leaverid:"+ leaverid +
			" leaveruid:"+ leaveruid +"\nPlayers:");
		if (players == null) {
			System.out.println("No players in scorerep!");
		} else {
			int i;
			for (i = 0; i < players.length; i++) {
				if (players[i] == null)
					System.out.println("players["+ i +"] is null!");
				else
					players[i].print();
			}
		}
		System.out.println(" --- ");
	}
}
