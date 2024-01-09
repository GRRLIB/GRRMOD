cmake_minimum_required(VERSION 3.18)
project(grrmod VERSION 0.0.1)

option(GRRMOD_USE_MOD "Enable MOD support" ON)
option(GRRMOD_USE_MP3 "Enable MP3 support" ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)

if(GRRMOD_USE_MOD)
  file(GLOB MOD_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD_MOD.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/drivers/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/loaders/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/mmio/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/playercode/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/depackers/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/posix/*.c"
  )
endif()
if(GRRMOD_USE_MP3)
  file(GLOB MP3_SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD_MP3.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/mpg123/*.c"
  )
endif()

add_library(grrmod STATIC)

target_compile_options(grrmod
  PRIVATE
  -Wall
  -Wunused
)

target_sources(grrmod
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/GRRMOD_core.c"
  "${MOD_SRC_FILES}"
  "${MP3_SRC_FILES}"
)

target_compile_options(grrmod PRIVATE
  -DHAVE_FCNTL_H -DHAVE_INTTYPES_H -DHAVE_LIMITS_H -DHAVE_MALLOC_H
  -DHAVE_MEMCMP -DHAVE_SETENV -DHAVE_SNPRINTF -DHAVE_STDINT_H
  -DHAVE_STDLIB_H -DHAVE_STRCASECMP -DHAVE_STRDUP -DHAVE_STRINGS_H
  -DHAVE_STRING_H -DHAVE_STRSTR -DHAVE_SYS_STAT_H -DHAVE_SYS_TYPES_H
  -DHAVE_SYS_WAIT_H -DHAVE_UNISTD_H -DHAVE_ATOLL -DHAVE_LANGINFO_H
  -DHAVE_LIBM -DHAVE_LOCALE_H -DHAVE_NL_LANGINFO -DHAVE_SIGNAL_H
  -DHAVE_STDIO_H -DHAVE_SYS_SIGNAL_H -DHAVE_SYS_PARAM_H -DHAVE_STRERROR
  -DHAVE_SYS_RESOURCE_H
)
if(GRRMOD_USE_MP3)
  target_compile_options(grrmod PRIVATE
    -DOPT_GENERIC
    -DREAL_IS_FLOAT
  )
endif()

target_include_directories(grrmod
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/mikmod/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/mpg123"
)

target_link_libraries(grrmod PUBLIC
  aesnd
)

configure_file(grrmod.pc.in ${CMAKE_BINARY_DIR}/grrmod.pc @ONLY)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/grrmod.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${CMAKE_BINARY_DIR}/grrmod.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

install(
  TARGETS grrmod EXPORT grrmod-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
