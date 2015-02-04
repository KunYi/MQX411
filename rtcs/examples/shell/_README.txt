				RTCS Shell Application


1. Introduction.
----------------
This is the main RTCS example that is used to demonstrate different features of the RTCS TCP/IP stack. The example allows performing initialization and configuration of network interfaces, running RTCS application services like DHCP, DNS, FTP, TFTP, Telnet, SMTP and others.


2. Running the application.
---------------------------
1) Prepare your evaluation board according to the "MQX Getting started" document.
   Pay special attention for correct jumper settings.
2) Connect the serial cable to your PC and to the evaluation board.
3) Connect the evaluation board and your PC to an Ethernet network.
4) Start Terminal application on your PC. 
   Set serial port settings to 115200 baud rate, no parity and 8 bits.
5) Compile all required libraries (BSP, PSP, MFS, RTCS).
6) Compile, load and run the application. In terminal window:  
		Initialized Ram Disk to a:\
		NOT A DOS DISK! You must format to continue.
		Shell (build: Nov 14 2013)
		Copyright (c) 2013 Freescale Semiconductor;
		shell>
		shell>
7) The RTCS Shell is now able to accept user commands.
8) Enter "help" command to display full command list and its syntax.

3. Using application.
---------------------
This chapter describes typical actions that can be done in the RTCS Shell application. 
To get information about all available shell commands enter the "help" command.

3.1) Initialize Ethernet Interface.
-----------------------------------
At application startup, no interface is initialized.
Its initialization should be done using the "ipconfig [device] init" command. For example:
	shell> ipconfig 0 init
	Ethernet device 0 initialization successful.
NOTE: 
	If [device] is not entered, assumed the device number is zero.

3.2) Release Ethernet Interface.
--------------------------------
Use the "ipconfig [device] release" command. For example:
	shell> ipconfig 0 release
	Ethernet device 0 release successful.

3.3) Change Ethernet Interface parameters.
------------------------------------------
3.3.1) Bind IPv4 address manually.
---------------------------------- 
Use the "ipconfig [device] ip <ipv4> <mask> [<gateway>]" command. For example:
	shell> ipconfig 0 ip 192.168.0.22 255.255.255.0
	Static IP4 bind successful.
	
3.3.2) Bind address parameters using DHCPv4.
-------------------------------------------- 
Use the "ipconfig [device] dhcp" command. For example:
	shell> ipconfig 0 dhcp
	Bind via dhcp successful.
	
3.3.3) Bind IPv6 address manually.
---------------------------------- 
Binding IPv6 address is done automatically during stateless address autoconfiguration, so the manual binding of IPv6 address is optional.
If it is really needed, it can be done using the "ipconfig [device] ip <ipv6>" command. For example:
	shell> ipconfig 0 ip 0123:4567:89ab:cdef:0123:4567:89ab:cdef
	IP6 bind successful.
	
3.3.4) Unbind IPv6 address. 
---------------------------
Use the "ipconfig [device] unbind <ipv6>" command. For example:
	shell> ipconfig 0 unbind 0123:4567:89ab:cdef:0123:4567:89ab:cdef
	Unbind successful.

3.4) Add gateway.
-----------------
To add a gateway to the routing table, use the "gate <gateway> [<netaddr>] [<netmask>]" command. For example:
	shell> gate 192.168.0.5
	Adding gateway 192.168.0.5, ip address: 0.0.0.0, netmask: 0.0.0.0

3.5) View Ethernet Interface parameters.
----------------------------------------
Use the "ipconfig [device]" command:
	shell> ipconfig
	Eth#     : 0
	Link     : on
	MAC      : 00:00:5e:00:00:00
	IP4      : 192.168.22.193 Type: DHCPNOAUTO
	IP4 Mask : 255.255.255.0
	IP4 Gate : 192.168.22.1
	IP4 DNS  : 212.71.186.249
	IP6      : fe80::200:5eff:fe00:0 State: PREFERRED Type: AUTOCONFIGURABLE
	ScopeID  : 2
	Link status task stopped

	NOTE: For information about Link status task refer to RTCS User's Guide.

