# C++ translation units for static analysis.
file(GLOB_RECURSE CPPFS_ANALYSIS_FILES CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
  "${PROJECT_SOURCE_DIR}/examples/*.cpp"
  "${PROJECT_SOURCE_DIR}/tests/*.cpp"
)

add_custom_target(check)

find_program(CPPFS_CLANG_TIDY NAMES
  clang-tidy clang-tidy-21 clang-tidy-20 clang-tidy-19
  clang-tidy-18 clang-tidy-17 clang-tidy-16)
if(CPPFS_CLANG_TIDY AND CPPFS_ANALYSIS_FILES)
  add_custom_target(clang-tidy
    COMMAND "${CPPFS_CLANG_TIDY}" -p "${CMAKE_BINARY_DIR}"
      ${CPPFS_ANALYSIS_FILES}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Checking C++ sources with clang-tidy"
    VERBATIM)
  add_dependencies(check clang-tidy)
else()
  message(STATUS "clang-tidy not found: skipping clang-tidy")
endif()

find_program(CPPFS_CPPCHECK NAMES cppcheck)
if(CPPFS_CPPCHECK AND CPPFS_ANALYSIS_FILES)
  add_custom_target(cppcheck
    COMMAND "${CPPFS_CPPCHECK}" --quiet --error-exitcode=1
      --enable=warning,performance,portability --std=c++20
      --language=c++ --suppress=missingIncludeSystem
      "-I${PROJECT_SOURCE_DIR}/include"
      ${CPPFS_ANALYSIS_FILES}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "Checking C++ sources with Cppcheck"
    VERBATIM)
  add_dependencies(check cppcheck)
else()
  message(STATUS "cppcheck not found: skipping cppcheck")
endif()

if(BUILD_TESTING)
  if(TARGET LinkedListTests)
    add_dependencies(check LinkedListTests)
  endif()
  add_custom_command(TARGET check POST_BUILD
    COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${CMAKE_BINARY_DIR}"
      --output-on-failure --no-tests=error
    COMMENT "Running registered CTest tests"
    VERBATIM)
endif()
