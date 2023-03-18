#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs.h>

#define DATA 10000

#define ASSERT(cond, func)                                                     \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fprintf(stderr, "Function '%s' failed\n", func);                         \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

int main(int argc, char *argv[]) {
  int ret;
  char *diskname;
  int fd;
  char data[DATA];
  char alpha[] = "abcdefghijklmnopqrstuvwxyz";

  for (int i = 0; i < DATA; i++) {
    data[i] = alpha[i % 26];
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
  // ASSERT(!ret, "fs_create");

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
  // printf("read: %d\n", ret);
  ASSERT(ret == sizeof(data), "fs_read");

  for (int i = 0; i < DATA; i++) {
    // printf("%c  %c\n", data[i], buf[i]);
    ASSERT(data[i] == buf[i], "giant read and write");
  }

  fs_close(fd);

  ret = fs_create("file_5");
  ASSERT(!ret, "fs_create");

  fd = fs_open("file_5");
  ASSERT(fd >= 0, "fs_open");

  char short_data1[] = "123456789";
  char short_data2[] = "abcdefghi";

  ret = fs_write(fd, short_data1, sizeof(short_data1));
  ASSERT(ret == sizeof(short_data1), "fs_write");

  ret = fs_write(fd, short_data2, sizeof(short_data2));
  ASSERT(ret == sizeof(short_data2), "fs_write");

  fs_lseek(fd, 0);

  char buf2[20];
  ret = fs_read(fd, buf2, 20);
  printf("ret: %d\n", ret);
  ASSERT(ret == 20, "fs_read");

  for (int i=0; i<20; i++) {
    if (i < 10) {
      ASSERT(short_data1[i] == buf2[i], "double read and write 1");
    } else {
      ASSERT(short_data2[i-10] == buf2[i], "double read and write 2");
    }
  }
  
	/* Close file and unmount */
	fs_close(fd);
	fs_umount();

	return 0;
}
