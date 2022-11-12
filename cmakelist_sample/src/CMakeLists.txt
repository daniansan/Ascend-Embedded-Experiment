cmake_minimum_required(VERSION 3.10)
project(CMAKELIST_EXAMPLE)

#设置交叉编译器
if (CMAKE_SYSTEM_PROCESSOR MATCHES "aarch")
    set(CMAKE_C_COMPILER gcc)
else (CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
    set(CMAKE_C_COMPILER "/opt/compiler/bin/aarch64-linux-gnu-gcc")
endif()

# 指定生成文件路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "../output")

add_executable(target main.c)
# add_library(hello SHARED main.c)

target_link_libraries(target)
# 安装
install(TARGETS target DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
