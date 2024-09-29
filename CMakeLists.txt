cmake_minimum_required(VERSION 3.5)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(insert-print-stmts)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

execute_process (
    COMMAND bash -c "clang++ -E -x c++ - -v < /dev/null 2>&1 | grep \"lib/clang\" | awk '{{print $1}}' | grep include"
    OUTPUT_VARIABLE clangBuiltInIncludePath
)

file(WRITE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/builtInInclude.path" "${clangBuiltInIncludePath}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(LLVM REQUIRED)
find_package(Clang REQUIRED)

set(CMAKE_CXX_FLAGS "-Wall -g3 -O0 -fno-rtti ${LLVM_COMPILE_FLAGS}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

add_executable(insert-print-stmts src/main.cpp src/utils/utils.cpp src/tool/code_inserter_tool.cpp)

target_link_libraries(insert-print-stmts PRIVATE clang-cpp ${LLVM_LIBS_CORE})
