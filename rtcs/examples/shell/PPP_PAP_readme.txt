How to make PAP authentication using PPP connection through serial line.
*****
* You can found some user_config.h examples in
* ...\rtcs\examples\shell\user_config\ BOARD NAME\ user_config.h
*****
	
To start PPP connection like client type command "ppp client ittyX:", 
to stop "ppp stop" in SHELL.	

****************EXAMPLE***************************
shell> ppp client ittyd: 
Start PPP...

Please initiate PPP connection.  Waiting...

PPP_link = 1, time = 3

PPP device on ittyb: is bound on.
PPP local address is   : 192.168.0.5
PPP remote address is : 192.168.0.1
Now PPP connection is established on 192.168.0.1

shell>


If you need  use login and password for PPP connection add it to command :
Example:
shell> ppp client ittyX: yourlogin yourpassword


To start PPP like server type " ppp server ittyX: yourlogin yourpassword local_ip_address remoute_ip_address
Example:
shell> ppp server ittyd: guest anonimous 192.168.0.1 192.168.0.217

Here:
- ittyd: is name of your serial interface. 
- guest is login to your PPP server.
- anonymous is password to your PPP server.
- 192.168.0.1 is IP address will have your board.
- 192.168.0.217 is IP address that your PPP server will set to client.
You should use all of that parameters.

To stop PPP server type "ppp stop" .
Shell function for PPP is in shell_ppp.c .









