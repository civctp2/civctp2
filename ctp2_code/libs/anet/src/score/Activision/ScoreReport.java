package Activision;

public interface ScoreReport
{
	// These flags must mirror those defined in scorerep.h
	public static final int FLAGS_SELFEXIT = 1;
	public static final int FLAGS_LAUNCHED = 2;

	// Adds a player to this report using the data given.
	// This is called nPlayersReported times by the JNI caller after
	// startReport(), once per player in the report, before the report
	// is considered complete and is sent to
	// ScoreSessionHandler.handleScoreReport().
	// Warning: the arguments passed in are reused by the JNI caller.
	// Sub-classes should not keep references to them around.
	// Use bloblen rather than blob.length.
	public void addPlayer(short ID, int UID, byte blob[], short bloblen);
}
