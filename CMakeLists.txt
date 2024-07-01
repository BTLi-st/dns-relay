# 允许测试
enable_testing()

# CMAKE 最小版本要求
cmake_minimum_required(VERSION 3.5)

# 项目信息
project(dns_relay_server)

# c++ 版本 20
set(CMAKE_CXX_STANDARD 20)

# 源文件
aux_source_directory(${PROJECT_BINARY_DIR}/../src SRC_LIST)

# 移除 main.cpp
list(REMOVE_ITEM SRC_LIST ${PROJECT_BINARY_DIR}/../src/main.cpp)

# 头文件
include_directories(${PROJECT_BINARY_DIR}/../inc)

# 设定库保存路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib)

# 设定可执行文件保存路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)

# 编译除 main.cpp 之外的所有文件为静态库
add_library(dns_server_lib STATIC ${SRC_LIST})

# 编译 main.cpp 为可执行文件
add_executable(dns_server ${PROJECT_BINARY_DIR}/../src/main.cpp)

# 为不同的构建类型设置不同的编译选项
if(MSVC)

target_compile_options(dns_server_lib PRIVATE 
    $<$<CONFIG:Debug>:/DDEBUG /Od /Zi /Zc:__cplusplus /utf-8>
    $<$<CONFIG:Release>:/DNDEBUG /O2 /Zc:__cplusplus /utf-8>
)

target_compile_options(dns_server PRIVATE 
    $<$<CONFIG:Debug>:/DDEBUG /Od /Zi /Zc:__cplusplus /utf-8>
    $<$<CONFIG:Release>:/DNDEBUG /O2 /Zc:__cplusplus /utf-8>
)
    
else(MSVC)

target_compile_options(dns_server_lib PRIVATE 
    $<$<CONFIG:Debug>:-DDEBUG -O0 -g>
    $<$<CONFIG:Release>:-DNDEBUG -O3>
)

target_compile_options(dns_server PRIVATE 
    $<$<CONFIG:Debug>:-DDEBUG -O0 -g>
    $<$<CONFIG:Release>:-DNDEBUG -O3>
)

endif(MSVC)

# 链接静态库
target_link_libraries(dns_server dns_server_lib)