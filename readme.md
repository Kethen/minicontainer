One of those for fun projects

referencing systemd-nspawn https://github.com/systemd/systemd/tree/master/src/nspawn

and

jchroot https://github.com/vincentbernat/jchroot/blob/master/

Currently embedding gnu screen as a vt100 emulator. In terms of source code volume this is more of an extension to gnu screen, however.

Goal: \
a simple linux container that run on random devices such as android phones as long as kernel supports CLONE_NEWNS, CLONE_NEWPID, CLONE_NEWIPC and CLONE_NEWUTS

Build:\
$make

Usage: 

	#minicontainer -p (root path) \

		[-i init=/sbin/init] \
	
		[-r runlevel=3] \
	
		[-n custom child process name and screen window name] \
	
		[-s alternate screen binary] \
	
		[-a [pid or name, relays to screen -r for reattachment]] \
		
		[-d pid or name, relays to screen -d for detachment] \
		
		[-h]

-n : inits could change their process name however
	
-h will skip starting a new screen then take current tty over directly. This method usually result in sighup on parent and kills the parent. This flag was made for self launching inside a screen 
