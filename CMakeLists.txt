set(LOOM_CMAKE_AUX_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${LOOM_CMAKE_AUX_DIR}/x86_64-tc.cmake")
endif()

cmake_minimum_required(VERSION 3.26)
project(Loom_Kernel C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIMINE_INSTALL_DIR "${CMAKE_BINARY_DIR}/limine")
set(LIMINE_BINARY_DIR "${LIMINE_INSTALL_DIR}/bin")
set(LIMINE_SHARE_DIR "${LIMINE_INSTALL_DIR}/share")

set(LIMINE_BINARY "${LIMINE_BINARY_DIR}/limine")
set(LIMINE_BIOS_SYS "${LIMINE_SHARE_DIR}/limine/limine-bios.sys")

file(MAKE_DIRECTORY "${LIMINE_INSTALL_DIR}")

include(ExternalProject)

ExternalProject_Add(
    Limine
    BUILD_COMMAND "make"
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ./bootstrap
    COMMAND LD_FOR_TARGET=ld ./configure --enable-bios --enable-uefi-x86-64 --prefix=${LIMINE_INSTALL_DIR}
    GIT_PROGRESS 1
    GIT_REPOSITORY https://codeberg.org/Limine/Limine.git
    GIT_SHALLOW 1
    GIT_TAG 8331af0d65f5a776ece0aa46b1d8122ea4afb07f
    INSTALL_BYPRODUCTS ${LIMINE_BINARY} ${LIMINE_BIOS_SYS}
    INSTALL_COMMAND make install
    UPDATE_COMMAND ""
)

add_executable(limine IMPORTED GLOBAL)
add_dependencies(limine Limine)
set_target_properties(limine PROPERTIES IMPORTED_LOCATION "${LIMINE_BINARY}")

set(SRCDIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(SRCS ${SRCDIR}/limine.c ${SRCDIR}/main.c)
set(LINKER_SCRIPT ${SRCDIR}/loom.ld)

add_executable(loom_elf ${SRCS})
set_target_properties(loom_elf PROPERTIES OUTPUT_NAME "loom.elf")
target_include_directories(loom_elf PRIVATE "include")
target_compile_options(loom_elf PRIVATE "-g")
target_link_options(loom_elf PRIVATE "-T${LINKER_SCRIPT}")
set_target_properties(loom_elf PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

set(MKIMG "${LOOM_CMAKE_AUX_DIR}/mkimg.sh")
set(RUN "${LOOM_CMAKE_AUX_DIR}/run.sh")
set(DBG "${LOOM_CMAKE_AUX_DIR}/dbg.sh")
set(LOOM_IMG "${CMAKE_BINARY_DIR}/loom.img")

add_custom_command(
    OUTPUT ${LOOM_IMG}
    COMMAND ${MKIMG} ${CMAKE_BINARY_DIR} ${SRCDIR}/limine.conf $<TARGET_FILE:loom_elf> $<TARGET_FILE:limine> ${LIMINE_BIOS_SYS}
    DEPENDS ${MKIMG} $<TARGET_FILE:loom_elf> $<TARGET_FILE:limine>
)

add_custom_target(loom_img ALL DEPENDS ${LOOM_IMG})

add_custom_target(
    run
    COMMAND ${RUN} ${LOOM_IMG}
    DEPENDS ${RUN} ${LOOM_IMG}
)

add_custom_target(
    dbg
    COMMAND ${DBG} $<TARGET_FILE:loom_elf> ${LOOM_IMG}
    DEPENDS ${DBG} $<TARGET_FILE:loom_elf> ${LOOM_IMG}
)