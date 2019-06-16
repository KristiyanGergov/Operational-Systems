#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

int main(int argc, char** argv) {

	if (argc != 4) {
		err(1, "Invalid number of arguments. Usage %s <string>", argv[0]);
	}

	int fdOrig, fdCopy, fdPatch;

	fdOrig = open(argv[1], O_RDONLY);
	
	if (fdOrig == -1) {
		err(2, "Failed while opening file: %s", argv[1]);
	}
		
	fdCopy = open(argv[2], O_RDONLY);
	
	if (fdCopy == -1) {
		close(fdOrig);
		err(2, "Failed while opening file: %s", argv[2]);
	}
	
	fdPatch = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);

	if (fdOrig == -1) {
		close(fdOrig);
		close(fdCopy);
		err(2, "Failed while opening file: %s", argv[3]);
	}

	uint8_t first = 0;
	uint8_t second = 0;
	uint16_t offset = 0;

	while (read(fdOrig, &first, 1) > 0 && read(fdCopy, &second, 1) > 0) {
		
		if (first != second) {
			write(fdPatch, &offset, 2);
			write(fdPatch, &first, 1);
			write(fdPatch, &second, 1);
		}

		offset++;
	}


	close(fdOrig);
	close(fdCopy);
	close(fdPatch);
}
