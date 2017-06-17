#include <linux/sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
struct data{
	char** arg;
	char** env;
	char* initPath;
	char* rootPath;
	char* processName;
	int pipe_fd[2];
};

int launch(void * input){
	// chroot
	struct data * command  = input;
	// sync
	//fclose(command->pipe_fd[0]);
	FILE * pipe_in = fdopen(command->pipe_fd[1], "w");
	fputc('c', pipe_in);
	printf("after putting\n");
	close(pipe_in);
	// according to jchroot sources, sharing files could lead to different chroot escape
	// I better read more on chroot and security at some point...
	unshare(CLONE_FILES);

	// mount file systems (moved to main thread)
	//mount("proc", "/proc", "proc", 0, "");
	//mount("sysfs", "/sys", "sysfs", 0, "");
	//mount("devtmpfs", "/dev", "devtmpfs", 0, "");
	//mount("devpts", "/dev/pts", "devpts", 0, "");
	// start init
	chdir("/");
	int pid =execvpe(command->initPath, command->arg, command->env);
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
	// check if the current user is root
	if(getuid() != 0){
		printf("this program needs to be executed as root\n");
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
	free(command->arg[1]);
	command->arg[1] = 0;
	//debug
	command->arg[2] = 0;
	command->env = malloc(sizeof(char*));
	command->env[0] = 0;// empty environment
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
	if(pipe(command->pipe_fd) != 0){
		printf("failed creating pipe");
		exit(1);
	}
	// set up some mounts before chroot
	length = strlen(command->rootPath);
	char* paths = malloc(sizeof(char) * (length + 13));
	int perm0755 = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	strcpy(paths, command->rootPath);
	strcat(paths, "/proc");
	mkdir(paths, perm0755);
	mount("proc", paths, "proc", 0, "");
	strcpy(paths, command->rootPath);
	strcat(paths, "/sys");
	mkdir(paths, perm0755);
	mount("sys", paths, "sys", 0, "");
	strcpy(paths, command->rootPath);
	strcat(paths, "/dev");
	mkdir(paths, perm0755);
	mount("tmpfs", paths, "tmpfs", 0, "");
	strcat(paths, "/pts");
	mkdir(paths, perm0755);
	mount("devpts", paths, "devpts", 0, "");
	// copy some device nodes, referencing systemd-nspawn
	char** devices = {"/null", "/zero", "/full", "/random", "/urandom", "/tty", "/net/tun"};
	strcpy(paths, command->rootPath);
	strcat(paths, "/net");
	mkdir(paths, perm0755);
	struct stat buffer;
	int i;
	for(i = 0;i < 7;i++){
		strcpy(paths, command->rootPath);
		strcat(paths, "/dev");
		strcat(paths, devices[i]);
		stat(paths, &buffer);
		mknod(paths, buffer.st_mode, buffer.st_rdev);
	}
	free(paths);
	// chroot
	int pid;
	void * stack = malloc(sysconf(_SC_PAGESIZE));
	if(chroot(command->rootPath) == -1){
		printf("chrooting failed\n");
		printf("path: %s\n", command->rootPath);
		exit(1);
	}
	pid = clone(launch, stack + sysconf(_SC_PAGESIZE), CLONE_NEWIPC | CLONE_NEWPID | CLONE_NEWNS, command);
	if(pid == -1){
		printf("failed to create child process\n");
		exit(1);
	}
	// sync with child
	close(command->pipe_fd[1]);
	FILE * pipe_out = fdopen(command->pipe_fd[0], "r");
	fgetc(pipe_out);
	printf("after getting\n");
	close(pipe_out);
	int status;
	if(waitpid(pid, &status, 0) == -1){
		printf("something went wrong while waiting for the child to exit\n");
		exit(1);
	}
	// unmount file systems and remove devices
	umount("/proc");
	umount("/sys");
	umount("/dev/pts");
	for(i = 0;i < 7;i++){
		paths = malloc(sizeof(char) * 13);
		strcpy(paths, "/dev");
		strcat(paths, devices[i]);
		remove(paths);
	}
	umount("/dev");
	if(!WIFEXITED(status)){
		exit(1);
	}
	return 0;
}

