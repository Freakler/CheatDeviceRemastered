cmake_minimum_required(VERSION 3.11)

project(
  CheatDeviceRemastered
  VERSION 1.0
  DESCRIPTION "A Cheat Device for Grand Theft Auto Liberty & Vice City Stories for PlayStation Portable."
  HOMEPAGE_URL "http://cheatdeviceremastered.com"
  LANGUAGES C
)

set(PROJECT_AUTHOR "Freakler")

include_directories(include)
add_subdirectory(src)

include("$ENV{PSPDEV}/psp/share/pspdev.cmake")

set_property(SOURCE src/modinfo.S PROPERTY LANGUAGE C)

add_prx_module(${PROJECT_NAME} ${MAIN_SOURCES} src/modinfo.S)

# Get version info from cmake to project
configure_file(
  src/versioning.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/versioning.h
  @ONLY # Only replace vars with this "@syntax@"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/archive"
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Os
  -fno-pic
  -G0
  -ffast-math
  -mpreferred-stack-boundary=4
  -fshort-wchar
  -mno-check-zero-division

  -Wall
  -Wno-restrict
  -Wno-format-truncation
  -Wno-implicit-function-declaration
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  ansic
  pspsystemctrl_user
  pspctrl
  pspmalloc
  pspuser
  psprtc
  m
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_options(${PROJECT_NAME} PRIVATE
  -nolibc
  -specs=${PSPDEV}/psp/sdk/lib/prxspecs
  -Wl,-zmax-page-size=128
  -flto
)