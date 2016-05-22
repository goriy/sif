cmake_minimum_required(VERSION 2.8)

set(project "sif2")

project (${project})

file (GLOB src "./src/*.c")

set(CMAKE_RC_COMPILER_INIT windres)
ENABLE_LANGUAGE(RC)
SET(CMAKE_RC_COMPILE_OBJECT
  "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

include_directories(include)
set(CMAKE_CXX_FLAGS "-O2 -Wall -funsigned-char")
set(CMAKE_C_FLAGS "-O2 -Wall -funsigned-char")
set(CMAKE_EXE_LINKER_FLAGS "-s -static -mwindows -Xlinker --no-insert-timestamp")

add_executable ( ${project} ${src} res/res.rc)
target_link_libraries(${project} comctl32 ole32 uuid shlwapi)
