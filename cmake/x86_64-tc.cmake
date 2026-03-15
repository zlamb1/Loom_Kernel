set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-elf-gcc)
set(
    CMAKE_C_FLAGS_INIT 
    "-std=gnu11 -mno-red-zone -mcmodel=large -ffreestanding -fno-strict-aliasing -fno-omit-frame-pointer -mno-sse -mno-sse2 -mno-mmx"
)
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static -nostdlib")
set(CMAKE_OBJDUMP x86_64-elf-objdump)
set(CMAKE_OBJCOPY x86_64-elf-objcopy)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)