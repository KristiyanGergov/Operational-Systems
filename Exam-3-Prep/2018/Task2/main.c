//find . -printf "%T@ %Tc %p\n" | sort -nclear

#include <err.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char** argv) {

	if ( argc != 2 ) {
		err(2, "Invalid number of arguments. Usage %s <string>: ", argv[0]);
	}
	
	int pipe1[2];

	if (pipe(pipe1) == -1) {
		err(1, "Couldn't open pipe1");
	}

	if (fork() == 0) {
		close(pipe1[0]);
		dup2(pipe1[1], 1);
		execlp("find", "find", argv[1], "-printf", "%T@ %Tc %p\n", (char*)NULL);
	}
	
	close(pipe1[1]);

	exit(0);
	wait(NULL);
	return 0;
}