3.6) DNS address resolver.
--------------------------
3.6.1) Add DNS Server address manually.
---------------------------------------
Usually, the DNS server address list is updated automatically by DHCP client or Router Discovery mechanism. 
If needed, it is possible to add DNS Server address manually, using the "ipconfig [device] dns add <ip>" command. For example:
	shell> ipconfig 0 dns add 192.168.0.5
	Add dns ip successful.
	
3.6.2) Delete DNS Server address manually.
------------------------------------------
If needed, it is possible to delete DNS Server address manually, using the "ipconfig [device] dns del <ip>" command. For example:
	shell> ipconfig 0 dns del 192.168.0.5
	Del dns ip successful.

3.6.3) Display DNS Server address list.
---------------------------------------
To print all DNS Server addresses, assigned to an interface, use "ipconfig [device] dns". For example:
	shell> ipconfig 0 dns
	[1] 212.71.186.249

3.6.4) Resolve host name.
-------------------------
Use the "gethbn <host>" command. For example:
	shell> gethbn www.example.com
	www.example.com 93.184.216.119
	www.example.com 2606:2800:220:6d:26bf:1447:1097:aa7

3.7) Ping.
----------
Use the "ping [-c count] [-h hoplimit] [-i interval] [-p pattern] [-s size] <host>" command. For example:
	shell> ping www.example.com
	Pinging 93.184.216.119:
	Press [q] to cancel.
	Reply from [93.184.216.119]: time=138ms
	Reply from [93.184.216.119]: time=130ms
	Reply from [93.184.216.119]: time=132ms
	Reply from [93.184.216.119]: time=132ms

3.8) Display network statistic.
-------------------------------
Use the "netstat" command. For example:
	shell> netstat

	ENET (1fff9bc0):
	   113 packets received
	   9 packets transmitted

	IPv4:
	   10 packets received
	   7 packets delivered
	   3 discarded for other reasons:
	   3   with an illegal destination

	IPv6:
	   4 packets sent

	ICMPv4:
	   0 packets sent or received

	ICMPv6:
	   4 Unknown

	UDP:
	   7 packets received
	   7 discarded for other reasons:
	   7   with unknown ports

	TCP:
	   0 packets sent or received

3.9) ARP table.
---------------
3.9.1) Display ARP table.
-------------------------
Use the "arpdisp" command. For example:
	shell> arpdisp
	192.168.000.005 00:e0:4c:68:23:43 C   Resend=0, Age=742230, Expire=180000
	192.168.000.022 00:00:00:00:00:00 I--Resend=0, Age=0, Expire=0

3.9.2) Add new entry to ARP table.
----------------------------------
Use the "arpadd <ip_address> <mac>" command. For example:
	shell> arpadd 192.168.0.25 00:23:5c:69:24:45
	192.168.000.025 00:23:5c:69:24:45 C   Resend=0, Age=25511, Expire=25511
	192.168.000.005 00:e0:4c:68:23:43 C   Resend=0, Age=993214, Expire=154489
	192.168.000.022 00:00:00:00:00:00 I--Resend=0, Age=0, Expire=0

3.9.3) Delete entry from ARP table.
-----------------------------------
Use the "arpdel <ip_address>" command. For example:
	shell> arpdel 192.168.0.25
	shell> arpdisp
	192.168.000.005 00:e0:4c:68:23:43 C   Resend=0, Age=816158, Expire=180000
	192.168.000.022 00:00:00:00:00:00 I--Resend=0, Age=0, Expire=0

3.10) File System.
------------------
File system is used by FTP, TFTP servers and clients.

3.10.1) Format disk.
--------------------
Use the "format <drive:> [<volume label>]" command. For example:
	shell> format a: RAM

	Formatting...
	Done. Volume name is RAM
	Free disk space: 11776 bytes

3.10.2) Create empty file.
--------------------------
Use the "create <filename> [<mode>]" command. For example:
	shell> create test.txt

