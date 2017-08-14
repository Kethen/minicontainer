#include<linux/sched.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

int test(void * d){
	struct toChild* data = d;
	return 0;
}

int main(int argc, char** argv){
	if(getuid() != 0){
		printf("namespaces can be created by root only!\n");
		exit(1);
	}
	int pid;
	int stackSize = sysconf(_SC_PAGESIZE);
	void * stack = malloc(stackSize);
	pid = clone(test, stack + stackSize,CLONE_NEWNS, 0);
	if(pid == -1){
		printf("does not support CLONE_NEWNS...\n");
	}else{
		printf("supports CLONE_NEWNS\n");
	}
	stack = malloc(stackSize);
	pid = clone(test, stack + stackSize,CLONE_NEWPID, 0);
	if(pid == -1){
		printf("does not support CLONE_NEWPID...\n");
	}else{
		printf("supports CLONE_NEWPID\n");
	}
	stack = malloc(stackSize);
	pid = clone(test, stack + stackSize,CLONE_NEWIPC, 0);
	if(pid == -1){
		printf("does not support CLONE_NEWIPC...\n");
	}else{
		printf("supports CLONE_NEWIPC\n");
	}
	stack = malloc(stackSize);
	pid = clone(test, stack + stackSize,CLONE_NEWUTS, 0);
	if(pid == -1){
		printf("does not support CLONE_NEWUTS...\n");
	}else{
		printf("supports CLONE_NEWUTS\n");
	}
	return 0;
}
