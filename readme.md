One of those for fun projects

referencing systemd-nspawn https://github.com/systemd/systemd/tree/master/src/nspawn

and

jchroot https://github.com/vincentbernat/jchroot/blob/master/

Goal: run on random devices such as android phones as long as kernel supports namespaces

TODO:

1. Implement proper recursive umount after init systems / investigate if MNT_DETACH does the job properly

2. Fake tty console for init systems
