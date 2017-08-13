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
//int ptymaster;
int initpid;
//int ptyoutpid;
//int ptyinpid;
//FILE* ptyinStream;
struct termios config;
void sigHandlerMain(int signum){
	/*if(ptyinpid != -1){
		kill(ptyinpid, signum);
	}*/
}
void sigHandlerTerm(int signum){
	if(initpid != -1){
		kill(SIGKILL, initpid);
	}
	
}
/*void sigHandlerPtyin(int signum){
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
/*void ptyin(void * input){
	// redirect input
	signal(SIGINT, sigHandlerPtyin);
	signal(SIGTSTP, sigHandlerPtyin);
	ptyinStream = fdopen(ptymaster, "w");
	FILE* stdin = fdopen(0, "r");
	while(1){
		fputc(fgetc(stdin), ptyinStream);
	}
}
void ptyout(void * input){
	// redirect output
	FILE* ptyout = fdopen(ptymaster, "r");
	while(1){
		int c = fgetc(ptyout);
		if(c != EOF)
		printf("%c", c);
	}
}*/
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
	char paths[length + 13];
	char paths2[length + 13];
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
		printf("failed mounting proc\n");
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
	// chroot
	if(chroot(command->rootPath) == -1){
		printf("chrooting failed\n");
		printf("path: %s\n", command->rootPath);
		exit(1);
	}
	symlink("/dev/pts/ptmx", "/dev/ptmx");
	// now symlink pty outputs created by parent
	symlink("/console", "/dev/console");
	//symlink("/console", "/dev/tty0");
	// according to jchroot sources, sharing files could lead to different chroot escape
	// I better read more on chroot and security at some point...
	unshare(CLONE_FILES);
	// start init
	chdir("/");
	// close current stdin, stdout, stderr
	//char ttynameBuffer[50];
	//ttyname_r(0, ttynameBuffer, 50);
	//close(0);
	//close(1);
	//close(2);
	int pid = execvpe(command->initPath, command->arg, command->env);
	//int ttyfd = open(ttynameBuffer, O_RDWR);
	//dup(ttyfd);
	//dup(ttyfd);
	//dup(ttyfd);
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
	//ptymaster = -1;
	//ptyinpid = -1;
	//ptyoutpid = -1;
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
	command->arg[0] = malloc(sizeof(char) * 14);
	strcpy(command->arg[0], "minicontainer");
	command->arg[1] = malloc(sizeof(char) * 2);
	command->arg[1][0] = '3';
	command->arg[1][1] = '\0';
	//debug
	//free(command->arg[1]);
	//command->arg[1] = 0;
	//debug
	command->arg[2] = 0;
	command->env = malloc(sizeof(char*));
	command->env[0] = malloc(sizeof(char) * 15);
	strcpy(command->env[0], "container=mini");
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
	/*ptymaster = getpt();
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
	}*/
	/*int ptyslave = open(pathBuffer, O_RDWR);
	struct termios ptsattr;
	tcgetattr(ptyslave, &ptsattr);
	cfmakeraw(&ptsattr);
	tcsetattr(ptyslave, TCSANOW, &ptsattr);
	close(ptyslave);*/
	/*strcpy(fullPathBuffer, command->rootPath);
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
		kill(SIGKILL, ptyinpid);
		kill(SIGKILL, ptyoutpid);
		exit(0);
	}*/
	// handle signals of ctrl+c and ctrl+z
	signal(SIGINT, sigHandlerMain);
	signal(SIGTSTP, sigHandlerMain);
	
	// bind the current tty to /dev/console
	char ttynameBuffer[50];
	char fullPathBuffer[strlen(command->rootPath) + 12];
	if(ttyname_r(0, ttynameBuffer, 50) != 0){
		printf("failed getting tty name\n");
		exit(1);
	}
	strcpy(fullPathBuffer, command->rootPath);
	strcat(fullPathBuffer, "/console");
	FILE* console = fopen(fullPathBuffer, "w");
	close(console);
	mount(ttynameBuffer, fullPathBuffer, 0, MS_BIND, 0);
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
	/*struct termios raw = config;
	//raw.c_lflag &= ~ICANON;
	//raw.c_iflag &= ~ECHO;
	//cfmakeraw(&raw);
	if(tcsetattr(0, TCSANOW, &raw) < 0){
		printf("failed setting tty to non cannon\n");
		exit(1);
	}*/
	// close connection to tty before handing it over
	//close(0);
	//close(1);
	//close(2);
	// wait for child to finish
	int initWaitRc = waitpid(initpid, &status, 0);
	//int ttyfd = open(ttynameBuffer, O_RDWR);
	// reconnect to tty
	//dup(ttyfd);
	//dup(ttyfd);
	//dup(ttyfd);
	if(initWaitRc == -1){
		printf("something went wrong while waiting for the child to exit\n");
	}

	printf("wrapping up\n");
	// wrap up
	// stop pty loops
	//kill(ptyinpid, SIGKILL);
	//kill(ptyoutpid, SIGKILL);
	free(stack);
	// unmount file systems and remove devices
	length = strlen(command->rootPath);
	char paths[length + 13];
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc/sys");
	umount2(paths, MNT_DETACH);
	umount2(paths, MNT_DETACH);
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
		strcat(paths, "/dev/");
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
	// restore tty to cooked mode
	tcsetattr(0, TCSANOW, &config);
	if(!WIFEXITED(status)){
		exit(1);
	}
	return 0;
}