3.10.3) Delete file.
--------------------
Use the "del <file>" command. For example:
	shell> dir
	.                   0 01-01-1980 03:41:18     D
	..                  0 01-01-1980 03:41:18     D
	TEST1.TXT           0 01-01-1980 03:42:32      A
	shell> del test1.txt
	shell> dir
	.                   0 01-01-1980 03:41:18     D
	..                  0 01-01-1980 03:41:18     D

3.10.4) List content of current directory.
------------------------------------------
Use the "dir" command. For example:
	shell> dir
	TEST.TXT            0 01-01-1980 03:28:06      A

3.10.5) Make new directory.
---------------------------
Use the "mkdir <directory>" command. For example:
	shell> mkdir test_dir
	shell> dir
	TEST_DIR            0 01-01-1980 03:41:18     D
	
3.10.6) Remove directory.
-------------------------
The directory must be empty.
Use the "rmdir <directory>" command. For example:
	shell> dir
	TEST.TXT            0 01-01-1980 03:28:06      A
	TEST_DIR            0 01-01-1980 03:41:18     D
	shell> rmdir test_dir
	shell> dir
	TEST.TXT            0 01-01-1980 03:28:06      A

3.10.7) Display path of working directory.
------------------------------------------
Use the "pwd" command. For example:
	shell> pwd
	a:\test_dir

3.10.8) Change working directory.
---------------------------------
Use the "cd <directory>" command. For example:
	shell> pwd
	a:\
	shell> cd test_dir
	shell> pwd
	a:\test_dir

3.10.9) Rename file.
--------------------
Use the "ren <oldname> <newname>" command. For example:
	shell> dir
	TEST.TXT            0 01-01-1980 03:28:06      A
	shell> ren test.txt test_new.txt
	shell> dir
	TEST_NEW.TXT        0 01-01-1980 04:08:46      A

3.10.10) Copy file.
-------------------
Use the "copy <source> <dest>" command. For example:
	shell> copy test_new.txt test_dir\test_copy.txt
	shell> cd test_dir
	shell> dir
	.                   0 01-01-1980 03:56:08     D
	..                  0 01-01-1980 03:56:08     D
	TEST_C~1.TXT        0 01-01-1980 04:11:56      A test_copy.txt

3.10.11) Write bytes to file.
-----------------------------
Use the "write <filename> <bytes> [<seek_mode> [<offset>]]" command. For example:
	shell> dir
	TEST_NEW.TXT        0 01-01-1980 04:08:46      A
	shell> write test_new.txt 100
	shell> dir
	TEST_NEW.TXT      100 01-01-1980 04:18:20      A

3.10.12) Display content of file.
---------------------------------
Use the "read <filename> <bytes> [<seek_mode> [<offset>]]" command. For example:
	shell> read test_new.txt 100
	Reading from test_new.txt:
	0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghij
	klmnopqrstuvwxyz0123456789:;<=>?@ABCDEFGH
	Done.

3.11 Telnet client.
-------------------
To connect to a remote Telnet server use the "telnet <server ip address>" command. For example:
	1. Start a Telnet server on your PC; 
	   or if you have installed the http://nmap.org/ tool,
	   in "command prompt" window type "ncat --listen 0.0.0.0 23".
	2. shell> telnet 192.168.0.5
	   Connecting to [192.168.0.5]:
	3. Type some characters on terminal and check 
	   "command prompt" window. 
	NOTE: If you are not able to connect to the server, 
	      try to disable firewall or antivirus service on your PC.

3.12 Telnet server.
-------------------
3.12.1 Start Telnet server.
---------------------------
Use the "telnetd start" command. For example:
	1. shell> telnetd start
	   Telnet Server Started.
	2. In the "command prompt" window, type "telnet 192.168.0.22" 
	   to connect to the telnet server on the board 
	   (192.168.0.22 is the IP address of the board).

3.12.1 Stop Telnet server.
--------------------------
Use the "telnetd stop" command. For example:
	shell> telnetd stop
	Telnet Server Stopped.

