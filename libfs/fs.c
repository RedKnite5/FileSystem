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
  uint16_t dataBlockCount;
  uint8_t  fatBlockCount;
  //Unused/padding, 4079 bytes
}

typedef struct fat((__packed__)) 
{
  //N = Number of data blocks
  uint16_t* fat = malloc(sizeof(*fat) * N);
}

typedef struct Root directory((__packed__)) 
{
  //The root directory is an array of 128 entries
  uint32_t fat[FS_FILE_MAX_COUNT];
  //Formatting of entries designated for functions
}

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */
}

int fs_umount(void)
{
	/* TODO: Phase 1 */
}

int fs_info(void)
{
	/* TODO: Phase 1 */
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

