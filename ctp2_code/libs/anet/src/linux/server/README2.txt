*** Anet Game Server Software ***
Last update: 5 October 1998

*** Overview ***
This is a beta version of the Anet Game Server.  This server is a
place for players of Dark Reign, Battlezone, Heavy Gear, Zork, Shanghai, 
and other games to organize and play multi-player Internet 
games.

This software requires Red Hat Linux 5.2 or later, 32 MB or more RAM, 
and a Pentium 200 or faster CPU.
The max number of users that can be supported depends on processor speed.
With 80 BogoMips (e.g. a Pentium 200; see the Linux bootup message),
you can support about 250 very active users well.
A Pentium 2/350 has about 350 BogoMips, so it should be able to handle
roughly a four times heavier load.

The max number of users also depends on bandwidth.  128kilobits/second 
is sufficient for light duty, but the server should not be used with
anything slower than a T1 connection.  It can happily share the T1 with
other services.

You should only run one copy of this game server, as users tend to congregate
at a single server.
You should keep Dan Kegel informed of the IP name and address of your server,
and warn him of any changes.  

Technical questions about installing and running the Anet game server 
should be directed to Dan Kegel (dank-anet@kegel.com).
A small amount of information about our game servers is also available 
informally at http://www.alumni.caltech.edu/~dank/anet.html

*** Firewall Support - Server ***
For those of you trying to run the server behind firewalls:

The server code uses local UDP port 21157. The clients connecting to
the server let the OS select a random local port number, and send from
that port to the server's port 21157; the server replies from port 21157
to the client's random local port.  

The server cannot run behind firewalls that do port or address translation; the 
only firewalls it can run behind are straight port filters.

*** Firewall Support - Games ***
For those of you trying to run Anet games behind firewalls:

This release will provide support for some port and address-translating 
firewalls.  Only those that allow 'party line' use of an open port
are supported (that is, they must reuse the same port number/address
for all packets sent from the same port/number address on the client,
regardless of destination).  Many firewalls do not support 'party line' use.

Only games released after October 1998 can use this feature.  Earlier games
do not contain the needed client code.

*** Server Naming ***
The game server code supports many games, so your server's hostname
shouldn't be specific to a single game.
I suggest using a *geographical* naming scheme, something like
oslo.alink.fragzone.se (nearest large city + "alink" + your domain)
or sweden.alink.fragzone.se (country + "alink" + your domain).

Important: if your server has more than one IP address, contact Dan Kegel
for assistance.  The server code expects a single hostname and a single
IP address.

*** Installation ***
0) Secure your system from intrusion by hackers!
   Install ssh, a secure replacement for telnet; you can download it
   from http://www.ssh.org/   
   A popular Windows client for ssh is SecureCRT, from http://www.vandyke.com.
   Verify that you can log in to your server remotely via ssh.
   Then disable remote access via rlogin and telnet, along with all other
   unnecessary network services!  You can do this by
   comment out all the services in /etc/inetd.conf,
   and renaming sendmail to sendmail.no and lpd to lpd.no.
   Verify that no network daemons besides httpd and sshd are running by 
   using ps aux.

   For more info on securing Linux, see
   http://metalab.unc.edu/LDP/HOWTO/Security-HOWTO.html

   No user accounts should exist on game server machines,
   because there is sensitive information in the game server account.
   (And the account can't be made private, since the web server needs access 
   to ~alink/public_html.)

1) Create a user named 'alink' to run the game server.  
   It's important to use that name, because otherwise you'll need to
   edit the server's configuration files.
      **** If your game server account's home directory is not /home/alink or
      its html root is not /home/alink/public_html, contact Dan Kegel 
      (DKegel@activision.com).

   Log in as user alink.

2) Configure your web server to point the URL http://yourhostname/anet2/
   to ~alink/public_html.  For Red Hat Linux's Apache, 
   edit /etc/httpd/conf/srm.conf to add the lines
	Alias /anet2/ /home/alink/public_html/
	Alias /anet2 /home/alink/public_html/

   Configure your web server to allow http://yourhostname/anet2/ to 
   contain HTML, cgi scripts, and symbolic links.  For Red Hat Linux's Apache,
   edit /etc/httpd/conf/access.conf to add the lines
	<Directory /home/alink/public_html>
		Options FollowSymLinks ExecCGI Indexes
		AllowOverride None
		order allow,deny
		allow from all
	</Directory>

   Restart your web server (in Red Hat Linux, go to /etc/rc.d/init.d
   and run httpd.init stop, then httpd.init start).

