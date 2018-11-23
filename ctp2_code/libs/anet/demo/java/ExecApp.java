import java.util.*;
import java.io.*;

import Activision.*;
//package Activision;

// Uses helper program exec.exe to change directories before running game.
public class ExecApp {

	public static void launchGame(ANet.InstalledApp app) {
		String cmd = "exec.exe " + app.getCwd() + " " + app.getPath() + " " + app.getArgs();
		//System.out.println("Running " + cmd);

		try {
			Runtime.getRuntime().exec(cmd);
			System.exit(0);
		} catch (IOException e) {
			System.out.println("Error running " + cmd);
		}
	}
}
