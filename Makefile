SCREENSRC = ./screen/screen-4.3.1

SCREENOBJFULL = $(SCREENSRC)/acls.o $(SCREENSRC)/ansi.o $(SCREENSRC)/attacher.o $(SCREENSRC)/braille.o $(SCREENSRC)/braille_tsi.o $(SCREENSRC)/canvas.o $(SCREENSRC)/comm.o $(SCREENSRC)/display.o $(SCREENSRC)/encoding.o $(SCREENSRC)/fileio.o $(SCREENSRC)/help.o $(SCREENSRC)/input.o $(SCREENSRC)/kmapdef.o $(SCREENSRC)/layer.o $(SCREENSRC)/layout.o $(SCREENSRC)/list_display.o $(SCREENSRC)/list_generic.o $(SCREENSRC)/list_window.o $(SCREENSRC)/loadav.o $(SCREENSRC)/logfile.o $(SCREENSRC)/mark.o $(SCREENSRC)/misc.o $(SCREENSRC)/nethack.o $(SCREENSRC)/process.o $(SCREENSRC)/pty.o $(SCREENSRC)/putenv.o $(SCREENSRC)/resize.o $(SCREENSRC)/sched.o $(SCREENSRC)/screen.o $(SCREENSRC)/search.o $(SCREENSRC)/socket.o $(SCREENSRC)/teln.o $(SCREENSRC)/termcap.o $(SCREENSRC)/term.o $(SCREENSRC)/tty.o $(SCREENSRC)/utmp.o $(SCREENSRC)/viewport.o $(SCREENSRC)/window.o

all: minicontainer featuretest

featuretest: featuretest.c
	gcc -s featuretest.c -o featuretest

minicontainer: $(SCREENOBJFULL) $(SCREENHEADERFULL) main3.c
	#gcc -g3 -O2 -Wall -Wextra -std=c11 -iquote. -DSCREENENCODINGS='"$(SCREENENCODINGS)"' $(SCREENOBJFULL) -I$(SCREENSRC)/ main3.c -lutil -lcrypt -lcurses  -lutil -lpam -o miniscreencontainer
	gcc -O2 -s $(SCREENOBJFULL) -I$(SCREENSRC)/ main3.c -lcurses  -lcrypt -o minicontainer

minicontainerDEBUG: $(SCREENOBJFULL) $(SCREENHEADERFULL) main3.c
	gcc -g3 $(SCREENOBJFULL) -I$(SCREENSRC)/ main3.c -lcurses  -lcrypt -o minicontainerDEBUG
	
$(SCREENSRC)/config.h:
	cd $(SCREENSRC)/; ./autogen.sh; ./configure

$(SCREENSRC)/tty.c:
	cd $(SCREENSRC)/; make tty.c

clean:
	rm -f minicontainer
	cd $(SCREENSRC)/; make realclean;

$(SCREENSRC)/acls.o:$(SCREENSRC)/config.h $(SCREENSRC)/acls.c $(SCREENSRC)/acls.h
	cd $(SCREENSRC); make acls.o
$(SCREENSRC)/ansi.o:$(SCREENSRC)/config.h $(SCREENSRC)/ansi.c $(SCREENSRC)/ansi.h
	cd $(SCREENSRC); make ansi.o
$(SCREENSRC)/attacher.o:$(SCREENSRC)/config.h $(SCREENSRC)/attacher.c
	cd $(SCREENSRC); make attacher.o
$(SCREENSRC)/braille.o:$(SCREENSRC)/config.h $(SCREENSRC)/braille.c $(SCREENSRC)/braille.h
	cd $(SCREENSRC); make braille.o
$(SCREENSRC)/braille_tsi.o:$(SCREENSRC)/config.h $(SCREENSRC)/braille_tsi.c
	cd $(SCREENSRC); make braille_tsi.o
$(SCREENSRC)/canvas.o:$(SCREENSRC)/config.h $(SCREENSRC)/canvas.c $(SCREENSRC)/canvas.h
	cd $(SCREENSRC); make canvas.o
$(SCREENSRC)/mm.o:$(SCREENSRC)/config.h $(SCREENSRC)/mm.c $(SCREENSRC)/mm.h
	cd $(SCREENSRC); make mm.o
$(SCREENSRC)/display.o:$(SCREENSRC)/config.h $(SCREENSRC)/display.c $(SCREENSRC)/display.h
	cd $(SCREENSRC); make display.o
$(SCREENSRC)/ending.o:$(SCREENSRC)/config.h $(SCREENSRC)/ending.c $(SCREENSRC)/ending.h
	cd $(SCREENSRC); make ending.o
$(SCREENSRC)/file.o:$(SCREENSRC)/config.h $(SCREENSRC)/file.c $(SCREENSRC)/file.h
	cd $(SCREENSRC); make file.o
$(SCREENSRC)/help.o:$(SCREENSRC)/config.h $(SCREENSRC)/help.c
	cd $(SCREENSRC); make help.o
