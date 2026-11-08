# Stage Clang's AddressSanitizer runtime DLL on Windows.
# On Linux, Clang/GCC resolve the runtime through the sanitizer link flags;
# no DLL copying is needed. Release and non-ASan builds are unaffected.

include_guard(GLOBAL)

set(CPPFS_ASAN_RUNTIME_DLL "")

if(WIN32 AND CPPFS_ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_cppfs_asan_arch x86_64)
  else()
    set(_cppfs_asan_arch i386)
  endif()

  set(_cppfs_asan_filename "clang_rt.asan_dynamic-${_cppfs_asan_arch}.dll")

  execute_process(
    COMMAND "${CMAKE_CXX_COMPILER}" -print-runtime-dir
    RESULT_VARIABLE _cppfs_asan_result
    OUTPUT_VARIABLE _cppfs_asan_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  if(_cppfs_asan_result EQUAL 0 AND
     EXISTS "${_cppfs_asan_dir}/${_cppfs_asan_filename}")
    set(CPPFS_ASAN_RUNTIME_DLL
      "${_cppfs_asan_dir}/${_cppfs_asan_filename}")
  else()
    execute_process(
      COMMAND "${CMAKE_CXX_COMPILER}" -print-resource-dir
      RESULT_VARIABLE _cppfs_asan_result
      OUTPUT_VARIABLE _cppfs_asan_dir
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )
    if(_cppfs_asan_result EQUAL 0 AND
       EXISTS "${_cppfs_asan_dir}/lib/windows/${_cppfs_asan_filename}")
      set(CPPFS_ASAN_RUNTIME_DLL
        "${_cppfs_asan_dir}/lib/windows/${_cppfs_asan_filename}")
    endif()
  endif()

  if(NOT CPPFS_ASAN_RUNTIME_DLL)
    message(FATAL_ERROR
      "Clang ASan runtime not found: ${_cppfs_asan_filename}. "
      "Check the LLVM compiler-rt installation.")
  endif()

  message(STATUS "Clang ASan runtime: ${CPPFS_ASAN_RUNTIME_DLL}")
endif()

function(cppfs_stage_asan_runtime target_name)
  if(NOT CPPFS_ASAN_RUNTIME_DLL)
    return()
  endif()

  if(NOT TARGET "${target_name}")
    message(FATAL_ERROR "ASan runtime target does not exist: ${target_name}")
  endif()

  # ALL makes the copy run even if the executable was not rebuilt.
  add_custom_target("${target_name}_AsanRuntime" ALL
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "${CPPFS_ASAN_RUNTIME_DLL}"
      "$<TARGET_FILE_DIR:${target_name}>"
    COMMENT "Staging ASan runtime for ${target_name}"
    VERBATIM
  )
  add_dependencies("${target_name}_AsanRuntime" "${target_name}")
endfunction()
