#
# Copyright (c) 2026, Daily
#
# CMake package for the Daily Core C++ SDK. Point CMAKE_PREFIX_PATH or
# DailyCore_ROOT to the SDK and use:
#
#   find_package(DailyCore 0.23 REQUIRED)
#   target_link_libraries(my_app PRIVATE DailyCore::DailyCore)
#
# DailyCore::DailyCore is the Daily Core shared library, with its headers. It
# only exports the C API and includes everything else it needs, so nothing else
# has to be linked. DailyCore_VERSION has the SDK version.
#

get_filename_component(_daily_core_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(WIN32)
  set(_daily_core_library "${_daily_core_root}/bin/daily_core.dll")
  set(_daily_core_import "${_daily_core_root}/lib/daily_core.dll.lib")
elseif(APPLE)
  set(_daily_core_library "${_daily_core_root}/lib/libdaily_core.dylib")
  set(_daily_core_soname "@rpath/libdaily_core.dylib")
else()
  set(_daily_core_library "${_daily_core_root}/lib/libdaily_core.so")
  set(_daily_core_soname "libdaily_core.so")
endif()

foreach(_daily_core_file IN ITEMS "${_daily_core_library}" "${_daily_core_import}")
  if(_daily_core_file AND NOT EXISTS "${_daily_core_file}")
    set(DailyCore_FOUND FALSE)
    set(DailyCore_NOT_FOUND_MESSAGE
      "The Daily Core library is missing: ${_daily_core_file}"
    )
    return()
  endif()
endforeach()

if(NOT TARGET DailyCore::DailyCore)
  add_library(DailyCore::DailyCore SHARED IMPORTED)
  set_target_properties(DailyCore::DailyCore PROPERTIES
    IMPORTED_LOCATION "${_daily_core_library}"
    INTERFACE_INCLUDE_DIRECTORIES "${_daily_core_root}/include"
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
endif()

unset(_daily_core_root)
unset(_daily_core_library)
unset(_daily_core_import)
unset(_daily_core_soname)
unset(_daily_core_file)
