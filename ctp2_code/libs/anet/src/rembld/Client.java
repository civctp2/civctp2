import java.io.*;
import java.net.*;
import java.lang.*;

public class Client extends Thread{
	private static final String PASSWORD = "demAngels";
	public static final int BUF_SIZE = 1024;
	public static final int FILE_MAX = 8192 * BUF_SIZE;

	public static void usage() {
		System.out.println("Usage: java Client <hostname> <sendname> <receivename> <logname> <port number>");
		System.exit(0);
	}

	public static void main(String[] args) {
		int nread, nlogfile;
		long fsize;
		byte[] buffer = new byte[BUF_SIZE];
		Socket s = null;
		FileInputStream fin = null;
		DataInputStream sin = null;
		FileOutputStream fout = null;
		DataOutputStream sout = null;
		PrintStream soutp = null;
		if (args.length != 5) usage();
		try {
			s = new Socket(args[0], (Integer.valueOf(args[4])).intValue());
		} catch (Exception e) {
			System.out.println("Error opening new socket: " + e);
			System.exit(1);
		}
		System.out.println("opened sockets; now opening streams");
		try {
			sin = new DataInputStream(s.getInputStream());
			sout = new DataOutputStream(s.getOutputStream());
			soutp = new PrintStream(s.getOutputStream());
			fin = new FileInputStream(args[1]);
		} catch (IOException e) {
			try { 
				s.close();
				sout.close();
				sin.close();
				fin.close();
			} catch (Exception e2) { ; }
			System.out.println("Error opening new streams: " + e);
			System.exit(1);
		}
		System.out.println("opened streams; now sending " + args[1]);
		try {
			File file = new File(args[1]);
			soutp.println(PASSWORD);
			fsize = file.length();
			System.out.println("writing " + fsize + " bytes");
			sout.writeLong(fsize);
			while((nread = fin.read(buffer)) > 0) {
				sout.write(buffer, 0, nread);
			}
		} catch (IOException e) {
			String sbuffer;
			try {
				while ((sbuffer = sin.readLine()) != null) {
					System.out.println(sbuffer);
				}
			} catch (IOException e2) { ; }
			try { fin.close(); sin.close(); sout.close(); s.close();
			} catch (IOException e2) { ; }
			System.out.println("error writing to server: " + e);
			System.exit(1);
		}
		System.out.println("finished sending " + args[1]);
		try {
			fin.close();
		} catch (IOException e) { ; }
		{	PrintStream outpf = null;
			try {
				int i;
				String sbuffer;
				fout = new FileOutputStream(args[3]);
				outpf = new PrintStream(fout);
				while ((sbuffer = sin.readLine()) != null && !sbuffer.equals("null")) {
					outpf.println(sbuffer);
					outpf.flush();
					System.out.println(sbuffer);
				}
				outpf.close();
				System.out.println();
			} catch(IOException e) {
				try { outpf.close(); fout.close(); sin.close(); sout.close(); s.close();
				} catch (IOException e2) { ; }
				System.out.println("Exception while getting log: " + e);
				System.exit(1);
			}
		}
		try {
			fout = new FileOutputStream(args[2]);
		} catch (IOException e) {
			try { sin.close(); sout.close(); s.close();
			} catch (IOException e2) { ; }
			System.out.println("Exception while opening output file: " + e);
			System.exit(1);
		}
		try {
			long read = 0;
			fsize = sin.readLong();
			System.out.println("reading " + fsize + " bytes");
			if (fsize > FILE_MAX) {
				try { fout.close(); sin.close(); sout.close(); s.close();
				} catch (IOException e2) { ; }
				System.out.println(args[2] + " too large");
				System.exit(1);
			}
			while (read < fsize) {
				nread = sin.read(buffer);
				if (nread >= 0) {
					fout.write(buffer, 0, nread);
					read += nread;
				} else {
					try { fout.close(); sin.close(); sout.close(); s.close();
					} catch (IOException e2) { ; }
					System.out.println("error writing: wrote " + nread + " bytes to "+ args[2]);
					System.exit(1);
				}
			}
			if (read != fsize) {
				System.out.println("error: didn't get " + args[2] + "correctly");
			} else {
				System.out.println("all done");
			}
		} catch (IOException e) {
			try { sin.close(); sout.close(); s.close();
			} catch (IOException e2) { ; }
			System.out.println("Exception while writing to file: " + e);
			System.exit(1);
		}
		try {
			fout.close();
			sin.close();
			sout.close();
			s.close();
		} catch (IOException e) { ; }
	}
}
