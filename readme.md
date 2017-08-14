One of those for fun projects

referencing systemd-nspawn https://github.com/systemd/systemd/tree/master/src/nspawn

and

jchroot https://github.com/vincentbernat/jchroot/blob/master/

Goal: run on random devices such as android phones as long as kernel supports namespaces

Build:
$make miniscreencontainer (screen embedded version)
$make minicontainer (tty stealing version, usually kills parent process due to SIGHUP sent from agetty when it takes over)
$make miniweirdcontainer (first prototype, trying to do pty passthrough then realise that I need vt100 emulation codes)

Usage:
#miniscreencontainer -p (root path) [-i (custom init path inside root=/sbin/init)] [-r (runlevel=3)] [-n (custom child process name=minicontainer_inside (inits could change their name however))] [-s (alternate screen binary)] [-h]
-h will skip starting a new screen then take current tty over directly. This method usually result in sighup on parent and kills the parent. This flag was made for self launching inside a screen
#minicontainer -p (root path) [-i (custom init path inside root=/sbin/init)] [-r (runlevel=3)] [-n (custom child process name=minicontainer_inside (inits could change their name however))] [-s (alternate screen binary)]
#miniweirdcontainer -p (root path) [-i (custom init path inside root=/sbin/init)] [-r (runlevel=3)] [-n (custom child process name=minicontainer_inside (inits could change their name however))] [-s (alternate screen binary)]

