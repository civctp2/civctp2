package Activision;

public class score_init {
	public static native void c_main(String argv[]);

	public static void main(String argv[])
	{
		System.out.println("Starting JNI\n");
		System.loadLibrary("jscore2");
		c_main(argv);
		System.out.println("JNI returns\n");
	}
}
