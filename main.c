#include <linux/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <signal.h>
#include <termios.h>
struct data{
	char** arg;
	char** env;
	char* initPath;
	char* rootPath;
	char* processName;
	int pipe_fd[2];
};
int ptymaster;
int initpid;
int ptyoutpid;
int ptyinpid;
FILE* ptyinStream;
struct termios config;
void sigHandlerNull(int signum){
	
}
void sigHandlerTerm(int signum){
	if(initpid != -1){
		kill(initpid, SIGKILL);
	}
	
}
/*void sigHandlerMain(int signum){
	// turns out every child uses the same tty, so they all could receive the same signal from that tty
	if(ptyinpid != -1){
		kill(ptyinpid, signum);
	}
}

void sigHandlerPtyin(int signum){
	//printf("ptyin signal\n");
	//if(0){
	if(ptymaster != -1){
		//FILE* ptyin = fdopen(ptymaster, "w");
		//printf("fdopen\n");
		switch(signum){
			case SIGINT:
				fputc(3, ptyinStream);
				break;
			case SIGTSTP:
				fputc(11, ptyinStream);
				break;
		}
		//printf("after fdopen\n");
		//fclose(ptyin);
		//printf("after fclose\n");
	}
}*/
void ptyin(void * input){
	// redirect input
	signal(SIGINT, sigHandlerNull);
	signal(SIGTSTP, sigHandlerNull);
	ptyinStream = fdopen(ptymaster, "w");
	FILE* stdin = fdopen(0, "r");
	while(1){
		char c = fgetc(stdin);
		/*if(c == 3){
			printf("debug: ctrl+c forwarded\n");
		}
		if(c == 32){
			printf("debug: ctrl+z forwarded\n");
		}*/
		fputc(c, ptyinStream);
	}
}
void ptyout(void * input){
	// redirect output
	signal(SIGINT, sigHandlerNull);
	signal(SIGTSTP, sigHandlerNull);
	FILE* ptyout = fdopen(ptymaster, "r");
	char ptyslavePathBuffer[100];
	/*int pts = -1;
	struct termios ttyattr, lastattr;
	if(ptsname_r(ptymaster, ptyslavePathBuffer, 100) == 0){
		pts = open(ptyslavePathBuffer, O_RDWR);
	}else{
		printf("fixme: ptyout could not open pts to clone ttyattr\n");
	}*/
	while(1){
		// copy slave's tty attr to current tty
		int c = fgetc(ptyout);
		/*if(pts != -1){
			tcgetattr(pts, &ttyattr);
			if(!memcmp(&ttyattr, &lastattr, sizeof(struct termios))){
				tcsetattr(0, TCSANOW, &ttyattr);
				lastattr = ttyattr;
			}
		}*/
		if(c != EOF)
			printf("%c", c);
		else
			sleep(2);
	}
}
const char* devices[] = {"null", "zero", "full", "random", "urandom", "tty", "net/tun"};
int launch(void * input){
	struct data * command  = input;
	// sync
	//fclose(command->pipe_fd[0]);
	FILE * pipe_in = fdopen(command->pipe_fd[1], "w");
	fputc('c', pipe_in);
	printf("debug: after putting\n");
	close(pipe_in);
	// set up some mounts before chroot
	int length = strlen(command->rootPath);
	char* paths = malloc(sizeof(char) * (length + 13));
	char* paths2 = malloc(sizeof(char) * (length + 13));
	int perm0755 = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	int result = 0;
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc");
	mkdir(paths, perm0755);
	if(mount("proc", paths, "proc", 0, "") != 0){
		printf("failed mounting proc\n");
		exit(1);
	}
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc/sys");
	strcpy(paths2, paths);
	strcat(paths2, "/sys");
	if(mount("proc", paths, "proc", MS_RDONLY, "") + mount(paths2, paths, 0, MS_BIND, 0) != 0){
		printf("failed making proc/sys readonly\n");
		exit(1);
	}
	strcpy(paths, command->rootPath);
	strcat(paths, "/sys");
	mkdir(paths, perm0755);
	if(mount("sysfs", paths, "sysfs", MS_RDONLY, "") != 0){
		printf("failed mounting sys\n");
		exit(1);
	}
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev");
	mkdir(paths, perm0755);
	if(mount("tmpfs", paths, "tmpfs", 0, "") != 0){
		printf("failed mounting dev tmpfs\n");
		exit(1);
	}
	strcat(paths, "/pts");
	mkdir(paths, perm0755);
	if(mount("devpts", paths, "devpts", 0, "") != 0){
		printf("failed mounting devpts\n");
		exit(1);
	}
	// copy some device nodes, referencing systemd-nspawn
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev/net");
	mkdir(paths, perm0755);
	//struct stat buffer;
	int i;
	// for some unknown reasons, mknod does not work, only empty files were created
	// resolving to bindmounts
	for(i = 0;i < 7;i++){
		strcpy(paths, command->rootPath);
		strcat(paths, "/dev/");
		strcat(paths, devices[i]);
		//stat(paths, &buffer);
		//mknod(paths, buffer.st_mode, buffer.st_rdev);
		strcpy(paths2, "/dev/");
		strcat(paths2, devices[i]);
		FILE * file = fopen(paths, "w");
		close(file);
		if(mount(paths2, paths, 0, MS_BIND , 0) !=0){
			printf("failed mounting %s\n", devices[i]);
			exit(1);
		}
	}
	free(paths);
	free(paths2);
	// chroot
	if(chroot(command->rootPath) == -1){
		printf("chrooting failed\n");
		printf("path: %s\n", command->rootPath);
		exit(1);
	}
	symlink("/dev/pts/ptmx", "/dev/ptmx");
	// now bind pty outputs created by parent
	FILE * devConsole = fopen("/dev/console", "w");
	close(devConsole);
	mount("/console", "/dev/console", 0, MS_BIND, 0);
	//symlink("/console", "/dev/tty0");
	// according to jchroot sources, sharing files could lead to different chroot escape
	// I better read more on chroot and security at some point...
	unshare(CLONE_FILES);
	// start init
	chdir("/");
	int pid = execvpe(command->initPath, command->arg, command->env);
	if(pid == -1){
		printf("failed to execute init %s\n", command->initPath);
		exit(1);
	}
}
void printUsage(){
	printf("Usage: minicontainer -p root path [-i custom init path inside root=/sbin/init] [-r runlevel=3] [-n custom child process name=minicontainer_inside]\n");
	return;
}
int main(int argc, char** argv){
	ptymaster = -1;
	ptyinpid = -1;
	ptyoutpid = -1;
	initpid = -1;
	// check if the current user is root
	if(getuid() != 0){
		printf("this program needs to be executed as root\n");
		exit(1);
	}
	// check if program is being executed in a tty
	if(!isatty(0)){
		printf("this program needs to be execute with tty or a pty\n");
		exit(1);
	}
	// capture tty attributes
	if(tcgetattr(0, &config) < 0){
		printf("failed saving tty attributes\n");
		exit(1);
	}
	struct data * command = malloc(sizeof(struct data));
	command->arg = malloc(sizeof(char*) * 3);
	command->arg[0] = malloc(sizeof(char) * 11);
	strcpy(command->arg[0], "/sbin/init");
	command->arg[1] = malloc(sizeof(char) * 2);
	command->arg[1][0] = '3';
	command->arg[1][1] = '\0';
	//debug
	//free(command->arg[1]);
	//command->arg[1] = 0;
	//debug
	command->arg[2] = 0;
	command->env = malloc(sizeof(char*) * 2);
	command->env[0] = malloc(sizeof(char) * 6);
	strcpy(command->env[0], "PATH=");
	command->env[1] = malloc(sizeof(char) * 16);
	strcpy(command->env[1], "container=mini");
	command->initPath = malloc(sizeof(char) * 11);
	strcpy(command->initPath, "/sbin/init");
	command->rootPath = 0;
	command->processName = malloc(sizeof(char) * 21);
	strcpy(command->processName, "minicontainer_inside");
	// parse command
	// -p for root path, -i for custom init path, -r for run level, -n for process name
	char opt;
	int length;
	while((opt = getopt(argc, argv, "p:i:r:n:")) != -1){
		switch(opt){
			case 'p':
				length = strlen(optarg);
				if(length == 0){
					printf("empty root path\n");
					printUsage();
					exit(1);
				}
				command->rootPath = malloc(sizeof(char) * (length + 1));
				strcpy(command->rootPath, optarg);
				break;
			case 'i':
				length = strlen(optarg);
				if(length == 0){
					printf("empty init path\n");
					printUsage();
					exit(1);
				}
				free(command->initPath);
				command->initPath = malloc(sizeof(char) * (length + 1));
				strcpy(command->initPath, optarg);
				break;
			case 'r':
				length = strlen(optarg);
				if(length == 0){
					printf("empty runlevel\n");
					printUsage();
					exit(1);
				}
				free(command->arg[1]);
				command->arg[1] = malloc(sizeof(char) * (length + 1));
				strcpy(command->arg[1], optarg);
				break;
			case 'n':
				length = strlen(optarg);
				if(length == 0){
					printf("empty process name\n");
					printUsage();
					exit(1);
				}
				free(command->processName);
				command->processName = malloc(sizeof(char) * (length + 1));
				strcpy(command->processName, optarg);
				break;

			default:
				printUsage();
				exit(1);
				break;
		}
	}
	if(command->rootPath == 0){
		printf("you must specify a root path\n");
		printUsage();
		exit(1);
	}
	if(strcmp(command->rootPath, "/") == 0){
		printf("don't think using root as a chroot is a good idea....\n");
		exit(1);
	}
	DIR* dir = opendir(command->rootPath);
	if(!dir){
		printf("given path %s is not accessible\n", command->rootPath);
		exit(1);
	}
	closedir(dir);
	// create new ptx and bind to /dev/console, hope that it works
	//ptymaster = posix_openpt(O_RDWR|O_NOCTTY /*|O_CLOEXEC|O_NDELAY*/);
	ptymaster = getpt();
	if(ptymaster < 0){
		printf("failed opening pty\n");
		exit(0);
	}
	char pathBuffer[50];
	char fullPathBuffer[strlen(command->rootPath) + 50];
	if(unlockpt(ptymaster) < 0){
		printf("failed unlocking pty\n");
		exit(0);
	}
	if(ptsname_r(ptymaster, pathBuffer) != 0){
		printf("failed getting pts path\n");
		exit(0);
	}
	// slave to raw
	int ptyslave = open(pathBuffer, O_RDWR);
	struct termios ptsattr;
	if(tcgetattr(ptyslave, &ptsattr) < 0){
		printf("failed getting pty slave's attributes\n");
		exit(0);
	}
	cfmakeraw(&ptsattr);
	if(tcsetattr(ptyslave, TCSANOW, &ptsattr) < 0){
		printf("failed setting pty slave's attributes\n");
		exit(0);
	}
	close(ptyslave);
	strcpy(fullPathBuffer, command->rootPath);
	strcat(fullPathBuffer, "/console");
	FILE * newFile = fopen(fullPathBuffer, "w");
	close(newFile);
	printf("debug: %s\n", pathBuffer);
	if(mount(pathBuffer, fullPathBuffer, 0, MS_BIND, 0) < 0){
		printf("failed binding %s as %s\n", pathBuffer, fullPathBuffer);
		exit(0);
	}
	void * ptyinstack = malloc(sysconf(_SC_PAGESIZE)); 
	ptyinpid = clone(ptyin, ptyinstack + sysconf(_SC_PAGESIZE), CLONE_VM, 0);
	void * ptyoutstack = malloc(sysconf(_SC_PAGESIZE));
	ptyoutpid = clone(ptyout, ptyoutstack + sysconf(_SC_PAGESIZE), CLONE_VM, 0);
	if(ptyinpid == -1 || ptyoutpid == -1){
		printf("failed opening pty\n");
		kill(ptyinpid, SIGKILL);
		kill(ptyoutpid, SIGKILL);
		exit(0);
	}
	printf("debug: ptyinpid %d\n", ptyinpid);
	printf("debug: ptyoutpid %d\n", ptyoutpid);
	// handle signals of ctrl+c and ctrl+z
	signal(SIGINT, sigHandlerNull);
	signal(SIGTSTP, sigHandlerNull);
	
	// prepare pipe to sync with child process
	if(pipe(command->pipe_fd) != 0){
		printf("failed creating pipe\n");
		exit(1);
	}
	// start child process
	void * stack = malloc(sysconf(_SC_PAGESIZE));
	
	//int cloneFlags = CLONE_NEWPID;
	int cloneFlags = CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC | CLONE_VM | CLONE_NEWUTS;
	initpid = clone(launch, stack + sysconf(_SC_PAGESIZE), cloneFlags, command);
	if(initpid == -1){
		printf("failed to create child process\n");
		exit(1);
	}
	printf("debug: child pid is  %d\n", initpid);
	printf("debug: my pid is %d\n", getpid());
	// sync with child
	close(command->pipe_fd[1]);
	FILE * pipe_out = fdopen(command->pipe_fd[0], "r");
	fgetc(pipe_out);
	printf("debug: after getting\n");
	close(pipe_out);
	int status;
	// now handle SIGTERM by killing the init
	signal(SIGTERM, sigHandlerTerm);
	// change tty to raw mode
	struct termios raw = config;
	raw.c_lflag &= ~(ISIG | ICANON | ECHO);
	raw.c_oflag &= ~(NLDLY | CRDLY| TABDLY | BSDLY | VTDLY | FFDLY);
	//cfmakeraw(&raw);
	if(tcsetattr(0, TCSANOW, &raw) < 0){
		printf("failed setting tty to new mode\n");
		exit(1);
	}
	if(waitpid(initpid, &status, 0) == -1){
		printf("something went wrong while waiting for the child to exit\n");
		// reset tty to cooked mode
		tcsetattr(0, TCSANOW, &config);
	}
	printf("wrapping up\n");
	// wrap up
	// kill both pty in and out loop
	kill(ptyinpid, SIGKILL);
	kill(ptyoutpid, SIGKILL);
	free(stack);
	// unmount file systems and remove devices
	length = strlen(command->rootPath);
	char* paths = malloc(sizeof(char) * (length + 13));
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc");
	umount2(paths, MNT_DETACH);
	strcpy(paths, command->rootPath);
	strcat(paths, "/sys");
	umount2(paths, MNT_DETACH);
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev/pts");
	umount2(paths, MNT_DETACH);
	int i;
	for(i = 0;i < 7;i++){
		strcpy(paths, command->rootPath);
		strcat(paths, "/dev");
		strcat(paths, devices[i]);
		// switching to bind mounts for those devices
		//remove(paths);
		umount2(paths, MNT_DETACH);
	}
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev");
	umount2(paths, MNT_DETACH);
	strcpy(paths, command->rootPath);
	strcat(paths, "/run");
	umount2(paths, MNT_DETACH);
	// unbind /dev/console from the pty
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev/console");
	umount2(paths, MNT_DETACH);
	strcpy(paths, command->rootPath);
	strcat(paths, "/console");
	umount2(paths, MNT_DETACH);
	free(paths);
	// restore tty to cooked mode
	tcsetattr(0, TCSANOW, &config);
	if(!WIFEXITED(status)){
		exit(1);
	}
	return 0;
}

