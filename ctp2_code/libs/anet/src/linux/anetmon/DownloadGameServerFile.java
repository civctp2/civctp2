import java.io.*;
import java.net.*;
import java.text.*;
import java.util.*;

public class DownloadGameServerFile extends Thread {
	private static String serverFile = "file://servers.txt";
	private static String tempfilePrefix = "";
	private static String downloadFile = null;

	public DownloadGameServerFile() {
		// set default for URL access
		URLConnection.setDefaultAllowUserInteraction(false);

		setStatus("searching...");

		this.start();
	}

	public static void main(String args[]) {
		if (args.length > 0) {
			int i;
			for (i = 0; i < args.length; i++) {
				if (args[i].charAt(0) == '-') {
					switch (args[i].charAt(1)) {
						case 's':
							serverFile = args[i+1];
							i++;
							break;
						case 'f':
							downloadFile = args[i+1];
							i++;
							break;
						case 'p':
							tempfilePrefix = args[i+1];
							i++;
							break;
						default:
							setStatus("error: unrecognized flag " + args[i]);
							printUsage();
							break;
					}
				} else {
					setStatus("error: unrecognized argument " + args[i]);
					printUsage();
				}
			}
		}
		if (downloadFile == null) {
			setStatus("need to specify download file");
			printUsage();
		}
		new DownloadGameServerFile();
	}
	
	private void GetURLS(Vector vServers) {
		String strURL;

		try {
			URL url = new URL(serverFile);
			URLConnection urlConnection = url.openConnection();
			urlConnection.setAllowUserInteraction(false);
			InputStream urlStream = url.openStream();
			BufferedReader in = new BufferedReader(new InputStreamReader(urlStream)); //new FileReader(serverFile));

			while ((strURL = in.readLine()) != null) {
				String strbuf;
				System.out.println("Got line " + strURL);
				strURL = strURL.trim();
				int i = strURL.indexOf(' ');
				if ((i == -1) ||
					((strbuf = strURL.substring(0, i)).length() == 0)) {
					setStatus("err: servers.dat has incorrect entry" + strURL);
					continue;
				}
				vServers.addElement(strbuf);
			}
		} catch (IOException e) {
			setStatus("exception " + e + " thrown while getting server list");
			System.exit(1);
		}
	}

	public void run() {
		// list of servers to download from
		Vector vServers = new Vector();
		GetURLS(vServers);
		int numberDownloaded = 0;

		for (Enumeration eServers = vServers.elements();
			eServers.hasMoreElements() ; ) {
			// get the next element from the servers list
			String strServer = (String) eServers.nextElement();
			String strURL = "http://" + strServer + "/" + 
							    downloadFile;
			String strExten;
			int dot;
			if ((dot = downloadFile.lastIndexOf(".")) >= 0)
				strExten = downloadFile.substring(dot);
			else
				strExten = "";

			setStatus("searching " + strURL);

			URL url;
			try { 
				url = new URL(strURL);
			} catch (MalformedURLException e) {
				setStatus("err: invalid URL " + strURL);
				break;
			}

			// can only search http: protocol URLs
			if (url.getProtocol().compareTo("http") != 0) 
				break;

			try {
				// try opening the URL
				URLConnection urlConnection = url.openConnection();
				urlConnection.setAllowUserInteraction(false);

				InputStream urlStream = url.openStream();
				FileOutputStream fileStream =
					new FileOutputStream("/tmp/" + tempfilePrefix + numberDownloaded + strExten);

				// search the input stream for links
				// first, read in the entire URL
				byte b[] = new byte[1000];
				int numRead;
				while ((numRead = urlStream.read(b)) != -1)
					fileStream.write(b, 0, numRead);
				urlStream.close();
				fileStream.close();
				numberDownloaded++;

			} catch (IOException e) {
				setStatus("couldn't open URL " + strURL);
			}
		}

		setStatus("done; searched " + vServers.size() + " sites; got "
					+ numberDownloaded + " logs");
	}

	private static void setStatus(String status) {
		System.out.println(status);
	}

	private static void printUsage() {
		System.out.println("Usage: java [-classpath class_path] DownloadLogs [-s servers_file] [-f download_file]");
		System.exit(1);
	}
}
