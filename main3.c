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
#include <mntent.h>
#include <config.h>
#include <screen.h>
struct data{
	char** arg;
	char** env;
	char* initPath;
	char* rootPath;
	char* processName;
	int pipe_fd[2];
};
// stores the init pid for sigterm to kill it
int initpid; 
void defaultArgv2(char*** argv2p, char** argv, int* argc2p, int argc){
	char** argv2;
	*argv2p = argv2 = malloc(sizeof(char*) * (argc + 5));
	*argc2p = 5;
	argv2[0] = malloc((strlen(argv[0]) + 1) * sizeof(char));
	strcpy(argv2[0], argv[0]);
	argv2[1] = malloc(3 * sizeof(char));
	strcpy(argv2[1], "-S");
	argv2[2] = malloc(sizeof(char) * 2);
	strcpy(argv2[2], "");
	argv2[3] = malloc((strlen(argv[0]) + 1) * sizeof(char));
	strcpy(argv2[3], argv[0]);
	argv2[4] = malloc(sizeof(char) * 3);
	strcpy(argv2[4], "-h");
	return;
}
void freeArgv2(char** argv2, int argc2){
	int i;
	for(i = 0;i < argc2;i++){
		free(argv2[i]);
	}
	free(argv2);
	return;
}
void defaultData(struct data** commandp){
	struct data * command;
	command = *commandp = malloc(sizeof(struct data));
	command->arg = malloc(sizeof(char*) * 3);
	command->arg[0] = malloc(sizeof(char) * 14);
	strcpy(command->arg[0], "minicontainer");
	command->arg[1] = malloc(sizeof(char) * 2);
	command->arg[1][0] = '3';
	command->arg[1][1] = '\0';
	command->arg[2] = 0;
	command->env = malloc(sizeof(char*));
	command->env[0] = malloc(sizeof(char) * 15);
	strcpy(command->env[0], "container=mini");
	command->initPath = malloc(sizeof(char) * 11);
	strcpy(command->initPath, "/sbin/init");
	command->rootPath = 0;
	command->processName = malloc(sizeof(char) * 21);
	strcpy(command->processName, "minicontainer_inside");
	return;
}
void freeData(struct data* command){
	int i = 0;
	while(command->arg[i] != 0){
		free(command->arg[i]);
		i++;
	}
	free(command->arg);
	i = 0;
	while(command->arg[i] != 0){
		free(command->arg[i]);
		i++;
	}
	free(command->env);
	free(command->initPath);
	free(command->rootPath);
	free(command->processName);
	free(command);
	return;
}
// structure and function for mount point cleaning
struct mountPoint{
	struct mountPoint* last;
	struct mountPoint* next;
	char* path;
};
void cleanMount(char *rootPath){
	// first get all mount points
	struct mntent* mntline;
	FILE* mounts = setmntent("/proc/mounts", "r");
	if(mounts == 0){
		printf("error loading /proc/mounts\n");
	}
	struct mountPoint* listTail = 0;
	struct mountPoint* new = 0;
	while((mntline = getmntent(mounts)) != 0){
		if(strstr(mntline->mnt_dir, rootPath) != mntline->mnt_dir && strcmp(rootPath, mntline->mnt_dir) != 0){
			continue;
		}
		new = malloc(sizeof(struct mountPoint));
		new->last = listTail;
		listTail = new;
		new->path = malloc((strlen(mntline->mnt_dir) + 1) * sizeof(char));
		strcpy(new->path, mntline->mnt_dir);
	}
	endmntent(mounts);
	// umount and consume the list from tail
	while(listTail != 0){
		if(umount2(listTail->path, MNT_DETACH) != 0){
			printf("failed unmounting %s", listTail->path);
		}
		free(listTail->path);
		new = listTail;
		listTail = listTail->last;
		free(new);
	}
	return;
}
// if termination, set a flag to skip termination messages
int termination = 0;
void sigHandlerTerm(int signum){
	if(initpid != -1){
		kill(initpid, SIGKILL);
		termination = 1;
	}
	return;
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
	free(paths);
	free(paths2);
	// chroot
	if(chroot(command->rootPath) == -1){
		printf("chrooting failed\n");
		printf("path: %s\n", command->rootPath);
		exit(1);
	}
	symlink("/dev/pts/ptmx", "/dev/ptmx");
	// now symlink pty outputs created by parent
	symlink("/console", "/dev/console");
	// according to jchroot sources, sharing files could lead to different chroot escape
	// I better read more on chroot and security at some point...
	unshare(CLONE_FILES);
	// start init
	chdir("/");
	if(execvpe(command->initPath, command->arg, command->env) == -1){
		printf("failed to execute init %s\n", command->initPath);
		exit(1);
	}
}
void printUsage(){
	printf("Usage:\n");
	printf("minicontainer -p root path \\\n");
	printf("\t[-i init=/sbin/init] \\\n"); 
	printf("\t[-r runlevel=3] \\\n");
	printf("\t[-n custom child process name and screen window name] \\\n");
	printf("\t[-s alternate screen binary] \\\n"); 
	printf("\t[-a [pid or name, relays to screen -r for reattachment]] \\\n");
	printf("\t[-d pid or name, relays to screen -d for detachment] \\\n");
	printf("\t[-h]\n");
	printf("\n");
	printf("-n : inits could change their process name however\n");
	printf("-h : will skip starting a new screen then take current tty over directly. This method usually result in sighup on parent and kills the parent. This flag was made for self launching inside a screen\n");

	
	return;
}
void addToArg(char opt, char* arg, char** argv, int* count){
	argv[*count] = malloc(sizeof(char) * 3);
	argv[*count][0] = '-';
	argv[*count][1] = opt;
	argv[*count][2] = '\0';
	(*count)++;
	argv[*count] = malloc(sizeof(char) * (strlen(arg) + 1));
	strcpy(argv[*count], arg);
	(*count)++;
	return;
}
void reassignString(char** dest, char* src){
	free(*dest);
	*dest = malloc(sizeof(char) * (strlen(src) + 1));
	strcpy(*dest, src);
	return;
}
int main(int argc, char** argv){
	//printf("debug: sleeping 10 seconds for gdb to be attached. my pid is %d\n", getpid());
	//sleep(10);
	int i;
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
	struct data * command = 0;
	defaultData(&command);
	// parse command
	// -p for root path, -i for custom init path, -r for run level, -n for process name
	char opt;
	int length;
	// make a copy of arguments if we're starting in screen mode
	int argc2 = 0;
	char** argv2 = 0;
	defaultArgv2(&argv2, argv, &argc2, argc);
	// flag for starting in screen mode
	int screen = 1;
	// screen binary path / custom screen start command
	char* screenBinary = 0;
	// flag for reattach mode
	int reattach = 0;
	char* reattachString = 0;
	// flag for changed name
	int nameChange = 0;
	// flag for detach mode
	int detach = 0;
	char* detachString;
	while((opt = getopt(argc, argv, ":p:i:r:n:s:a:d:h")) != -1){
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
				if(length < MAXSTR){
					free(argv2[2]);
					argv2[2] = malloc(sizeof(char) * (length + 1));
					strcpy(argv2[2], optarg);
					for(i = 0;i < length;i++){
						if(argv2[2][i] == '/'){
							argv2[2][i] = '\\';
						}
					}
				}
				addToArg(opt, optarg, argv2, &argc2);
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
				addToArg(opt, optarg, argv2, &argc2);
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
				addToArg(opt, optarg, argv2, &argc2);
				break;
			case 'n':
				length = strlen(optarg);
				if(length == 0){
					printf("empty process name\n");
					printUsage();
					exit(1);
				}
				nameChange = 1;
				free(command->processName);
				command->processName = malloc(sizeof(char) * (length + 1));
				strcpy(command->processName, optarg);
				addToArg(opt, optarg, argv2, &argc2);
				break;
			case 'h':
				screen = 0;
				break;
			case 's':
				length = strlen(optarg);
				if(length == 0){
					printf("empty custom screen command\n");
					printUsage();
					exit(1);
				}
				screenBinary = malloc(sizeof(char)*(length + 1));
				strcpy(screenBinary, optarg);
				break;
			case 'a':
				reattach = 1;
				length = strlen(optarg);
				if(length != 0){
					reattachString = malloc(sizeof(char) * (length + 1));
					strcpy(reattachString, optarg);
				}
				break;
			case 'd':
				detach = 1;
				length = strlen(optarg);
				if(length != 0){
					detachString = malloc(sizeof(char) * (length + 1));
					strcpy(detachString, optarg);
				}
				break;
			case ':':
				if(optopt != 'a'){
					printUsage();
					exit(1);
				}else{
					reattach = 1;
				}
				break;
			default:
				printUsage();
				exit(1);
				break;
		}
	}
	// cap the newly created argument copy
	argv2[argc2] = 0;
	// handle signals of ctrl+c and ctrl+z as well as tty takeover
	struct sigaction sigActionIgnore;
	sigActionIgnore.sa_handler = SIG_IGN;
	sigemptyset(&sigActionIgnore.sa_mask);
	sigActionIgnore.sa_flags = 0;
	sigActionIgnore.sa_restorer = 0;
	sigaction(SIGINT, &sigActionIgnore, 0);
	sigaction(SIGTSTP, &sigActionIgnore, 0);
	sigaction(SIGHUP, &sigActionIgnore, 0);
	// run in reattach mode
	if(reattach){
		reassignString(&(argv2[1]), "-r");
		if(reattachString != 0)
			reassignString(&(argv2[2]), reattachString);
		else{
			free(argv2[2]);
			argv2[2] = 0;
		}
		free(argv2[3]);
		argv2[3] = 0;
		if(screenBinary){
			reassignString(&(argv2[0]), screenBinary);
			execvp(screenBinary, argv2);
		}else{
			main2(reattachString == 0 ? 2 : 3, argv2);
			exit(0);
		}
	}
	// detach mode
	if(detach){
		reassignString(&(argv2[1]), "-d");
		reassignString(&(argv2[2]), detachString);
		free(argv2[3]);
		argv2[3] = 0;
		if(screenBinary){
			reassignString(&(argv2[0]), screenBinary);
			execvp(screenBinary, argv2);
		}else{
			main2(3, argv2);
			exit(0);
		}
	}
	// check root path
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
	// run in screen mode or pty highjack mode
	if(screen){
		if(nameChange){
			reassignString(&(argv2[2]), command->processName);
		}
		// custom binary or embedded
		if(screenBinary){
			reassignString(&(argv2[0]), screenBinary);
			if(execvp(screenBinary, argv2) == -1){
				printf("cannot find specified binary %s\n", screenBinary);
			}
			exit(0);
		}else{
			//printf("debug: argv2[1] is %s\n", argv2[1]);
			//printf("debug: argc2 is %d\n", argc2);
			main2(argc2, argv2);
			exit(0);
		}
	}
	// we don't need argv2 anymore
	freeArgv2(argv2, argc2);
	// bind the current tty to /dev/console
	char* ttynameBuffer = malloc(sizeof(char) * 50);
	char* fullPathBuffer = malloc(sizeof(char) * (strlen(command->rootPath) + 12));
	if(ttyname_r(0, ttynameBuffer, 50) != 0){
		printf("failed getting tty name\n");
		exit(1);
	}
	strcpy(fullPathBuffer, command->rootPath);
	strcat(fullPathBuffer, "/console");
	FILE* console = fopen(fullPathBuffer, "w");
	close(console);
	mount(ttynameBuffer, fullPathBuffer, 0, MS_BIND, 0);
	free(fullPathBuffer);
	// prepare pipe to sync with child process
	if(pipe(command->pipe_fd) != 0){
		printf("failed creating pipe\n");
		exit(1);
	}
	// start child process
	void * stack = malloc(sysconf(_SC_PAGESIZE));
	// log output for debug after tty takeover
	// FILE* log = fopen("/tmp/minicontainerlog", "w");

	//int cloneFlags = CLONE_NEWPID;
	int cloneFlags = CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWUTS | CLONE_VM;
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
	struct sigaction sigActionTerm;
	sigActionTerm.sa_handler = sigHandlerTerm;
	sigemptyset(&sigActionTerm.sa_mask);
	sigActionTerm.sa_flags = 0;
	sigActionTerm.sa_restorer = 0;
	sigaction(SIGTERM, &sigActionTerm, 0);
	// wait for child to finish
	printf("debug: initpid is %d\n", initpid);
	int waitpidRv;
	if((waitpidRv = waitpid(initpid, &status, 0)) == -1){
		//int errorNumber = errno;
		printf("something went wrong while waiting for the child to exit\n");
		//fprintf(log, "something went wrong while waiting for the child to exit\n");
		//fprintf(log, "waitpidRv is %d\n", waitpidRv);
		//fprintf(log, "errno is %d\n", errorNumber);
		//fprintf(log, "EINVAL %d, ECHILD %d, EINTR %d, SIGHUP %d\n", EINVAL, ECHILD, EINTR, SIGHUP);
	}
	// try reconnecting to tty
	close(0);
	close(1);
	close(2);
	int ttyfd = open(ttynameBuffer, O_RDWR);
	dup(ttyfd);
	dup(ttyfd);
	dup(ttyfd);
	printf("wrapping up\n");
	free(ttynameBuffer);
	//fprintf(log, "wrapping up\n");
	// wrap up
	free(stack);
	// unmount file systems and remove devices
	/*length = strlen(command->rootPath);
	char* paths = malloc(sizeof(char) * (length + 13));
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc/sys");
	// unmount twice, first time strip off the bind mount, second time the ro mount
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
	free(paths);*/
	cleanMount(command->rootPath);
	//close(log);
	if(!termination){
		printf("finished, press any key then <return> to continue\n");
		getchar();
		printf("goodbye!\n");
	}
	if(!WIFEXITED(status)){
		exit(1);
	}
	return 0;
}