$(SCREENSRC)/input.o:$(SCREENSRC)/config.h $(SCREENSRC)/input.c
	cd $(SCREENSRC); make input.o
$(SCREENSRC)/kmapdef.o:$(SCREENSRC)/config.h $(SCREENSRC)/kmapdef.c
	cd $(SCREENSRC); make kmapdef.o
$(SCREENSRC)/layer.o:$(SCREENSRC)/config.h $(SCREENSRC)/layer.c $(SCREENSRC)/layer.h
	cd $(SCREENSRC); make layer.o
$(SCREENSRC)/laut.o:$(SCREENSRC)/config.h $(SCREENSRC)/laut.c $(SCREENSRC)/laut.h
	cd $(SCREENSRC); make laut.o
$(SCREENSRC)/list_display.o:$(SCREENSRC)/config.h $(SCREENSRC)/list_display.c
	cd $(SCREENSRC); make list_display.o
$(SCREENSRC)/list_generic.o:$(SCREENSRC)/config.h $(SCREENSRC)/list_generic.c $(SCREENSRC)/list_generic.h
	cd $(SCREENSRC); make list_generic.o
$(SCREENSRC)/list_winw.o:$(SCREENSRC)/config.h $(SCREENSRC)/list_winw.c $(SCREENSRC)/list_winw.h
	cd $(SCREENSRC); make list_winw.o
$(SCREENSRC)/adav.o:$(SCREENSRC)/config.h $(SCREENSRC)/adav.c $(SCREENSRC)/adav.h
	cd $(SCREENSRC); make adav.o
$(SCREENSRC)/gfile.o:$(SCREENSRC)/config.h $(SCREENSRC)/gfile.c $(SCREENSRC)/gfile.h
	cd $(SCREENSRC); make gfile.o
$(SCREENSRC)/mark.o:$(SCREENSRC)/config.h $(SCREENSRC)/mark.c $(SCREENSRC)/mark.h
	cd $(SCREENSRC); make mark.o
$(SCREENSRC)/misc.o:$(SCREENSRC)/config.h $(SCREENSRC)/misc.c
	cd $(SCREENSRC); make misc.o
$(SCREENSRC)/nethack.o:$(SCREENSRC)/config.h $(SCREENSRC)/nethack.c
	cd $(SCREENSRC); make nethack.o
$(SCREENSRC)/pcess.o:$(SCREENSRC)/config.h $(SCREENSRC)/pcess.c $(SCREENSRC)/pcess.h
	cd $(SCREENSRC); make pcess.o
$(SCREENSRC)/pty.o:$(SCREENSRC)/config.h $(SCREENSRC)/pty.c
	cd $(SCREENSRC); make pty.o
$(SCREENSRC)/putenv.o:$(SCREENSRC)/config.h $(SCREENSRC)/putenv.c
	cd $(SCREENSRC); make putenv.o
$(SCREENSRC)/resize.o:$(SCREENSRC)/config.h $(SCREENSRC)/resize.c
	cd $(SCREENSRC); make resize.o
$(SCREENSRC)/sched.o:$(SCREENSRC)/config.h $(SCREENSRC)/sched.c $(SCREENSRC)/sched.h
	cd $(SCREENSRC); make sched.o
$(SCREENSRC)/screen.o:$(SCREENSRC)/config.h $(SCREENSRC)/screen.c $(SCREENSRC)/screen.h
	cd $(SCREENSRC); make screen.o
$(SCREENSRC)/search.o:$(SCREENSRC)/config.h $(SCREENSRC)/search.c
	cd $(SCREENSRC); make search.o
$(SCREENSRC)/cket.o:$(SCREENSRC)/config.h $(SCREENSRC)/cket.c $(SCREENSRC)/cket.h
	cd $(SCREENSRC); make cket.o
$(SCREENSRC)/teln.o:$(SCREENSRC)/config.h $(SCREENSRC)/teln.c
	cd $(SCREENSRC); make teln.o
$(SCREENSRC)/termcap.o:$(SCREENSRC)/config.h $(SCREENSRC)/termcap.c
	cd $(SCREENSRC); make termcap.o
$(SCREENSRC)/term.o:$(SCREENSRC)/config.h $(SCREENSRC)/term.c $(SCREENSRC)/term.h
	cd $(SCREENSRC); make term.o
$(SCREENSRC)/tty.o:$(SCREENSRC)/config.h $(SCREENSRC)/tty.c 
	cd $(SCREENSRC); make tty.o
$(SCREENSRC)/utmp.o:$(SCREENSRC)/config.h $(SCREENSRC)/utmp.c
	cd $(SCREENSRC); make utmp.o
$(SCREENSRC)/viewrt.o:$(SCREENSRC)/config.h $(SCREENSRC)/viewrt.c $(SCREENSRC)/viewrt.h
	cd $(SCREENSRC); make viewrt.o
$(SCREENSRC)/winw.o:$(SCREENSRC)/config.h $(SCREENSRC)/winw.c $(SCREENSRC)/winw.h
	cd $(SCREENSRC); make winw.o
