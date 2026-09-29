#
# Copyright (c) 2026, Daily
#
# CMake package for the Daily Core C++ SDK. Point CMAKE_PREFIX_PATH or
# DailyCore_ROOT to the SDK and use:
#
#   find_package(DailyCore 0.22 REQUIRED)
#   target_link_libraries(my_app PRIVATE DailyCore::DailyCore)
#
# DailyCore::DailyCore has the headers, the library and the system libraries
# it needs. DailyCore_VERSION has the SDK version.
#

include(CMakeFindDependencyMacro)

get_filename_component(_daily_core_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(WIN32)
  set(_daily_core_release "${_daily_core_root}/lib/Release/daily_core.lib")
  set(_daily_core_debug "${_daily_core_root}/lib/Debug/daily_cored.lib")
else()
  set(_daily_core_release "${_daily_core_root}/lib/libdaily_core.a")
  set(_daily_core_debug "")
endif()

if(NOT EXISTS "${_daily_core_release}")
  set(DailyCore_FOUND FALSE)
  set(DailyCore_NOT_FOUND_MESSAGE
    "The Daily Core library is missing: ${_daily_core_release}"
  )
  return()
endif()

if(NOT WIN32 AND NOT APPLE)
  find_dependency(Threads)
endif()

if(NOT TARGET DailyCore::DailyCore)
  add_library(DailyCore::DailyCore STATIC IMPORTED)
  set_target_properties(DailyCore::DailyCore PROPERTIES
    IMPORTED_CONFIGURATIONS RELEASE
    IMPORTED_LOCATION "${_daily_core_release}"
    IMPORTED_LOCATION_RELEASE "${_daily_core_release}"
    # The library includes C++ code, so it needs the C++ standard library.
    IMPORTED_LINK_INTERFACE_LANGUAGES CXX
    INTERFACE_INCLUDE_DIRECTORIES "${_daily_core_root}/include"
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )

  # Windows also has a Debug library.
  if(_daily_core_debug AND EXISTS "${_daily_core_debug}")
    set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
      IMPORTED_CONFIGURATIONS DEBUG
    )
    set_target_properties(DailyCore::DailyCore PROPERTIES
      IMPORTED_LOCATION_DEBUG "${_daily_core_debug}"
    )
  endif()

  # System libraries Daily Core needs.
  if(APPLE)
    foreach(_daily_core_framework
        AppKit AudioToolbox AVFoundation CoreAudio CoreGraphics CoreMedia
        CoreVideo Foundation IOSurface Metal MetalKit OpenGL QuartzCore
        ScreenCaptureKit Security VideoToolbox)
      find_library(DailyCore_${_daily_core_framework}_FRAMEWORK
        ${_daily_core_framework}
      )
      mark_as_advanced(DailyCore_${_daily_core_framework}_FRAMEWORK)
      set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES
          "${DailyCore_${_daily_core_framework}_FRAMEWORK}"
      )
    endforeach()
    # Keeps the Objective-C code Daily Core needs.
    set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
      INTERFACE_LINK_OPTIONS -ObjC
    )
  elseif(WIN32)
    set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES
        bcrypt crypt32 d3d11 dmoguids dwmapi dxgi gdi32 iphlpapi msdmo ncrypt
        ntdll ole32 secur32 shcore strmiids userenv winmm wmcodecdspuuid ws2_32
    )
  else()
    set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
      INTERFACE_LINK_LIBRARIES Threads::Threads ${CMAKE_DL_LIBS} m
    )
  endif()

  # Daily Core is built with this, and everything linked with it must be too.
  if(MSVC)
    set_property(TARGET DailyCore::DailyCore APPEND PROPERTY
      INTERFACE_COMPILE_DEFINITIONS _ITERATOR_DEBUG_LEVEL=0
    )
  endif()
endif()

unset(_daily_core_root)
unset(_daily_core_release)
unset(_daily_core_debug)
unset(_daily_core_framework)
