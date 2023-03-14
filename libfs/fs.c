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
  uint32_t size;
  uint16_t start_index;
  uint8_t padding[FAT_PADDING];
  //Formatting of entries designated for functions
};

struct __attribute__ ((__packed__)) openFile {
  int32_t filenum;
  uint32_t offset;
};


//Global Variables
uint16_t *fat;  // array of unknown length
static struct superBlock superBlock;
static struct rootDirEntry rootDir[FS_FILE_MAX_COUNT];
static struct openFile openFileTable[FS_OPEN_MAX_COUNT];
// set defaults
void _setOpenFileTableDefaults(void) {
  for (int __OPENFILE=0; __OPENFILE<FS_OPEN_MAX_COUNT; __OPENFILE++) {
    openFileTable[__OPENFILE].filenum = -1;
    openFileTable[__OPENFILE].offset = 0;
  }
}

int fs_mount(const char *diskname) {
  int rootIndex;
  uint8_t bytes[8];
  //char input[8];
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
  return 0;
}

int fs_umount(void) {
  //TODO: Check for current open file descriptors
	/* TODO: Phase 1 */
  if (block_disk_close() == -1) {
    return -1;
  }
  return 0;
}

int fs_info(void) {
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
  if (strlen(filename) >= FS_FILENAME_LEN || block_disk_count() == -1) {
    return -1;
  }
  
	//Initial size is 0
  //Index is -1, nothing in data block
  //Get name, size, index, add it to root
  //Check if root dir is empty
  for (int entry=1; entry<FS_FILE_MAX_COUNT; entry++) {
    if (rootDir[entry].filename[0] != '\0') {
      continue;
    }

    // check rest of directory for preexisting filename
    for (int j=entry; j<FS_FILE_MAX_COUNT; j++) {
      if (!strcmp(rootDir[j].filename, filename)) {
        return -1;
      }
    }
    
    strcpy(rootDir[entry].filename, filename);
    rootDir[entry].size = 0;
    rootDir[entry].start_index = 0xFFFF;  
    return 0;
  }
  return -1;
}

int fs_delete(const char *filename) {
	/* TODO: Phase 2 */
  // still need to check that file isn't open
  if (strlen(filename) >= FS_FILENAME_LEN || block_disk_count() == -1) {
    return -1;
  }
  
  for (int entry=1; entry<FS_FILE_MAX_COUNT; entry++) {
    if (strcmp(rootDir[entry].filename, filename)) {
      continue;
    }
    rootDir[entry].size = 0;
    rootDir[entry].start_index = 0;  
    rootDir[entry].filename[0] = '\0';
    return 0;
  }
  return -1;
}

int fs_ls(void) {
  if (block_disk_count() == -1) {
    return -1;
  }
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (rootDir[i].filename[0] != '\0') {
      printf("%s\n", rootDir[i].filename);
    }
  }
  return 0;
}

int fs_open(const char *filename) {
	/* TODO: Phase 3 */
  if (strlen(filename) >= FS_FILENAME_LEN || block_disk_count() == -1) {
    return -1;
  }

  int index = -1;
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (!strcmp(rootDir[i].filename, filename)) {
      index = i;
    }
  }
  if (index == -1) {
    // file not found
    return -1;
  }
  
  for (int i=0; i<FS_OPEN_MAX_COUNT; i++) {
    if (openFileTable[i].filenum == -1) {
      continue;
    }
    openFileTable[i].filenum = index;
    openFileTable[i].offset = 0;
    return 0;
  }
  return -1;
}

int fs_close(int fd) {
	/* TODO: Phase 3 */
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
  }

  // does anything else need to be done?
  openFileTable[fd].filenum = -1;
  return 0;
}

int fs_stat(int fd) {
	/* TODO: Phase 3 */
  
}

int fs_lseek(int fd, size_t offset) {
	/* TODO: Phase 3 */
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
  }

  // TODO: if fd is greater than filesize return -1

  openFileTable[fd].offset = offset;
  return 0;
}

int fs_write(int fd, void *buf, size_t count) {
	/* TODO: Phase 4 */
}

int fs_read(int fd, void *buf, size_t count) {
	/* TODO: Phase 4 */
}
