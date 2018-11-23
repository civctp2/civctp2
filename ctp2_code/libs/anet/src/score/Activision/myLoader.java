package Activision;

import java.util.*;
import java.util.zip.*;
import java.io.*;

public class myLoader extends ClassLoader {
	Hashtable cache = new Hashtable();

	public myLoader() {}

	private byte[] JarGetFile(String cname, String fname) {
		if (cname == null || fname == null)
			return null;
		ZipFile fjar;
		try {
			fjar = new ZipFile("servjar/" + cname + ".jar");
		} catch (Exception e) { 
			System.out.println("JarGetFile: new Zipfile exception "+
				e.getMessage());
			return null;
		}
		ZipEntry ze = fjar.getEntry(fname);
		if (ze == null) {
			try { fjar.close(); } catch (IOException e) { ; }
			System.out.println("JarGetFile: "+ fname +" not found");
			return null;
		}
		byte[] byteclass;
		InputStream in = null;
		try {
			in = fjar.getInputStream(ze);
			int csize = (int)ze.getSize();
			byteclass = new byte[csize];
			int cread = in.read(byteclass, 0, csize);
			if (cread != csize) {
				in.close(); fjar.close();
				System.out.println("JarGetFile: wrong size");
				return null;
			}
		} catch (Exception e) {
			System.out.println("JarGetFile: read exception "+ e.getMessage());
			try { in.close(); fjar.close(); } catch (IOException e2) { ; }
			return null;
		}
		try { in.close(); fjar.close(); } catch (IOException e) { ; }
		return byteclass;
	}

	public synchronized Class loadClass(String name, boolean resolve) 
		throws ClassNotFoundException
	{
		Class c = (Class)(cache.get(name));

		if (c == null) {
			if (name.startsWith("java.") || name.startsWith("javax.")
			||  name.startsWith("Activision."))
				return super.findSystemClass(name);

			// Expect something of form "my_class_<SessionType>"
			// or "my_class_<SessionType>$inner_class".
			// Look in jar file "score_<SessionType>.jar" first, if so.
			int index = name.indexOf('$');
			int index_;
			if (index == -1)
				index_ = name.lastIndexOf('_');
			else
				index_ = name.lastIndexOf('_', index);
			if (index_ == -1)
				return super.findSystemClass(name);

			String jarname;
			if (index == -1)
				jarname = new String("score_"+ name.substring(index_+1));
			else
				jarname = new String("score_"+ name.substring(index_+1, index));
			System.out.println("loadClass: "+ name +" from "+ jarname);
			byte[] byteClass = JarGetFile(jarname, name + ".class");
			if (byteClass == null) {
				throw new ClassNotFoundException();
			}
			c = defineClass(name, byteClass, 0, byteClass.length);
			if (c == null) {
				throw new ClassFormatError();
			}
			cache.put(name, c);
		}
		if (resolve)
			resolveClass(c);
		return c;
	}
}