3) Verify that /tmp and /var/tmp are world-writable (don't laugh).
   Under Red Hat Linux, ls -ld /var/tmp /tmp should show both
   directories with permissions rwxrwxrwt.
   Verify that there is at least 100 MBytes of disk space (we'll only
   use 5 MB, but the log files might grow a bit).

4) Place the distribution file anet3srv.tgz in the game server account's
   home directory.  Unzip the file by typing:

	gunzip < anet3srv.tgz | tar xv

   This will create an etc/ directory in the game server account.

5) Finish installation
   a) The game server local servers
      
	  In the etc/ directory of the game server account, create the file 
	  local-servers.txt and put the IP address followed by the hostname 
	  of the game server in the file.  Here is an example:

	  192.168.0.123 dual.kegel.com

   b) The game server status HTML template files.
 
      In the etc/ directory of the game server account, you will find a
      shell script servfil.  This will download the text files used by the
      game server then generate game server status HTML template files and
      move them into the anet2 directory.  Run the script by typing:

	cd ~/etc
	servfil

      This generates an HTML template file for each supported game, with 
      name [GAME].tpl, eg. Dark_Reign.tpl.
      When the server is running, it will create and update
      files with the same name, except with the extension changed to .html.

   Note: the following steps 5c-5e should be done automatically by servfil,
   but aren't yet, so they must still be done by hand.

   c) Directory etc
 
	cd ~/public_html
	mkdir etc
	cd etc
	ln -s ~alink/etc/apps.txt apps.txt
	ln -s ~alink/etc/names.txt names.txt
	ln -s ~alink/etc/servers.txt servers.txt
	ln -s ~alink/etc/types.txt types.txt

      This allows us to easily check if your game server text files are
      up to date.

   d) Put runsrvfil.cgi into ~alink/public_html/etc.  This allows
      us to update the game server text files whenever they change, eg., when
      a new game is added.  Make sure the owner of runsrvfil.cgi is the game
      server account and the permission is setuid, setguid.
      e.g.
	cd ~/public_html/etc
	cp ../../etc/runsrvfil.cgi .
	chmod ug+sx runsrvfil.cgi

   e) Verify that the web server can read the files in ~alink/public_html
      by visiting http://(yourhostname)/anet2 with a web browser.
      If it can't, you may need to do 
      	chmod 755 ~alink
      

6) Install the file ~/etc/crontab2.lst as the game server account's crontab
   file.  If the account does not yet have a crontab file (which can be
   checked with crontab -l), typing "crontab crontab2.lst" will suffice.
   You may wish to customize the crontab2.lst to reflect different times
   or schedules.

   Within five minutes, this should automatically start the game server;
   you can verify this by looking for a anet3srv process owned by alink:
	ps auxww | grep 'alink.*anet3'

7) Send E-mail to dank-anet@kegel.com containing
   the URL of the game server status page, and the IP name and number of the
   game server, so it can be added to the master list of game servers
   seen by users when they start the games.

   Dan Kegel will be maintaining a WWW page with links to all
   game servers; you are also welcome to link to the game server from your
   own pages.

8) We discourage customization of the game server status page.  
   However, if you want to translate it into your local language,
   you need to edit the files etc/meta.in and etc/index.in, then
   rerun etc/servfil, and finally restart the server with etc/stop2,
   then etc/start2.  

Troubleshooting
   Your machine needs to have the same IP address when seen from the
   outside world as from the inside for the server to work.  This means that
   the IP address in /etc/hosts for `hostname` must be the same as the
   publicly advertised IP address in DNS for the name we advertise for your
   server. (You can set environment variable UUDP_HOSTNAME to the real IP
   address in ~/etc/anet2env if for some reason you can't or don't want to
   put the right address in /etc/hosts.)


*** Technical Questions ***
Technical questions about installing and running the Anet game server 
should be directed to Dan Kegel (dank-anet@kegel.com).


*** Legal ***
The game server, and all associated scripts and files in this distribution,
are copyright 1995-2001 Activision, and licensed under the LGPL.

*** end ***
