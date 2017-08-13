SCREENSRC = ./screen/src

SCREENOBJ = acls.o ansi.o attacher.o backtick.o canvas.o comm.o display.o encoding.o fileio.o help.o input.o kmapdef.o layer.o layout.o list_display.o list_generic.o list_license.o list_window.o logfile.o mark.o misc.o process.o pty.o resize.o sched.o screen.o search.o socket.o telnet.o termcap.o term.o tty.o utmp.o viewport.o window.o winmsgbuf.o winmsgcond.o winmsg.o

SCREENOBJFULL = $(SCREENSRC)/acls.o $(SCREENSRC)/ansi.o $(SCREENSRC)/attacher.o $(SCREENSRC)/backtick.o $(SCREENSRC)/canvas.o $(SCREENSRC)/comm.o $(SCREENSRC)/display.o $(SCREENSRC)/encoding.o $(SCREENSRC)/fileio.o $(SCREENSRC)/help.o $(SCREENSRC)/input.o $(SCREENSRC)/kmapdef.o $(SCREENSRC)/layer.o $(SCREENSRC)/layout.o $(SCREENSRC)/list_display.o $(SCREENSRC)/list_generic.o $(SCREENSRC)/list_license.o $(SCREENSRC)/list_window.o $(SCREENSRC)/logfile.o $(SCREENSRC)/mark.o $(SCREENSRC)/misc.o $(SCREENSRC)/process.o $(SCREENSRC)/pty.o $(SCREENSRC)/resize.o $(SCREENSRC)/sched.o $(SCREENSRC)/screen.o $(SCREENSRC)/search.o $(SCREENSRC)/socket.o $(SCREENSRC)/telnet.o $(SCREENSRC)/termcap.o $(SCREENSRC)/term.o $(SCREENSRC)/tty.o $(SCREENSRC)/utmp.o $(SCREENSRC)/viewport.o $(SCREENSRC)/window.o $(SCREENSRC)/winmsgbuf.o $(SCREENSRC)/winmsgcond.o $(SCREENSRC)/winmsg.o 

SCREENHEADER = acls.h ansi.h attacher.h backtick.h canvas.h comm.h config.h display.h encoding.h fileio.h help.h image.h input.h kmapdef.h layer.h layout.h list_generic.h logfile.h mark.h misc.h os.h process.h pty.h resize.h sched.h screen.h search.h socket.h telnet.h termcap.h term.h tty.h utmp.h viewport.h window.h winmsgbuf.h winmsgcond.h winmsg.h

SCREENHEADERFULL = $(SCREENSRC)/acls.h $(SCREENSRC)/ansi.h $(SCREENSRC)/attacher.h $(SCREENSRC)/backtick.h $(SCREENSRC)/canvas.h $(SCREENSRC)/comm.h $(SCREENSRC)/config.h $(SCREENSRC)/display.h $(SCREENSRC)/encoding.h $(SCREENSRC)/fileio.h $(SCREENSRC)/help.h $(SCREENSRC)/image.h $(SCREENSRC)/input.h $(SCREENSRC)/kmapdef.h $(SCREENSRC)/layer.h $(SCREENSRC)/layout.h $(SCREENSRC)/list_generic.h $(SCREENSRC)/logfile.h $(SCREENSRC)/mark.h $(SCREENSRC)/misc.h $(SCREENSRC)/os.h $(SCREENSRC)/process.h $(SCREENSRC)/pty.h $(SCREENSRC)/resize.h $(SCREENSRC)/sched.h $(SCREENSRC)/screen.h $(SCREENSRC)/search.h $(SCREENSRC)/socket.h $(SCREENSRC)/telnet.h $(SCREENSRC)/termcap.h $(SCREENSRC)/term.h $(SCREENSRC)/tty.h $(SCREENSRC)/utmp.h $(SCREENSRC)/viewport.h $(SCREENSRC)/window.h $(SCREENSRC)/winmsgbuf.h $(SCREENSRC)/winmsgcond.h $(SCREENSRC)/winmsg.h

SCREENENCODINGS = $(datadir)/screen/utf8encodings

miniscreencontainer: $(SCREENOBJFULL) $(SCREENHEADERFULL) main3.c
	#gcc -g3 -O2 -Wall -Wextra -std=c11 -iquote. -DSCREENENCODINGS='"$(SCREENENCODINGS)"' $(SCREENOBJFULL) -I$(SCREENSRC) main3.c -lutil -lcrypt -lcurses  -lutil -lpam -o miniscreencontainer
	gcc -s $(SCREENOBJFULL) -I$(SCREENSRC) main3.c -lutil -lcrypt -lcurses  -lutil -lpam -o miniscreencontainer

miniweirdcontainer: main.c
	gcc -s main.c -o miniweirdcontainer
	
minicontainer: main2.c
	gcc -s main2.c -o minicontainer
	
$(SCREENSRC)/Makefile:
	cd $(SCREENSRC); ./autogen.sh; ./configure

clean:
	rm -f minicontainer miniweirdcontainer miniscreencontainer 
	cd $(SCREENSRC); make distclean;

