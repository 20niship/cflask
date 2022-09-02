cmake_minimum_required(VERSION 3.1)
project(dashcpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on") # compile_commands.json作成

option(BUILD_EXAMPLES "Build example programs" ON)

#コンパイラーがC++17に対応しているかチェック
include(CheckCXXCompilerFlag)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)

check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
check_cxx_compiler_flag("-std=c++2a" COMPILER_SUPPORTS_CXX2A)

if(${COMPILER_SUPPORTS_CXX20})
  set(CMAKE_CXX_FLAGS "-std=c++20 ${CMAKE_CXX_FLAGS}")
elseif(${COMPILER_SUPPORTS_CXX2A})
  set(CMAKE_CXX_FLAGS "-std=c++2a ${CMAKE_CXX_FLAGS}")
else()
  message(FATAL_ERROR "${CMAKE_CXX_COMPILER} はC++20をサポートしてません。C++20に対応したコンパイラを指定してください。\n")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Releaseモードでビルドします")
  set(CMAKE_CXX_FLAGS "-O3 -DNDEBUG ${CMAKE_CXX_FLAGS}")
else()
  message("Debugモードでビルドします")
  set(CMAKE_CXX_FLAGS "-O2 -g ${CMAKE_CXX_FLAGS}")
endif()

message("Compiler:\n\t${CMAKE_CXX_COMPILER}")
message("compiler flags:\n\t${CMAKE_CXX_FLAGS}")

# Set warning flags
set(
  CXX_WARNING_FLAGS
  -Wall
  -Wextra
  -Wpedantic
  # -Wdisabled-optimization
  -Wformat
  -Winit-self
  -Woverloaded-virtual
  -Wno-old-style-cast
  # -Wdeprecated-attributes
  # -Wno-ignored-attributes
  # -Wno-missing-field-initializers
)
foreach(FLAG IN LISTS CXX_WARNING_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

add_compile_options(-DFLASK_TEMPLATE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/template/")
add_compile_options(-DFLASK_PUBLIC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/template/public/")
add_compile_options(-DFLASK_DASH_SCRIPT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/template/public/js/dash.js")

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(Threads REQUIRED)
file(GLOB DASHCPP_SRC_FILES  ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp)
add_library(dashcpp ${DASHCPP_SRC_FILES})
target_link_libraries(dashcpp Threads::Threads)

if(${BUILD_EXAMPLES})
  add_subdirectory(examples)
else()
  message("exampleをビルドしない設定になっています。ExampleをビルドするにはBUILD_EXMPLESのOptionをONにしてください")
endif()
