#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define SUPER_PADDING 4079
#define FAT_PADDING 10
#define FAT_EOC 0xFFFF
//Change FAT_EOC to -1 if doesnt work
/* TODO: Phase 1 */
struct __attribute__ ((__packed__)) superBlock
{
  int64_t signature;
  uint16_t blockCount;
  uint16_t rootDirIndex;
  uint16_t dataBlockStartIndex;
  uint16_t dataBlockCount;
  uint8_t  fatBlockCount;
  uint8_t  padding[SUPER_PADDING];
};

struct __attribute__ ((__packed__)) rootDirEntry 
{
  //The root directory is an array of 128 entries
  char filename[FS_FILENAME_LEN];
  uint64_t size;
  uint32_t start_index;
  uint8_t padding[FAT_PADDING];
  //Formatting of entries designated for functions
};
uint16_t *fat;


//Global Variables
static struct superBlock superBlock;
static struct rootDirEntry rootDir[FS_FILE_MAX_COUNT];

int fs_mount(const char *diskname)
{
  int rootIndex;
  uint8_t bytes[8];
  char input[8];
  char sig[8] = {'E', 'C', 'S', '1', '5', '0', 'F', 'S'};
	/* TODO: Phase 1 */
  //Open disk, store info in data structures?
  if(block_disk_open(diskname)) {
    return -1;
  }
  //superBlock
  block_read(0, &superBlock);
  int N = superBlock.fatBlockCount;
  fat = malloc(sizeof(uint16_t) * N);
  for ( int i = 0; i < 8; i++ )
    {
        bytes[i] = superBlock.signature >> (8 * i) & 0xFF;
    }
    //print the individual bytes
  for ( int i = 0; i < 8; i++ )
    {
        if(sig[i] != (char)bytes[i]) {
          return -1;
        }
    }
  
  //FAT
  for (int i = 1; i < N; i++) {
    block_read(i, &fat[i * BLOCK_SIZE]);
    rootIndex = i;
  }
  
  //RootDirectory
  block_read(rootIndex+1, &rootDir);
  
}

int fs_umount(void)
{
  //TODO: Check for current open file descriptors
	/* TODO: Phase 1 */
  if (block_disk_close() == -1) {
    return -1;
  }
  return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */
  uint8_t bytes[8];
    for ( int i = 0; i < 8; i++ )
    {
        bytes[i] = superBlock.signature >> (8 * i) & 0xFF;
    }

    //print the individual bytes
  printf("Signature is: ");
    for ( int i = 0; i < 8; i++ )
    {
        printf( "%c", bytes[i] );
    }
  printf( "\n" );
  printf("blockCount is: %i\n", superBlock.blockCount);
  printf("rootDirIndex is: %i\n", superBlock.rootDirIndex);
  printf("dataBlockStartIndex is: %i\n", superBlock.dataBlockStartIndex);
  printf("dataBlockCount is: %i\n", superBlock.dataBlockCount);
  printf("fatBlockCount is: %i\n", superBlock.fatBlockCount);
  return 0;
}


int fs_create(const char *filename)
{
  int creating = 1;
  int entry = 1;
	//Initial size is 0
  //Index is -1, nothing in data block
  //Get name, size, index, add it to root
  //Check if root dir is empty
  while (creating) {
     if (rootDir[entry].filename[0] == '\0') {
      for (int i = 0; i < strlen(filename); i++) {
        rootDir[entry].filename[i] = filename[i];
      }
  //Error test this, filename is 16 bytes but we need to null terminate string
      rootDir[entry].filename[strlen(filename) + 1] = '\0';
      rootDir[entry].size = 0;
      rootDir[entry].start_index = 0xFFFF;  
      creating = 0;
      return 0;
    }
    entry++; 
  }
  return -1;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */
  //NUll terminated string
  int deleting = 1;
  int entry = 1;
  while (deleting) {
    if (!strcmp(rootDir[entry].filename, filename)) {
      rootDir[entry].size = 0;
      rootDir[entry].start_index = 0;  
      rootDir[entry].filename[0] = '\0';
      deleting = 0;
      return 0;
    }
    entry++;
  }
  return -1;
}

int fs_ls(void)
{
  int success = -1;
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (rootDir[i].filename[0] != '\0') {
      for (int j = 0; j < strlen(rootDir[i].filename); j++) {
        printf("%c", rootDir[i].filename[j]);
        success = 1;
      }
      printf("\n");
    }
  }
  return success;
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
