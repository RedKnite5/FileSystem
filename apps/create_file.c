#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs.h>

#define ASSERT(cond, func)                               \
do {                                                     \
	if (!(cond)) {                                       \
		fprintf(stderr, "Function '%s' failed\n", func); \
		exit(EXIT_FAILURE);                              \
	}                                                    \
} while (0)

int main(int argc, char *argv[])
{
	int ret;
	char *diskname;

	if (argc < 1) {
		printf("Usage: %s <diskimage>\n", argv[0]);
		exit(1);
	}

	/* Mount disk */
	diskname = argv[1];
	ret = fs_mount(diskname);
	ASSERT(!ret, "fs_mount");

	/* Create file and open */
	ret = fs_create("myfile");
	ASSERT(!ret, "fs_create");

	ret = fs_create("This_Filename_is_too_long");
	ASSERT(ret, "fs_create");

	fs_umount();

  printf("Success!\n");

	return 0;
}
