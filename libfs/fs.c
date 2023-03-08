#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */
typedef struct superBlock((__packed__)) 
{
  uint64_t signature;
  uint16_t blockCount;
  uint16_t rootDirIndex;
  uint16_t dataBlockStartIndex;
  uint16_t dataBlockCount;
  uint8_t  fatBlockCount;
  uint8_t  padding[4079]
}

typedef struct fat((__packed__)) 
{
  //N = Number of data blocks
  uint16_t* fat[]= malloc(sizeof(*fat) * N);
}

typedef struct rootDir((__packed__)) 
{
  //The root directory is an array of 128 entries
  uint32_t fat[FS_FILE_MAX_COUNT];
  //Formatting of entries designated for functions
}

//Global Variables
static struct superBlock superBlock;

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
  //Open disk, store info in data structures?
  if(!block_disk_open(*diskname);) {
    return -1;
  }
  //superBlock
  block_read(0, &superBlock);

  //FAT
  for (int i = 1; i < (block_disk_count() * 2)/BLOCK_SIZE; i++) {
    block_read(i, fat[i * BLOCK_SIZE]);
  }

  //RootDirectory
  block_read((i+1) * BLOCK_SIZE, &rootDir);
  
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
  block_disk_close();
}

int fs_info(void)
{
	/* TODO: Phase 1 */
  printf("Signature is: %d\n", '0' + signature);
  printf("blockCount is: %i\n", blockCount);
  printf("rootDirIndex is: %i\n", rootDirIndex);
  printf("dataBlockStartIndex is: %i\n", dataBlockStartIndex);
  printf("dataBlockCount is: %i\n", dataBlockCount);
  printf("fatBlockCount is: %i\n", fatBlockCount);
}

int fs_create(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
}

int fs_ls(void)
{
	/* TODO: Phase 2 */
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */
}

int fs_stat(int fd)
{
	/* TODO: Phase 3 */
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */
}

int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */
}
