# ECS 150 Operating Systems: Project 3
## Overview
This project is designed to create a FAT-based filesystem by implementing
various functions such as mounting and unmounting a partition, reading and
writing files, and creating or removing files. In this project, we also simulate
a formatted partition with a binary file, and test various edge cases with it.


## Testing 
We have conducted several tests in order to guarentee the expected output. 
 * Create a test file for each function 
 * Using a small sample of text as a baseline for writing, reading, and reading
 with an offset 
 * Creating and deleting a file, as well as checking for persistence through
 multiple executions 
 * Comparing output results with the reference file fs_ref 
 * Manually calculating and creating diagrams to guarentee that the FAT and data
 table are correctly allocated
    
## Implementation

### Phase 1:
The first phase requires us to implement the mounting and unmounting portion of
the project. Mounting and unmounting refers to the loading of a virtual disk,
making sure that the disk is ready to be used. `fs_mount` loads all of the
relevant meta-information needed for the entire software stack, such as the
superblock, FAT blocks, root directory block, and data blocks. We then check the
signature of the superblock to ensure that it matches with the provided name
`ECS150FS`. Data from the superblock is then loaded into a struct, where it can
be retrieved via `fs_info`. `fs_unmount` checks for any open files, saves and
closes them, thus ensuring data is not lost when removing the disk.
 
### Phase 2:
Phase 2 implements two functions, `fs_create` and `fs_delete`. Using the
previous allocated FAT blocks, root directory, and data blocks, `fs_create`
searches for an empty root directory and allocates information for a new block.
This refers to setting the size of the file to 0, setting the FAT to `FAT_EOC`,
and naming the file the name provided. `fs_delete` provides a similar function
by setting everything to 0 and naming the file the null character `'\0'`.

Testing this feature included creating a file, then running the program again to
see if two files exist. If so, the write to disk is successful and the disk
retains memory. Delete operates the same way, deleting the files we have
created. In addition, error checking has been tested by trying to delete a file
that does not exist.

### Phase 3:
The third phase refers to various file descriptor functions such as `fs_open`,
`fs_close`, `fs_lseek`, and `fs_stat`. `fs_open` is implemented by first
searching the root directory for the file to open, then returning the file
descriptor along with setting the offset to 0. `fs_close` closes the file
descriptor by setting it to -1. `fs_lseek` receives the file descriptor, then
sets the offset of the file to the value provided. `fs_stat` recieves a file
descriptor, then checks an array for that value to return the size of the file.

### Phase 4:
Phase 4 creates the final two functions, `fs_read` and `fs_write`. Both of these
functions require the use of three different parameters, the file descriptor, a
buffer to read/write from, and the number of bytes to perform the corresponding
operation on. 

`fs_read` first takes in the file descriptor. The first data block to read from
is based off of the file descriptor, but also incremented by the previous blocks
such as the superblock. The data is read to  a bounce buffer, which is needed in
case the amount to read is bigger than the size of the block. If this is not the
case, the block is simply read to the bounce, copied to buf, and returned.
Otherwise, the bounce buffer reads up to the max size of a block from the
offset, moving to the next block, and reading the remainder. This continues
until either `fs_read` reads all the bytes needed or the file ends. The offset
is then moved to the location that `fs_read` finishes on. `fs_read` has certain
caveats that it must overcome. For example, `fs_read` may read a file which is
smaller than the block size. This can be resolved by reading only the size of
the file. Another issue that may occur is reading a file that exceeds 1 block,
or additionally, exceeds multiple blocks and ends in the middle of another. This
issue is resolved by continously reading a block's worth of data, trimming off
any excess to read from the next block, until there are no more excess bytes.

`fs_write` operates similarly to `fs_read` in which it needs to fix the same
problems encountered in `fs_read`. `fs_write` has some additional components to
tackle on, such as finding a new, empty block to write from after filling up the
previous block, then assigning it a value in the FAT. In addition, if the most
recent block written was the last block, the FAT value would be set to `FAT_EOC`
This is implemented in our code similar to `fs_read`, where we write the amount
from offset to end of the block, save the excess, look for another block to
write to, then continue writing. This repeats until we have written all the
bytes needed to, or we run out of space to write. `fs_write` returns the number
of bytes written.