$(SCREENSRC)/acls.o:$(SCREENSRC)/acls.c $(SCREENSRC)/acls.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make acls.o
$(SCREENSRC)/ansi.o:$(SCREENSRC)/ansi.c $(SCREENSRC)/ansi.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make ansi.o
$(SCREENSRC)/attacher.o:$(SCREENSRC)/attacher.c $(SCREENSRC)/attacher.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make attacher.o
$(SCREENSRC)/backtick.o:$(SCREENSRC)/backtick.c $(SCREENSRC)/backtick.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make backtick.o
$(SCREENSRC)/canvas.o:$(SCREENSRC)/canvas.c $(SCREENSRC)/canvas.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make canvas.o
$(SCREENSRC)/mm.o:$(SCREENSRC)/mm.c $(SCREENSRC)/mm.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make mm.o
$(SCREENSRC)/display.o:$(SCREENSRC)/display.c $(SCREENSRC)/display.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make display.o
$(SCREENSRC)/ending.o:$(SCREENSRC)/ending.c $(SCREENSRC)/ending.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make ending.o
$(SCREENSRC)/file.o:$(SCREENSRC)/file.c $(SCREENSRC)/file.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make file.o
$(SCREENSRC)/help.o:$(SCREENSRC)/help.c $(SCREENSRC)/help.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make help.o
$(SCREENSRC)/input.o:$(SCREENSRC)/input.c $(SCREENSRC)/input.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make input.o
$(SCREENSRC)/kmapdef.o:$(SCREENSRC)/kmapdef.c $(SCREENSRC)/kmapdef.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make kmapdef.o
$(SCREENSRC)/layer.o:$(SCREENSRC)/layer.c $(SCREENSRC)/layer.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make layer.o
$(SCREENSRC)/laut.o:$(SCREENSRC)/laut.c $(SCREENSRC)/laut.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make laut.o
$(SCREENSRC)/list_display.o:$(SCREENSRC)/list_display.c $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make list_display.o
$(SCREENSRC)/list_generic.o:$(SCREENSRC)/list_generic.c $(SCREENSRC)/list_generic.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make list_generic.o
$(SCREENSRC)/list_license.o:$(SCREENSRC)/list_license.c $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make list_license.o
$(SCREENSRC)/list_winw.o:$(SCREENSRC)/list_winw.c $(SCREENSRC)/list_winw.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make list_winw.o
$(SCREENSRC)/gfile.o:$(SCREENSRC)/gfile.c $(SCREENSRC)/gfile.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make gfile.o
$(SCREENSRC)/mark.o:$(SCREENSRC)/mark.c $(SCREENSRC)/mark.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make mark.o
$(SCREENSRC)/misc.o:$(SCREENSRC)/misc.c $(SCREENSRC)/misc.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make misc.o
$(SCREENSRC)/pcess.o:$(SCREENSRC)/pcess.c $(SCREENSRC)/pcess.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make pcess.o
$(SCREENSRC)/pty.o:$(SCREENSRC)/pty.c $(SCREENSRC)/pty.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make pty.o
$(SCREENSRC)/resize.o:$(SCREENSRC)/resize.c $(SCREENSRC)/resize.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make resize.o
$(SCREENSRC)/sched.o:$(SCREENSRC)/sched.c $(SCREENSRC)/sched.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make sched.o
$(SCREENSRC)/screen.o:$(SCREENSRC)/screen.c $(SCREENSRC)/screen.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make screen.o
$(SCREENSRC)/search.o:$(SCREENSRC)/search.c $(SCREENSRC)/search.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make search.o
$(SCREENSRC)/cket.o:$(SCREENSRC)/cket.c $(SCREENSRC)/cket.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make cket.o
$(SCREENSRC)/telnet.o:$(SCREENSRC)/telnet.c $(SCREENSRC)/telnet.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make telnet.o
$(SCREENSRC)/termcap.o:$(SCREENSRC)/termcap.c $(SCREENSRC)/termcap.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make termcap.o
$(SCREENSRC)/term.o:$(SCREENSRC)/term.c $(SCREENSRC)/term.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make term.o
$(SCREENSRC)/tty.o:$(SCREENSRC)/tty.c $(SCREENSRC)/tty.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make tty.o
$(SCREENSRC)/utmp.o:$(SCREENSRC)/utmp.c $(SCREENSRC)/utmp.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make utmp.o
$(SCREENSRC)/viewrt.o:$(SCREENSRC)/viewrt.c $(SCREENSRC)/viewrt.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make viewrt.o
$(SCREENSRC)/winw.o:$(SCREENSRC)/winw.c $(SCREENSRC)/winw.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make winw.o
$(SCREENSRC)/winmsgbuf.o:$(SCREENSRC)/winmsgbuf.c $(SCREENSRC)/winmsgbuf.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make winmsgbuf.o
$(SCREENSRC)/winmsgnd.o:$(SCREENSRC)/winmsgnd.c $(SCREENSRC)/winmsgnd.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make winmsgnd.o
$(SCREENSRC)/winmsg.o:$(SCREENSRC)/winmsg.c $(SCREENSRC)/winmsg.h $(SCREENSRC)/Makefile
	cd $(SCREENSRC); make winmsg.o
