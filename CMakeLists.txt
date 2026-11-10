cmake_minimum_required(VERSION 3.11)

project(
  CheatDeviceRemastered
  VERSION 1.0
  DESCRIPTION "A Cheat Device for Grand Theft Auto Liberty & Vice City Stories for PlayStation Portable."
  HOMEPAGE_URL "http://cheatdeviceremastered.com"
  LANGUAGES C ASM
)

# Include PSPSDK CMake stuff
include("$ENV{PSPDEV}/psp/share/pspdev.cmake")

set(PROJECT_AUTHOR "Freakler")

add_definitions(-DNDEBUG -DINI_READONLY)

add_subdirectory(lib/minIni-PSP)
add_subdirectory(lib/StoriesRWSDK)
add_subdirectory(src)

add_prx_module(${PROJECT_NAME} ${MAIN_SOURCES})

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
  C_STANDARD 23
  C_STANDARD_REQUIRED ON
)

target_include_directories(${PROJECT_NAME} PRIVATE
  include
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Os
  -fno-pic
  -G0
  -fno-math-errno
  -mpreferred-stack-boundary=4
  -fshort-wchar
  -mno-check-zero-division
  -mtune=mips32r2

  -Wall
  -Wextra
  -Werror

  -Wno-unused-function
  -Wno-unused-const-variable
  -Wno-unused-parameter
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  minIni
  StoriesRWSDK
  ansic
  pspsystemctrl_user
  pspctrl
  pspmalloc
)

target_link_options(${PROJECT_NAME} PRIVATE
  -specs=${PSPDEV}/psp/sdk/lib/prxspecs
  -Wl,-q,-T${PSPDEV}/psp/sdk/lib/linkfile.prx
  ${PSPDEV}/psp/sdk/lib/prxexports.o
  -Wl,-zmax-page-size=128

  -Wl,-static
  -Wl,--gc-sections
)