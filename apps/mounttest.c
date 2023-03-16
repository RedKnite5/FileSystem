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
  char buf[10];

	/* Mount disk */
	diskname = argv[1];
	ret = fs_mount(diskname);
	ASSERT(!ret, "fs_mount");
  fs_info();

  fs_create("Roger");
  int fd_Is = fs_create("Is");
  fs_create("Cool");
  fs_ls();
  fs_delete("Roger");
  fs_ls();
  fs_read(fd_Is, buf, 10);
	fs_umount();

	return 0;
}
