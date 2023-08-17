set(HOST_PROCESSOR "riscv64")
execute_process (
  COMMAND bash -c "uname -m"
  OUTPUT_VARIABLE HOST_PROCESSOR
)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR riscv64)
set(CMAKE_HOST_SYSTEM_PROCESSOR ${HOST_PROCESSOR})
set(CMAKE_C_COMPILER /mnt/disk_nfs/yocto/riscv-toolchain/bin/riscv64-linux-gcc)
set(CMAKE_CXX_COMPILER /mnt/disk_nfs/yocto/riscv-toolchain/bin/riscv64-linux-g++)
set(CMAKE_SYSROOT /mnt/disk_nfs/yocto/riscv-toolchain/sysroot)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
