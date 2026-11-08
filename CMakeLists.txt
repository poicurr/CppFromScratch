cmake_minimum_required(VERSION 3.24)

project(CppFromScratch LANGUAGES CXX)

include(CTest)

option(CPPFS_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(CPPFS_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(CPPFS_ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(CPPFS_ENABLE_TSAN AND (CPPFS_ENABLE_ASAN OR CPPFS_ENABLE_UBSAN))
  message(FATAL_ERROR "TSan must be built separately from ASan/UBSan")
endif()

if(CPPFS_ENABLE_TSAN AND WIN32)
  message(FATAL_ERROR "ThreadSanitizer is not supported on Windows")
endif()

# Apply instrumentation at directory scope BEFORE adding tests/FetchContent.
# Catch2 and project objects must use identical sanitizer settings.
set(CPPFS_SANITIZERS "")
if(CPPFS_ENABLE_ASAN)
  list(APPEND CPPFS_SANITIZERS address)
endif()
if(CPPFS_ENABLE_UBSAN)
  list(APPEND CPPFS_SANITIZERS undefined)
endif()
if(CPPFS_ENABLE_TSAN)
  list(APPEND CPPFS_SANITIZERS thread)
endif()

if(CPPFS_SANITIZERS)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(Clang|AppleClang|GNU)$")
    list(JOIN CPPFS_SANITIZERS "," CPPFS_SANITIZER_FLAGS)
    add_compile_options("-fsanitize=${CPPFS_SANITIZER_FLAGS}" -fno-omit-frame-pointer)
    add_link_options("-fsanitize=${CPPFS_SANITIZER_FLAGS}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND
         CPPFS_ENABLE_ASAN AND NOT CPPFS_ENABLE_UBSAN AND NOT CPPFS_ENABLE_TSAN)
    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address)
  else()
    message(FATAL_ERROR
      "Selected sanitizers require Clang/GCC (MSVC supports ASan only).")
  endif()
endif()

include(cmake/AsanRuntime.cmake)

add_library(CppFromScratch INTERFACE)
add_library(CppFromScratch::CppFromScratch ALIAS CppFromScratch)
target_include_directories(CppFromScratch INTERFACE "${PROJECT_SOURCE_DIR}/include")
target_compile_features(CppFromScratch INTERFACE cxx_std_20)

# Warnings apply only to our sources, not third-party Catch2 internals.
add_library(CppFromScratchWarnings INTERFACE)
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(CppFromScratchWarnings INTERFACE /W4 /permissive-)
else()
  target_compile_options(CppFromScratchWarnings INTERFACE
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# 1. LinkedList
add_executable(LinkedListExample examples/LinkedListExample.cpp)
target_link_libraries(LinkedListExample PRIVATE CppFromScratch::CppFromScratch CppFromScratchWarnings)
cppfs_stage_asan_runtime(LinkedListExample)

# 2. 

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

include(cmake/QualityTools.cmake)

# Running `--target check` must stage the DLL before invoking CTest.
if(CPPFS_ASAN_RUNTIME_DLL)
  foreach(target_name IN ITEMS LinkedListExample LinkedListSmoke LinkedListTests)
    if(TARGET "${target_name}_AsanRuntime")
      add_dependencies(check "${target_name}_AsanRuntime")
    endif()
  endforeach()
endif()
