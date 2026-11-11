# A prebuilt, non-sanitized Catch2 library causes MSVC STL ASan annotation
# mismatches. With instrumentation enabled, always build Catch2 from source.
if(NOT CPPFS_SANITIZERS)
  find_package(Catch2 3 QUIET CONFIG)
endif()

if(NOT Catch2_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(Catch2)
  list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif()

add_executable(LinkedListTests LinkedListTests.cpp)
target_link_libraries(LinkedListTests PRIVATE
  CppFromScratch::CppFromScratch CppFromScratchWarnings
  Catch2::Catch2WithMain)
cppfs_stage_asan_runtime(LinkedListTests)

add_executable(StackTests StackTests.cpp)
target_link_libraries(StackTests PRIVATE
  CppFromScratch::CppFromScratch CppFromScratchWarnings
  Catch2::Catch2WithMain)
cppfs_stage_asan_runtime(StackTests)

include(Catch)
catch_discover_tests(LinkedListTests DISCOVERY_MODE PRE_TEST)
catch_discover_tests(StackTests DISCOVERY_MODE PRE_TEST)
