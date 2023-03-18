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
  size_t offset = 100;
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
  
  int fd_open = fs_open("simple_writer.c");
  fs_lseek(fd_open, offset);
  int read = fs_read(fd_open, buf, TO_READ);
  for (int i = 0; i < read; i++) {
    printf("%c", buf[i]);
  }
  printf("\nend\n");

  fs_umount();

  return 0;
}
