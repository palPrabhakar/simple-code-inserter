cmake_minimum_required(VERSION 3.5)

# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (CMAKE_CXX_STANDARD 20)

project(simple-code-inserter)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

set(CMAKE_PREFIX_PATH "/home/pal/opt/llvm/")

find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -g3 -O0 -fno-rtti ${LLVM_COMPILE_FLAGS}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(simple-code-inserter src/main.cpp src/tool/code_inserter_tool.cpp)

target_link_libraries(simple-code-inserter
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  clangRewrite
  clangToolingCore
  clangToolingRefactoring
  clangTransformer
  )
