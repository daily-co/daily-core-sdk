#
# Copyright (c) 2026, Daily
#
# Builds the Daily Core shared library (libdaily_core.so, libdaily_core.dylib or
# daily_core.dll) from the static library. It exports only the C API in
# include/daily_core.h, and includes everything else the library needs (the C++
# standard library on Linux, and the system libraries and frameworks), so apps
# only link to it.
#
# From the SDK:
#
#   cmake -S shared-library -B build/shared-library -DCMAKE_BUILD_TYPE=Release
#   cmake --build build/shared-library --config Release
#   cmake --install build/shared-library --config Release --prefix .
#
# This installs lib/libdaily_core.so or lib/libdaily_core.dylib, or
# bin/daily_core.dll and its import library, lib/daily_core.dll.lib. To build it
# from another static library, set DAILY_CORE_STATIC_LIBRARY.
#

cmake_minimum_required(VERSION 3.16)

# Like the static library.
if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target")
endif()

project(daily_core_shared LANGUAGES CXX)

get_filename_component(DAILY_CORE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

if(WIN32)
  set(_static_library "${DAILY_CORE_SDK_DIR}/lib/Release/daily_core.lib")
else()
  set(_static_library "${DAILY_CORE_SDK_DIR}/lib/libdaily_core.a")
endif()
set(DAILY_CORE_STATIC_LIBRARY "${_static_library}" CACHE FILEPATH
  "The Daily Core static library to build the shared library from"
)
if(NOT EXISTS "${DAILY_CORE_STATIC_LIBRARY}")
  message(FATAL_ERROR
    "The Daily Core static library is missing: ${DAILY_CORE_STATIC_LIBRARY}"
  )
endif()

#
# The C API: every daily_core_* function declared in the header.
#
file(READ "${DAILY_CORE_SDK_DIR}/include/daily_core.h" _header)
string(REGEX MATCHALL "[ *]daily_core_[a-z0-9_]+\\(" _declarations "${_header}")
set(DAILY_CORE_EXPORTS)
foreach(_declaration IN LISTS _declarations)
  string(REGEX REPLACE "^[ *](daily_core_[a-z0-9_]+)\\($" "\\1" _function
    "${_declaration}"
  )
  list(APPEND DAILY_CORE_EXPORTS ${_function})
endforeach()
list(REMOVE_DUPLICATES DAILY_CORE_EXPORTS)
list(SORT DAILY_CORE_EXPORTS)
list(LENGTH DAILY_CORE_EXPORTS _count)
if(_count EQUAL 0)
  message(FATAL_ERROR "No daily_core_* functions found in daily_core.h")
endif()
message(STATUS "Exporting ${_count} daily_core_* functions")

# A shared library needs at least one source file.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/daily_core.cpp"
  "// The Daily Core shared library. Its code is all in the static library.\n"
)
add_library(daily_core SHARED "${CMAKE_CURRENT_BINARY_DIR}/daily_core.cpp")

set_target_properties(daily_core PROPERTIES
  LINKER_LANGUAGE CXX
  # daily_core.lib is the static library on Windows.
  IMPORT_SUFFIX ".dll.lib"
  # Apps find it through their rpath.
  INSTALL_NAME_DIR "@rpath"
  BUILD_WITH_INSTALL_NAME_DIR ON
)

target_link_libraries(daily_core PRIVATE "${DAILY_CORE_STATIC_LIBRARY}")

if(WIN32)
  set(_def "${CMAKE_CURRENT_BINARY_DIR}/daily_core.def")
  set(_content "LIBRARY daily_core\nEXPORTS\n")
  foreach(_function IN LISTS DAILY_CORE_EXPORTS)
    string(APPEND _content "  ${_function}\n")
  endforeach()
  file(WRITE "${_def}" "${_content}")
  target_sources(daily_core PRIVATE "${_def}")

  target_link_libraries(daily_core PRIVATE
    bcrypt crypt32 d3d11 dmoguids dwmapi dxgi gdi32 iphlpapi msdmo ncrypt ntdll
    ole32 secur32 shcore strmiids userenv winmm wmcodecdspuuid ws2_32
  )
elseif(APPLE)
  set(_exports "${CMAKE_CURRENT_BINARY_DIR}/daily_core.exports")
  set(_content "")
  foreach(_function IN LISTS DAILY_CORE_EXPORTS)
    string(APPEND _content "_${_function}\n")
    # Pull the function in from the static library.
    target_link_options(daily_core PRIVATE "LINKER:-u,_${_function}")
  endforeach()
  file(WRITE "${_exports}" "${_content}")

  target_link_options(daily_core PRIVATE
    "LINKER:-exported_symbols_list,${_exports}"
    "LINKER:-dead_strip"
    # Keeps the Objective-C code Daily Core needs.
    -ObjC
  )

  foreach(_framework
      AppKit AudioToolbox AVFoundation CoreAudio CoreGraphics CoreMedia
      CoreVideo Foundation IOSurface Metal MetalKit OpenGL QuartzCore
      ScreenCaptureKit Security VideoToolbox)
    find_library(${_framework}_FRAMEWORK ${_framework})
    target_link_libraries(daily_core PRIVATE "${${_framework}_FRAMEWORK}")
  endforeach()
else()
  set(_version_script "${CMAKE_CURRENT_BINARY_DIR}/daily_core.map")
  set(_content "{\n  global:\n")
  foreach(_function IN LISTS DAILY_CORE_EXPORTS)
    string(APPEND _content "    ${_function};\n")
    # Pull the function in from the static library.
    target_link_options(daily_core PRIVATE "LINKER:--undefined=${_function}")
  endforeach()
  string(APPEND _content "  local: *;\n};\n")
  file(WRITE "${_version_script}" "${_content}")

  find_package(Threads REQUIRED)
  target_link_libraries(daily_core PRIVATE Threads::Threads ${CMAKE_DL_LIBS} m)
  target_link_options(daily_core PRIVATE
    "LINKER:--version-script=${_version_script}"
    # Everything must be resolved here, not when an app loads the library.
    "LINKER:--no-undefined"
    "LINKER:--gc-sections"
  )

  # Include the C++ standard library, so the library doesn't depend on it. Some
  # toolchains don't have a static one, and then the system's is used, which
  # still works with apps built with another one, like Unreal Engine's libc++.
  include(CheckCXXSourceCompiles)
  set(CMAKE_REQUIRED_LINK_OPTIONS -static-libstdc++ -static-libgcc)
  check_cxx_source_compiles(
    "#include <string>\nint main() { return static_cast<int>(std::string(\"x\").size()); }"
    DAILY_CORE_HAS_STATIC_LIBSTDCXX
  )
  unset(CMAKE_REQUIRED_LINK_OPTIONS)
  if(DAILY_CORE_HAS_STATIC_LIBSTDCXX)
    target_link_options(daily_core PRIVATE -static-libstdc++ -static-libgcc)
  else()
    message(WARNING
      "No static C++ standard library, so the shared library will need the "
      "system's libstdc++."
    )
  endif()
endif()

install(TARGETS daily_core
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
)
