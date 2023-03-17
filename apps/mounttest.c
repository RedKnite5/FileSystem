#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs.h>

#define TO_READ 10370
#define TO_WRITE 10

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

  if (argc < 1) {
    printf("Usage: %s <diskimage>\n", argv[0]);
    exit(1);
  }
  char buf[TO_READ];
  //char tuff[TO_WRITE] = "HelloWorld";
  /* Mount disk */
  diskname = argv[1];
  ret = fs_mount(diskname);
  ASSERT(!ret, "fs_mount");
  fs_info();
  fs_ls();
  int fd_open = fs_open("simple_writer.c");
  printf("fd_write: %d\n", fd_open);
  printf("fs_stat: %d\n", fs_stat(fd_open));
  
  int read = fs_read(fd_open, buf, TO_READ);
  //fs_write(fd_open, tuff, TO_WRITE);
  for (int i = 0; i < read; i++) {
    printf("%c", buf[i]);
  }
  printf("\nend\n");

  fs_umount();

  return 0;
}
