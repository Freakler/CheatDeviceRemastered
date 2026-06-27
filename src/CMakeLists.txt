file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS
  *.c
  **/*.c
  *.S
  exports.exp
)

add_prx_module(${PROJECT_NAME} ${MAIN_SOURCES})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND mv ${PROJECT_NAME}.prx ../${PROJECT_NAME}.prx
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  -nodefaultlibs
  m
  gcc
  ansic
  pspmalloc
  pspctrl
  pspsystemctrl_user
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 23)
set_property(SOURCE imports.S PROPERTY LANGUAGE C)
set_property(SOURCE modinfo.S PROPERTY LANGUAGE C)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Os
  -fno-pic
  -G0
  -ffast-math
  -mpreferred-stack-boundary=4
  -fshort-wchar
  -mno-check-zero-division
  -frename-registers
  -mtune=mips32r2
  -ffunction-sections
  -fdata-sections

  -Wall
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
  # [TODO] This is a pretty shitty way of doing it...
  PLUGIN_NAME=\"${PROJECT_NAME}\"
  PLUGIN_DESCRIPTION=\"${PROJECT_DESCRIPTION}\"
  PLUGIN_HOMEPAGEURL=\"${PROJECT_HOMEPAGE_URL}\"
  PLUGIN_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
  PLUGIN_VERSION_MINOR=${PROJECT_VERSION_MINOR}
  PLUGIN_VERSION=\"${PROJECT_VERSION}\"
)

target_link_options(${PROJECT_NAME} PRIVATE
  -specs=${PSPDEV}/psp/sdk/lib/prxspecs
  -L -Wl,-zmax-page-size=128
  -flto
)