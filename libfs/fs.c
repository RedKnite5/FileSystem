#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define SUPER_PADDING 4079
#define FAT_PADDING 10
#define SIGNATURELENGTH 8

/* TODO: Phase 1 */
struct __attribute__((__packed__)) superBlock {
  uint8_t signature[SIGNATURELENGTH];
  uint16_t blockCount;
  uint16_t rootDirIndex;
  uint16_t dataBlockStartIndex;
  uint16_t dataBlockCount;
  uint8_t fatBlockCount;
  uint8_t padding[SUPER_PADDING];
};

struct __attribute__((__packed__)) rootDirEntry {
  // The root directory is an array of 128 entries
  char filename[16];
  uint64_t size;
  uint32_t start_index;
  uint8_t padding[FAT_PADDING];
  // Formatting of entries designated for functions
};
uint16_t *fat;

// Global Variables
static struct superBlock superBlock;
static struct rootDirEntry rootDir[FS_FILE_MAX_COUNT];

int fs_mount(const char *diskname) {
  int rootIndex;
  /* TODO: Phase 1 */
  // Open disk, store info in data structures?
  if (!block_disk_open(diskname)) {
    return -1;
  }
  // superBlock
  block_read(0, &superBlock);
  int N = superBlock.fatBlockCount;
  fat = malloc(sizeof(uint16_t) * N);

  // FAT
  for (int i = 1; i < N; i++) {
    block_read(i, &fat[i * BLOCK_SIZE]);
    rootIndex = i;
  }

  // RootDirectory
  block_read((rootIndex + 1) * BLOCK_SIZE, &rootDir);
}

int fs_umount(void) {
  // TODO: Check for current open file descriptors
  /* TODO: Phase 1 */
  if (block_disk_close() == -1) {
    return -1;
  }
  return 0;
}

int fs_info(void) {
  /* TODO: Phase 1 */
  printf("%.*s", SIGNATURELENGTH, superBlock.signature);
  // printf("Signature is: %lu\n", '0' + superBlock.signature);
  printf("blockCount is: %i\n", superBlock.blockCount);
  printf("rootDirIndex is: %i\n", superBlock.rootDirIndex);
  printf("dataBlockStartIndex is: %i\n", superBlock.dataBlockStartIndex);
  printf("dataBlockCount is: %i\n", superBlock.dataBlockCount);
  printf("fatBlockCount is: %i\n", superBlock.fatBlockCount);
}

int fs_create(const char *filename) { /* TODO: Phase 2 */ }

int fs_delete(const char *filename) { /* TODO: Phase 2 */ }

int fs_ls(void) { /* TODO: Phase 2 */ }

int fs_open(const char *filename) { /* TODO: Phase 3 */ }

int fs_close(int fd) { /* TODO: Phase 3 */ }

int fs_stat(int fd) { /* TODO: Phase 3 */ }

int fs_lseek(int fd, size_t offset) { /* TODO: Phase 3 */ }

int fs_write(int fd, void *buf, size_t count) { /* TODO: Phase 4 */ }

int fs_read(int fd, void *buf, size_t count) { /* TODO: Phase 4 */ }
