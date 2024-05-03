cmake_minimum_required(VERSION 3.18)
project(demo)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GRRMOD REQUIRED grrmod)
pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(JPEG REQUIRED libjpeg)
pkg_check_modules(FREETYPE REQUIRED freetype2)

find_library(AESND aesnd
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)
find_library(FAT fat
  PATHS "${OGC_ROOT}/lib/${OGC_SUBDIR}"
  REQUIRED
)
find_library(GRRLIB grrlib REQUIRED)
find_library(PNGU pngu REQUIRED)

file(GLOB_RECURSE BIN_FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/data/*.*"
)

add_executable(demo)

target_compile_options(demo
  PRIVATE
  -Werror
  -Wall
  -Wunused
)

target_sources(demo
  PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/source/main.c"
)

target_include_directories(demo
  PRIVATE
  ${DEVKITPRO}/portlibs/${OGC_CONSOLE}/${CMAKE_INSTALL_INCLUDEDIR}
)

dkp_add_embedded_binary_library(data
  ${BIN_FILES}
)

target_link_libraries(demo
  data
  ${GRRMOD_LIBRARIES}
  ${AESND}
  ${GRRLIB}
  ${PNGU}
  ${PNG_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${FAT}
)
