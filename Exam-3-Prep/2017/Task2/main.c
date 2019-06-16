#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

//awk -F: '{print $7}' /etc/passwd | sort | uniq -c | sort -n -k1

int main() {
	
	int pipe1[2];

	if (pipe(pipe1) == -1)
	{
		err(1, "Couldn't create pipe1");
	}	

	if (fork() == 0)
       	{
		close(pipe1[0]);
		dup2(pipe1[1], 1);
                execlp("awk", "awk", "-F", ":", "{print $7}",  "/etc/passwd", (char*)NULL);
	}

	close(pipe1[1]);

	int pipe2[2];

	if (pipe(pipe2) == -1)
	{
		err(1, "Coulnd't create pipe2");
	}

	if (fork() == 0)
	{
		close(pipe2[0]);
		dup2(pipe1[0], 0);
		dup2(pipe2[1], 1);
		execlp("sort", "sort", (char*)NULL);
	}
	
	close(pipe2[1]);

	int pipe3[2];

	if (pipe(pipe3) == -1) {
		err(1, "Unable to create pipe3");
	}

	if (fork() == 0) {

		close(pipe3[0]);
		dup2(pipe2[0], 0);
		dup2(pipe3[1], 1);
		execlp("uniq", "uniq", "-c", (char*)NULL);
	}

	close(pipe3[1]);

	int pipe4[2];

	if (pipe(pipe4) == -1) {
		err(1, "Coulnd't open pipe4");
	}

	if (fork() == 0) {
		close(pipe4[0]);
		dup2(pipe3[0], 0);
		execlp("sort", "sort", "-n" ,"-k1", (char*)NULL);
	}

	close(pipe4[1]);
	exit(0);	
	exit(0);
}
