#
# Copyright (c) 2026, Daily
#
# CMake package for the Daily Core C++ SDK. Point CMAKE_PREFIX_PATH or
# DailyCore_ROOT to the SDK and use:
#
#   find_package(DailyCore 0.23 REQUIRED)
#   target_link_libraries(my_app PRIVATE DailyCore::DailyCore)
#
# DailyCore::DailyCore is the shared library, with its headers. It only exports
# the C API and includes everything else it needs, so nothing else has to be
# linked. DailyCore::DailyCoreStatic is the static library, with the system
# libraries it needs. DailyCore_VERSION has the SDK version.
#

include(CMakeFindDependencyMacro)

get_filename_component(_daily_core_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_daily_core_include "${_daily_core_root}/include")

if(WIN32)
  set(_daily_core_shared "${_daily_core_root}/bin/daily_core.dll")
  set(_daily_core_import "${_daily_core_root}/lib/daily_core.dll.lib")
  set(_daily_core_static "${_daily_core_root}/lib/Release/daily_core.lib")
  set(_daily_core_static_debug "${_daily_core_root}/lib/Debug/daily_cored.lib")
elseif(APPLE)
  set(_daily_core_shared "${_daily_core_root}/lib/libdaily_core.dylib")
  set(_daily_core_soname "@rpath/libdaily_core.dylib")
  set(_daily_core_static "${_daily_core_root}/lib/libdaily_core.a")
else()
  set(_daily_core_shared "${_daily_core_root}/lib/libdaily_core.so")
  set(_daily_core_soname "libdaily_core.so")
  set(_daily_core_static "${_daily_core_root}/lib/libdaily_core.a")
endif()

set(_daily_core_has_shared FALSE)
if(EXISTS "${_daily_core_shared}"
    AND (NOT WIN32 OR EXISTS "${_daily_core_import}"))
  set(_daily_core_has_shared TRUE)
endif()

set(_daily_core_has_static FALSE)
if(EXISTS "${_daily_core_static}")
  set(_daily_core_has_static TRUE)
endif()

if(NOT _daily_core_has_shared AND NOT _daily_core_has_static)
  set(DailyCore_FOUND FALSE)
  set(DailyCore_NOT_FOUND_MESSAGE
    "The Daily Core library is missing: ${_daily_core_shared}"
  )
  return()
endif()

if(_daily_core_has_static AND NOT WIN32 AND NOT APPLE)
  find_dependency(Threads)
endif()

# Defines `name` as the static library, with the system libraries it needs.
function(_daily_core_add_static_library name)
  add_library(${name} STATIC IMPORTED)
  set_target_properties(${name} PROPERTIES
    IMPORTED_CONFIGURATIONS RELEASE
    IMPORTED_LOCATION "${_daily_core_static}"
    IMPORTED_LOCATION_RELEASE "${_daily_core_static}"
    # The library includes C++ code, so it needs the C++ standard library.
    IMPORTED_LINK_INTERFACE_LANGUAGES CXX
    INTERFACE_INCLUDE_DIRECTORIES "${_daily_core_include}"
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )

  # Windows also has a Debug library.
  if(_daily_core_static_debug AND EXISTS "${_daily_core_static_debug}")
    set_property(TARGET ${name} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
    set_target_properties(${name} PROPERTIES
      IMPORTED_LOCATION_DEBUG "${_daily_core_static_debug}"
    )
  endif()

  # System libraries Daily Core needs.
  if(APPLE)
    foreach(_framework
        AppKit AudioToolbox AVFoundation CoreAudio CoreGraphics CoreMedia
        CoreVideo Foundation IOSurface Metal MetalKit OpenGL QuartzCore
        ScreenCaptureKit Security VideoToolbox)
      find_library(DailyCore_${_framework}_FRAMEWORK ${_framework})
      mark_as_advanced(DailyCore_${_framework}_FRAMEWORK)
      set_property(TARGET ${name} APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "${DailyCore_${_framework}_FRAMEWORK}"
      )
    endforeach()
    # Keeps the Objective-C code Daily Core needs.
    set_property(TARGET ${name} APPEND PROPERTY INTERFACE_LINK_OPTIONS -ObjC)
  elseif(WIN32)
    set_property(TARGET ${name} APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES
        bcrypt crypt32 d3d11 dmoguids dwmapi dxgi gdi32 iphlpapi msdmo ncrypt
        ntdll ole32 secur32 shcore strmiids userenv winmm wmcodecdspuuid ws2_32
    )
  else()
    set_property(TARGET ${name} APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES Threads::Threads ${CMAKE_DL_LIBS} m
    )
  endif()

  # Daily Core is built with this, and everything linked with it must be too.
  if(MSVC)
    set_property(TARGET ${name} APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS _ITERATOR_DEBUG_LEVEL=0
    )
  endif()
endfunction()

if(_daily_core_has_static AND NOT TARGET DailyCore::DailyCoreStatic)
  _daily_core_add_static_library(DailyCore::DailyCoreStatic)
endif()

if(NOT TARGET DailyCore::DailyCore)
  if(_daily_core_has_shared)
    add_library(DailyCore::DailyCore SHARED IMPORTED)
    set_target_properties(DailyCore::DailyCore PROPERTIES
      IMPORTED_LOCATION "${_daily_core_shared}"
      INTERFACE_INCLUDE_DIRECTORIES "${_daily_core_include}"
    )
    if(WIN32)
      set_target_properties(DailyCore::DailyCore PROPERTIES
        IMPORTED_IMPLIB "${_daily_core_import}"
      )
    else()
      # Lets CMake add the rpath apps need to find the library.
      set_target_properties(DailyCore::DailyCore PROPERTIES
        IMPORTED_SONAME "${_daily_core_soname}"
      )
    endif()
  else()
    # SDKs without the shared library.
    _daily_core_add_static_library(DailyCore::DailyCore)
  endif()
endif()

unset(_daily_core_root)
unset(_daily_core_include)
unset(_daily_core_shared)
unset(_daily_core_import)
unset(_daily_core_soname)
unset(_daily_core_static)
unset(_daily_core_static_debug)
unset(_daily_core_has_shared)
unset(_daily_core_has_static)
