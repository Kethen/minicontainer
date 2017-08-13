SCREENSRC = ./screen/src

SCREENOBJ = acls.o ansi.o attacher.o backtick.o canvas.o comm.o display.o encoding.o fileio.o help.o input.o kmapdef.o layer.o layout.o list_display.o list_generic.o list_license.o list_window.o logfile.o mark.o misc.o process.o pty.o resize.o sched.o screen.o search.o socket.o telnet.o termcap.o term.o tty.o utmp.o viewport.o window.o winmsgbuf.o winmsgcond.o winmsg.o

SCREENOBJFULL = $(SCREENSRC)/acls.o $(SCREENSRC)/ansi.o $(SCREENSRC)/attacher.o $(SCREENSRC)/backtick.o $(SCREENSRC)/canvas.o $(SCREENSRC)/comm.o $(SCREENSRC)/display.o $(SCREENSRC)/encoding.o $(SCREENSRC)/fileio.o $(SCREENSRC)/help.o $(SCREENSRC)/input.o $(SCREENSRC)/kmapdef.o $(SCREENSRC)/layer.o $(SCREENSRC)/layout.o $(SCREENSRC)/list_display.o $(SCREENSRC)/list_generic.o $(SCREENSRC)/list_license.o $(SCREENSRC)/list_window.o $(SCREENSRC)/logfile.o $(SCREENSRC)/mark.o $(SCREENSRC)/misc.o $(SCREENSRC)/process.o $(SCREENSRC)/pty.o $(SCREENSRC)/resize.o $(SCREENSRC)/sched.o $(SCREENSRC)/screen.o $(SCREENSRC)/search.o $(SCREENSRC)/socket.o $(SCREENSRC)/telnet.o $(SCREENSRC)/termcap.o $(SCREENSRC)/term.o $(SCREENSRC)/tty.o $(SCREENSRC)/utmp.o $(SCREENSRC)/viewport.o $(SCREENSRC)/window.o $(SCREENSRC)/winmsgbuf.o $(SCREENSRC)/winmsgcond.o $(SCREENSRC)/winmsg.o 
SCREENHEADER = acls.h ansi.h attacher.h backtick.h canvas.h comm.h config.h display.h encoding.h fileio.h help.h image.h input.h kmapdef.h layer.h layout.h list_generic.h logfile.h mark.h misc.h os.h process.h pty.h resize.h sched.h screen.h search.h socket.h telnet.h termcap.h term.h tty.h utmp.h viewport.h window.h winmsgbuf.h winmsgcond.h winmsg.h

SCREENHEADERFULL = $(SCREENSRC)/acls.h $(SCREENSRC)/ansi.h $(SCREENSRC)/attacher.h $(SCREENSRC)/backtick.h $(SCREENSRC)/canvas.h $(SCREENSRC)/comm.h $(SCREENSRC)/config.h $(SCREENSRC)/display.h $(SCREENSRC)/encoding.h $(SCREENSRC)/fileio.h $(SCREENSRC)/help.h $(SCREENSRC)/image.h $(SCREENSRC)/input.h $(SCREENSRC)/kmapdef.h $(SCREENSRC)/layer.h $(SCREENSRC)/layout.h $(SCREENSRC)/list_generic.h $(SCREENSRC)/logfile.h $(SCREENSRC)/mark.h $(SCREENSRC)/misc.h $(SCREENSRC)/os.h $(SCREENSRC)/process.h $(SCREENSRC)/pty.h $(SCREENSRC)/resize.h $(SCREENSRC)/sched.h $(SCREENSRC)/screen.h $(SCREENSRC)/search.h $(SCREENSRC)/socket.h $(SCREENSRC)/telnet.h $(SCREENSRC)/termcap.h $(SCREENSRC)/term.h $(SCREENSRC)/tty.h $(SCREENSRC)/utmp.h $(SCREENSRC)/viewport.h $(SCREENSRC)/window.h $(SCREENSRC)/winmsgbuf.h $(SCREENSRC)/winmsgcond.h $(SCREENSRC)/winmsg.h

SCREENENCODINGS = $(datadir)/screen/utf8encodings

miniscreencontainer: $(SCREENOBJFULL) $(SCREENHEADERFULL) main3.c
	gcc -g3 -O2 -Wall -Wextra -std=c11 -iquote. -DSCREENENCODINGS='"$(SCREENENCODINGS)"' $(SCREENOBJFULL) -I$(SCREENSRC) main3.c -lutil -lcrypt -lcurses  -lutil -lpam -o miniscreencontainer

miniweirdcontainer: main.c
	gcc -s main.c -o miniweirdcontainer
	
minicontainer: main2.c
	gcc -s main2.c -o minicontainer
	
$(SCREENSRC)/acls.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/ansi.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/attacher.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/backtick.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/canvas.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/comm.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/display.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/encoding.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/fileio.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/help.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/input.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/kmapdef.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/layer.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/layout.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/list_display.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/list_generic.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/list_license.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/list_window.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/logfile.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/mark.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/misc.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/process.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/pty.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/resize.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/sched.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/screen.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/search.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/socket.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/telnet.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/termcap.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/term.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/tty.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/utmp.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/viewport.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/window.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/winmsgbuf.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/winmsgcond.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
$(SCREENSRC)/winmsg.o:
	cd $(SCREENSRC)/
	./autogen
	./configure
	make
