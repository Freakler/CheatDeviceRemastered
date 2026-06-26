cmake_minimum_required(VERSION 3.28) # [TODO] Is there a proper version to set it to?

project(CheatDeviceRemastered
  VERSION 1.0
  DESCRIPTION "A Cheat Device for Grand Theft Auto Liberty & Vice City Stories for PlayStation Portable."
  HOMEPAGE_URL "http://cheatdeviceremastered.com"
  LANGUAGES C
)

include_directories(include)
add_subdirectory(src)