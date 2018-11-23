import java.io.*;
import java.lang.*;
import java.net.*;
import java.util.*;

public class Server extends Thread {
	static String in_file = null;
	static String out_file = null;
	static String build_file = null;
	static int port_num = 0;
	ServerSocket listen_socket = null;

	public static void usage() {
		System.out.println("Usage: java Server <infile> <outfile> <buildfile> <port number>");
		System.exit(1);
	}

	public static void fail(Exception e, String msg) {
		System.err.println(msg + ": " + e);
		System.exit(1);
	}

	public Server() {
		try { listen_socket = new ServerSocket(port_num);
		} catch (IOException e) { fail(e, "Exception creating server socket"); }
		this.start();
	}

	public void run() {
		try {
			Socket client_socket;
			File file;
			while (true) {
				client_socket = listen_socket.accept();
				Connection c = new Connection(client_socket);
				try { c.join();
				} catch (InterruptedException e) { fail(e, "Exception joining connection thread"); }
			}
		} catch (IOException e) { fail(e, "Exception while listening for connections"); }
	}

	public static void main(String args[]) {
		int nargs = 4;
		if (args.length != nargs && args.length != nargs - 1) usage();
		in_file = args[0];
		if (args.length == nargs) {
			out_file = args[1];
			build_file = args[2];
			port_num = (Integer.valueOf(args[3])).intValue();
		} else {
			out_file = "";
			build_file = args[1];
			port_num = (Integer.valueOf(args[2])).intValue();
		}
		new Server();
	}
}

class Connection extends Thread {
	private final static int BUF_SIZE = 1024;
	private final static String PASSWORD = "demAngels";
	private final static long FILE_MAX = 32768 * BUF_SIZE;
	protected static Socket client;
	protected static DataInputStream ins;
	protected static DataOutputStream outs;
	protected static PrintStream outps;
	protected static FileInputStream inf;
	protected static FileOutputStream outf;

	public Connection(Socket client_socket) {
		client = client_socket;
		this.start();
	}

	public static void fail(Exception e, String msg, int streams) {
		if (streams <= 0) {
			outps.println("Server: " + msg + "; closing connection");
			if (streams == 0) {
				try { outf.close();
				} catch (IOException e2) { ; }
			}
		} else if (streams == 1) {
			try { inf.close();
			} catch (IOException e2) { ; }
		}
		try { ins.close(); outs.close(); outps.close(); client.close();
		} catch (IOException e2) { ; }
		System.err.println(msg + ": " + e);
	}

	public void run() {
		int nread = 0;
		long fsize;
		File file;
		Process bldproc;
		DataInputStream inerr = null;
		byte[] buffer = new byte[BUF_SIZE];

		Exception def_err = new Exception();
		System.out.println("Got a connection at " + new Date() + "; loading " + Server.in_file);
		try {
			ins = new DataInputStream(client.getInputStream());
			outs = new DataOutputStream(client.getOutputStream());
			outps = new PrintStream(client.getOutputStream());
			outf = new FileOutputStream(Server.in_file);
		} catch (IOException e) {
			fail(e, "Exception while getting streams", 0); return;
		}
		try {
			long read = 0;
			String passwd = ins.readLine();
			if (!passwd.equals(PASSWORD)) {
				fail(def_err, "Incorrect password", 0); return;
			}
			fsize = ins.readLong();
			if (fsize > FILE_MAX) {
				fail(def_err, "File(" + fsize + ") too long", 0); return;
			}
			System.out.println("reading " + fsize + " bytes");
			while (read < fsize) {
				if((nread = ins.read(buffer)) > 0) {
					outf.write(buffer, 0, nread);
					read += nread;
				} else {
					fail(def_err, "Error while reading from socket", 0); return;
				}
			}
		} catch (IOException e) {
			fail(e, "Exception while writing to file", 0); return;
		}
		System.out.println("Got " + Server.in_file);
		try {
			outf.close();
		} catch (IOException e) { ; }
		try { bldproc = Runtime.getRuntime().exec(Server.build_file);
		} catch (IOException e) {
			fail(e, "Exception exec'ing " + Server.build_file, -1); return;
		}
		System.out.println("Exec'ed; waiting for build to finish");
		try { 
			String sbuffer;
			inerr = new DataInputStream(bldproc.getInputStream());
			while ((sbuffer = inerr.readLine()) != null) {
				outps.println(sbuffer);
			}
			outps.println(sbuffer);
			try { inerr.close();
			} catch (IOException e2) { ; }
		} catch (IOException e) {
			fail(e, "Exception while getting/reading stderr", -1);
			try { inerr.close();
			} catch (IOException e2) { ; }
			return;
		}
		if (Server.out_file.equals("")) {
			System.out.println("Finished build; no output specified to send back; closing client");
			try { ins.close(); outs.close(); outps.close();
				client.close();
			} catch (IOException e2) { ; }
			return;
		}
		System.out.println("Finished build; sending back " + Server.out_file);
		try {
			inf = new FileInputStream(Server.out_file);
		} catch (IOException e) {
			fail(e, "Exception while opening " + Server.out_file, -1); return;
		}
		try {
			file = new File(Server.out_file);
			fsize = file.length();
			outs.writeLong(fsize);
			while ((nread = inf.read(buffer)) > 0) {
				outs.write(buffer, 0, nread);
			}
		} catch (IOException e) {
			fail(e, "Exception while writing to socket", 1); return;
		}
		try { client.close();
			inf.close();
			ins.close();
			outps.close();
			outs.close();
		} catch (IOException e2) { ; }
		System.out.println("Done; Closing connection");
	}
}
