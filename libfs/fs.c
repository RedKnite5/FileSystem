#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "disk.h"
#include "fs.h"

#define SUPER_PADDING 4079
#define ROOTDIR_PADDING 10
#define FAT_EOC 0xFFFF
#define ENTRIES_PER_FATBLOCK 2048

#if 0
  #define error(msg) fprintf(stderr, msg)
#else
  #define error(msg) do {} while (0)
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))

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
  uint8_t padding[ROOTDIR_PADDING];
  //Formatting of entries designated for functions
};

struct __attribute__ ((__packed__)) openFile
{
  int32_t filenum;
  uint32_t offset;
};


//Global Variables
uint16_t *fat;  // array of unknown length
static struct superBlock superBlock;
static struct rootDirEntry rootDir[FS_FILE_MAX_COUNT];
static struct openFile openFileTable[FS_OPEN_MAX_COUNT];

void _setOpenFileTableDefaults(void) {
  for (int __OPENFILE=0; __OPENFILE<FS_OPEN_MAX_COUNT; __OPENFILE++) {
    openFileTable[__OPENFILE].filenum = -1;
    openFileTable[__OPENFILE].offset = 0;
  }
}

int fs_mount(const char *diskname) {
  uint8_t bytes[8];
  //char input[8];
  char sig[] = "ECS150FS";
	/* TODO: Phase 1 */
  //Open disk, store info in data structures?
  if(block_disk_open(diskname)) {
    return -1;
  }
  _setOpenFileTableDefaults();
  //superBlock
  block_read(0, &superBlock);
  int N = superBlock.fatBlockCount;
  fat = malloc(sizeof(uint16_t) * ENTRIES_PER_FATBLOCK * N);
  for (int i = 0; i < 8; i++) {
    bytes[i] = superBlock.signature >> (8 * i) & 0xFF;
  }
  for (int i = 0; i < 8; i++) {
    if(sig[i] != (char)bytes[i]) {
      return -1;
    }
  }
  
  //FAT
  for (int i = 1; i < N; i++) {
    block_read(i, &fat[i * BLOCK_SIZE / sizeof(uint16_t)]);
  }
  
  //RootDirectory
  block_read(superBlock.rootDirIndex, rootDir);
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
  int occupied = 0;
  for (int i = 0; i < 8; i++) {
    bytes[i] = superBlock.signature >> (8 * i) & 0xFF;
  }

    //print the individual bytes
  printf("FS Info:\n");
  printf("total_blk_count=%i\n", superBlock.blockCount);
  printf("fat_blk_count=%i\n", superBlock.fatBlockCount);
  printf("rdir_blk=%i\n", superBlock.rootDirIndex);
  printf("data_blk=%i\n", superBlock.dataBlockStartIndex);
  printf("data_blk_count=%i\n", superBlock.dataBlockCount);
  for (int i=0; i<FS_OPEN_MAX_COUNT; i++) {
    if (openFileTable[i].filenum != 0) {
        occupied++;
    }
  }
  occupied = superBlock.dataBlockCount - occupied;
  printf("fat_free_ratio=%i/%i\n", occupied, superBlock.dataBlockCount);
  occupied = 0;
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (rootDir[i].filename[0] != '\0') {
      occupied++;
    }
  }
  occupied = FS_FILE_MAX_COUNT - occupied;
  printf("rdr_free_ratio=%i/%i", occupied, FS_FILE_MAX_COUNT);
  printf( "\n" );
  
  return 0;
}

int fs_create(const char *filename) {
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
      printf("file: %s, size: %i, data_blk: %i\n", rootDir[i].filename, rootDir[i].size, rootDir[i].start_index);
    }
  }
  return 0;
}

int fs_open(const char *filename) {
	/* TODO: Phase 3 */
  if (strlen(filename) >= FS_FILENAME_LEN || block_disk_count() == -1) {
    return -1;
    error("invalid filename or disk not mounted\n");
  }

  int index = -1;
  for (int i = 0; i < FS_FILE_MAX_COUNT; i++) {
    if (!strcmp(rootDir[i].filename, filename)) {
      index = i;
    }
  }
  if (index == -1) {
    // file not found
    error("file not found\n");
    return -1;
  }
  
  for (int i=0; i<FS_OPEN_MAX_COUNT; i++) {
    if (openFileTable[i].filenum != -1) {
      continue;
    }
    openFileTable[i].filenum = index;
    openFileTable[i].offset = 0;
    return i;
  }
  error("no open space\n");
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
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
  }
  printf("Size of file '%s' is %i bytes\n", rootDir[openFileTable[fd].filenum].filename, rootDir[openFileTable[fd].filenum].size);
  return rootDir[openFileTable[fd].filenum].size;
}

