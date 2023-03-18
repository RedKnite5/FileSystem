#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fs.h>

#define TO_READ 1000

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
  /* Mount disk */
  diskname = argv[1];
  ret = fs_mount(diskname);
  ASSERT(!ret, "fs_mount");
  fs_info();


  /* Mount disk */
  diskname = argv[1];
  ret = fs_mount(diskname);
  ASSERT(!ret, "fs_mount");
  fs_info();
  fs_create("Roger");
  fs_create("Is");
  fs_create("Cool");
  fs_ls();
  fs_delete("Roger");
  // fs_ls();
  int fd_write = fs_open("simple_writer.c");
  printf("fd_write: %d\n", fd_write);
  printf("fs_stat: %d\n", fs_stat(fd_write));
  fs_read(fd_write, buf, TO_READ);
  for (int i = 0; i < TO_READ; i++) {
    printf("%c", buf[i]);
  }
  printf("\nend\n");

  fs_umount();

  return 0;
}
