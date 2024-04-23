# 要求的最低cmake版本
cmake_minimum_required(VERSION 3.0)

#项目名称
project(mprpc)

# 生成debug版本，可gdb调试
set(CMAKE_BUILD_TYPE "Debug")
# 项目可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 项目编译头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

# 项目库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 添加编译的文件搜索子目录，src包含了mprpc框架的所有代码
add_subdirectory(src)
# 添加编译的文件搜索子目录，example包含了mprpc框架使用的示例代码
add_subdirectory(example)


