#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs.h>

#define DATA 10000

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
	int fd;
	char data[DATA];
  char alpha[] = "abcdefghijklmnopqrstuvwxyz";

  for (int i=0; i<DATA; i++) {
    data[i] = alpha[i%26];
    if (i % 1000 == 0) {
      data[i] = '0' + (i / 1000) - 1;
    }
  }

	if (argc < 1) {
		printf("Usage: %s <diskimage>\n", argv[0]);
		exit(1);
	}

	/* Mount disk */
	diskname = argv[1];
	ret = fs_mount(diskname);
	ASSERT(!ret, "fs_mount");

	// Too long filename
	ret = fs_create("file_678901234567");
	ASSERT(ret, "fs_create");

	ret = fs_create("Giant_file");
	//ASSERT(!ret, "fs_create");

	fd = fs_open("Giant_file");
	ASSERT(fd >= 0, "fs_open");

  if (!ret) {
  	/* Write some 3 blocks data */
  	ret = fs_write(fd, data, sizeof(data));
  	ASSERT(ret == sizeof(data), "fs_write");
  }

  ret = fs_lseek(fd, 0);
  ASSERT(!ret, "fs_lseek");

  fs_ls();

  char buf[DATA];
  ret = fs_read(fd, buf, sizeof(data));
  printf("read: %d\n", ret);
  ASSERT(ret == sizeof(data), "fs_read");

  for (int i=1; i<DATA; i++) {
    printf("%c  %c\n", data[i], buf[i]);
    ASSERT(data[i-1] == buf[i-1], "read and write");
  }
  
	/* Close file and unmount */
	fs_close(fd);
	fs_umount();

	return 0;
}
