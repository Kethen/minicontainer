#include "window.h"
#include "process.h"
#include "screen.h"
#include <stdio.h>
#include <string.h>
int main(int argc, char** argv){
	//struct NewWindow * nwin  = malloc(sizeof(struct NewWindow));
	//*nwin = nwin_default;
	//char **args = malloc(sizeof(char*));
	//args[0] = malloc(sizeof(char) * 4);
	//strcpy(args[0], "top");
	//DoScreen("key", args);
	char * args[3];
	args[0] = argv[0];
	args[1] = "top";
	args[2] = 0;
	printf("debug: argc is %d\n", argc);
	int i;
	for(i = 0;i < argc;i++){
		printf("debug: argv[%d] is \"%s\"\n", i, argv[i]);
	}
	//main2(2, args);
	struct NewWindow nwin;
	nwin = nwin_default;
	nwin.args = args;
	MakeWindow(&nwin);
}
