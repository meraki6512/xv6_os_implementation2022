#include "types.h"  // uint, ...
#include "stat.h" // dir, file, dev, stuct stat
#include "user.h" //system calls

int main(int argc, char**argv){

	int pid, mask;

	if (argc<3){
		printf(1, "ERROR: few argument\n");
		exit();
	}

	mask = atoi(argv[1]);
	if (mask<=1){
		printf(1, "ERROR: mask must be larger than 1\n");
		exit();
	}

	trace(mask);
	
	if ((pid = fork())<0){
		printf(1, "ERROR: fork failed\n"); //2?check
		exit();
	}
	else if (pid==0){
		//trace(mask);
		exec(argv[2], &argv[2]);
	}
	else{
		wait();
		exit();
	}

}
