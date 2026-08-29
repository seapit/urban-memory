#  armv7a-toolchain.cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Target triple for ARMv7-A with hard-float ABI
set(CMAKE_C_FLAGS "--target=armv7-linux-gnueabihf")
set(CMAKE_CXX_FLAGS "--target=armv7-linux-gnueabihf")

# Optional: sysroot if you have one
# set(CMAKE_SYSROOT /path/to/arm/sysroot)
