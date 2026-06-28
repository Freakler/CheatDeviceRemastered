file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS
  *.c
  **/*.c
  exports.exp
)

add_prx_module(${PROJECT_NAME} ${MAIN_SOURCES})

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND mv ${PROJECT_NAME}.prx ../${PROJECT_NAME}.prx
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  pspuser
  ansic
  pspmalloc
  pspctrl
  pspsystemctrl_user
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 23)

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

target_link_options(${PROJECT_NAME} PRIVATE
  -specs=${PSPDEV}/psp/sdk/lib/prxspecs
  -L -Wl,-zmax-page-size=128
  -flto
)