int fs_lseek(int fd, size_t offset) {
	/* TODO: Phase 3 */
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
  }
  if (offset > rootDir[openFileTable[fd].filenum].size) {
    error("offset larger than file\n");
    return -1;
  }
  openFileTable[fd].offset = offset;
  return 0;
}

int fs_write(int fd, void *buf, size_t count) {
/*Similar logic to FS_READ
    Steps to figure out:
      Block still has empty space to write to, stop when reach end
      Find next empty block to write to if overflows

  Steps to execute:
    1. Copy buf into bounce buffer, buffer may be > block size
    2. If bounce is > remaining space
        write what you can, count remaining bytes
        Look for next availiable block
          *Check FAT, 0 means it is empty
        Write to block, repeat 2 if new block is full
        Set FAT to point at newly written block
        If done writing, set FAT to FAT_EOC
    3. Else
      Write into disk
*/
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
    }
  char bounce[count];
  int total = 0;
  uint16_t block = rootDir[openFileTable[fd].filenum].start_index + superBlock.fatBlockCount + 2;
  //Step 1
  memcpy(bounce, buf, count);

  //Step 2, Step 3
  int left_in_block = BLOCK_SIZE - (openFileTable[fd].offset % BLOCK_SIZE);
  int to_write = MIN(left_in_block, count);
  char remain[BLOCK_SIZE];
  memcpy(remain, bounce, to_write);
  block_write(block, remain);
  total += to_write;
  count -= to_write;
  openFileTable[fd].offset += to_write;

  //Step 2.5: Look for next block
  while(count) {
    block++;
    if (block == 0) {
      if (count >= BLOCK_SIZE) {
        // read into buffer
        memcpy(remain, bounce, count);
        total += BLOCK_SIZE;
        count -= BLOCK_SIZE;
        openFileTable[fd].offset += BLOCK_SIZE;
        openFileTable[fd].filenum = block;
        fd = block;
        block_write(block, remain);
      } else {
        memcpy(remain, bounce, count);
        total += count;
        count -= count;
        openFileTable[fd].offset += count;
        openFileTable[fd].filenum = 0xFFFF;
        fd = block;
        block_write(block, remain);
      }
      total += count;
      openFileTable[fd].offset += count;
    }
  }
  return total;
  
}

int fs_read(int fd, void *buf, size_t count) {
	/* TODO: Phase 4 */
  if (block_disk_count() == -1 || fd >= FS_OPEN_MAX_COUNT || openFileTable[fd].filenum == -1) {
    return -1;
    }
  //Assumption: openFileTable is file table, fd is file descriptor
  char bounce[BLOCK_SIZE];
  int total = 0;
  uint16_t block = rootDir[openFileTable[fd].filenum].start_index + superBlock.fatBlockCount + 2;
  block_read(block, bounce);
  /*Special situations:
    Too long
    Too short
    Starts halfway, overflows into another
  */
  // first block
  int left_in_block = BLOCK_SIZE - (openFileTable[fd].offset % BLOCK_SIZE);
  int to_copy = MIN(left_in_block, count);
  memcpy(buf, bounce+openFileTable[fd].offset, to_copy);
  total += to_copy;
  count -= to_copy;
  openFileTable[fd].offset += to_copy;
  
  // what if we run out of file to read?
  while (count) {
    block = fat[block];
    //end of file
    if (block == FAT_EOC) {
      return total;
    }
    if (count >= BLOCK_SIZE) {
      // read into buffer
      block_read(block, buf);
      total += BLOCK_SIZE;
      count -= BLOCK_SIZE;
      openFileTable[fd].offset += BLOCK_SIZE;
      continue;
    }
    block_read(block, bounce);
    //block_read(openFileTable[fd].filenum, bounce);
    memcpy(buf, bounce, count);
    total += count;
    openFileTable[fd].offset += count;
    count = 0;
  }
  return total;
}