3.13 FTP Client.
----------------
To start FTP client use the "ftp [server]" command. For example:
	1. Start a FTP server on your PC.
		NOTE: It was used the FileZilla FTP server.
	2. Before client start, be sure the RAM disk is formatted:
			shell> format a:
			Formatting...
			Done. Volume name is
			Free disk space: 11776 bytes
	2. Connect to FTP server:
			shell> ftp 192.168.0.5
			RTCS v3.08.00 FTP client
			Connecting to:  [192.168.0.5]
			220-FileZilla Server version 0.9.41 beta
			220-written by Tim Kosse (Tim.Kosse@gmx.de)
			220 Please visit http://sourceforge.net/projects/filezilla/
			ftp []>
	3. If required, type user and password to login to ftp server.
	4. To display the full command list, supported by 
	   the FTP client, enter the "help" command. 
	5. To close the FTP client, enter "bye" command. 
	NOTE: If you are not able to connect to the server, 
	      try to disable firewall or antivirus service on your PC.

3.14 FTP Server.
----------------
3.14.1 Start FTP server.
------------------------
Use the "ftpsrv start" command. For example:
	1. Before server start, be sure the RAM disk is formatted:
		shell> format a:
		Formatting...
		Done. Volume name is
		Free disk space: 11776 bytes
	2. Start the FTP server:
		shell> ftpsrv start
		FTP Server Started. Root directory is set to "a:", login: "developer", password: "freescale".
  3. In "command prompt" window, type "ftp 192.168.0.22" to connect the FTP server on the board (192.168.0.22 is its IP address).
	NOTE: Windows OS native FTP client does not support all IPv6 FTP commands. 
        So it is recommended to use FileZilla FTP client (https://filezilla-project.org/). 


3.14.1 Stop FTP server.
------------------------
Use the "ftpsrv stop" command. For example:
	shell> ftpsrv stop
	FTP server stopped.


3.13 TFTP Client.
----------------
To download a file from a TFTP server use the "tftp <server address> <source file> [<dest file>]" command. For example:
	1. Start a TFTP server on your PC.
		NOTE: It was used the Tftpd32 server.
	2. Before client start, be sure the RAM disk is formatted:
		shell> format a:
		Formatting...
		Done. Volume name is
		Free disk space: 11776 bytes
	2. Download file from a TFTP server:
		shell> tftp 192.168.0.5 test.txt
		Downloading file test.txt from TFTP server:  [192.168.0.5]
	3. Use the "dir" command to check file is downloaded
		shell> dir
		TEST.TXT          106 01-01-1980 02:58:28      A
	NOTE: If you are not able to connect to the server, 
	      try to disable firewall or antivirus service on your PC.

3.14 TFTP Server.
----------------
3.14.1 Start TFTP server.
------------------------
Use the "tftpd start" command. For example:
	1. Before server start, be sure the RAM disk is formatted:
		shell> format a:
		Formatting...
		Done. Volume name is
		Free disk space: 11776 bytes
	2. Create test file:
		shell> write test.txt 123
	3. Start the TFTP server:
		shell> tftpd start
		TFTP Server Started.
4. Set remote file parameter of a TFTP client to "a:\TEST.TXT" and download the file.
	NOTE: It was used the Tftpd32 client.

3.14.2 Stop TFTP server.
------------------------
Use the "tftpd stop" command. For example:
	shell> tftpd stop
	TFTP Server Stopped.

3.15 SMTP E-mail client.
------------------------
To send an E-mail message use the "email -f <from@email.com> -t <to@email.com> -s <server address> [-u <username>] [-p <password>] -m <"text of email message">" command. For example: 
	1. Configure your mail server, setup proper e-mail accounts, 
	   passwords and other parameters.
	   NOTE: It was used the mail server from www.hmailserver.com.
	2. Send test E-mail message:
		email -f test1@test.fsl -t test2@test.fsl -s 192.168.0.5 -u test1@test.fsl -p 123456 -m "TEST"
		Email send. Server response: Queued (0.827 seconds)
	NOTE: If you are not able to connect to the server, 
	      try to disable firewall or antivirus service on your PC.




