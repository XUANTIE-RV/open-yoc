# littlefs
## Summary
A little fail-safe filesystem designed for microcontrollers.

   | | |     .---._____
  .-----.   |          |
--|o    |---| littlefs |
--|     |---|          |
  '-----'   '----------'
   | | |
Power-loss resilience - littlefs is designed to handle random power failures. All file operations have strong copy-on-write guarantees and if power is lost the filesystem will fall back to the last known good state.

Dynamic wear leveling - littlefs is designed with flash in mind, and provides wear leveling over dynamic blocks. Additionally, littlefs can detect bad blocks and work around them.

Bounded RAM/ROM - littlefs is designed to work with a small amount of memory. RAM usage is strictly bounded, which means RAM consumption does not change as the filesystem grows. The filesystem contains no unbounded recursion and dynamic memory is limited to configurable buffers that can be provided statically.

## Example
Here's a simple example that updates a file named boot_count every time main runs. The program can be interrupted at any time without losing track of how many times it has been booted and without corrupting the filesystem:
```c
#include "lfs.h"

// variables used by the filesystem
lfs_t lfs;
lfs_file_t file;

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};

// entry point
int main(void) {
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %d\n", boot_count);
}
```
## Usage
Detailed documentation (or at least as much detail as is currently available) can be found in the comments in lfs.h.

littlefs takes in a configuration structure that defines how the filesystem operates. The configuration struct provides the filesystem with the block device operations and dimensions, tweakable parameters that tradeoff memory usage for performance, and optional static buffers if the user wants to avoid dynamic memory.

The state of the littlefs is stored in the lfs_t type which is left up to the user to allocate, allowing multiple filesystems to be in use simultaneously. With the lfs_t and configuration struct, a user can format a block device or mount the filesystem.

Once mounted, the littlefs provides a full set of POSIX-like file and directory functions, with the deviation that the allocation of filesystem structures must be provided by the user.

All POSIX operations, such as remove and rename, are atomic, even in event of power-loss. Additionally, file updates are not actually committed to the filesystem until sync or close is called on the file.

## Design
At a high level, littlefs is a block based filesystem that uses small logs to store metadata and larger copy-on-write (COW) structures to store file data.

In littlefs, these ingredients form a sort of two-layered cake, with the small logs (called metadata pairs) providing fast updates to metadata anywhere on storage, while the COW structures store file data compactly and without any wear amplification cost.

Both of these data structures are built out of blocks, which are fed by a common block allocator. By limiting the number of erases allowed on a block per allocation, the allocator provides dynamic wear leveling over the entire filesystem.

                    root
                   .--------.--------.
                   | A'| B'|         |
                   |   |   |->       |
                   |   |   |         |
                   '--------'--------'
                .----'   '--------------.
       A       v                 B       v
      .--------.--------.       .--------.--------.
      | C'| D'|         |       | E'|new|         |
      |   |   |->       |       |   | E'|->       |
      |   |   |         |       |   |   |         |
      '--------'--------'       '--------'--------'
      .-'   '--.                  |   '------------------.
     v          v              .-'                        v
.--------.  .--------.        v                       .--------.
|   C    |  |   D    |   .--------.       write       | new E  |
|        |  |        |   |   E    |        ==>        |        |
|        |  |        |   |        |                   |        |
'--------'  '--------'   |        |                   '--------'
                         '--------'                   .-'    |
                         .-'    '-.    .-------------|------'
                        v          v  v              v
                   .--------.  .--------.       .--------.
                   |   F    |  |   G    |       | new F  |
                   |        |  |        |       |        |
                   |        |  |        |       |        |
                   '--------'  '--------'       '--------'