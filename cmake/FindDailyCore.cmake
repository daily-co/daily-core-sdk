#
# Copyright (c) 2024-2026, Daily
#
# Finds the Daily Core C++ SDK for projects that use find_package(DailyCore)
# with this file in their CMAKE_MODULE_PATH.
#
# New projects don't need this file: point CMAKE_PREFIX_PATH or DailyCore_ROOT
# to the SDK and link to DailyCore::DailyCore (see DailyCoreConfig.cmake).
#
# Looks in the DAILY_CORE_PATH environment variable, if it's set, and defines:
#
#   DAILY_CORE_FOUND      - Whether the SDK was found.
#   DAILY_CORE_INCLUDE_DIRS
#   DAILY_CORE_LIBRARIES  - The static library, as before, with its system
#                           libraries.
#
# It also defines the package's targets, DailyCore::DailyCore and
# DailyCore::DailyCoreStatic.
#

set(_daily_core_version)
if(DailyCore_FIND_VERSION)
  set(_daily_core_version "${DailyCore_FIND_VERSION}")
  if(DailyCore_FIND_VERSION_EXACT)
    list(APPEND _daily_core_version EXACT)
  endif()
endif()

find_package(DailyCore ${_daily_core_version} CONFIG QUIET
  HINTS "$ENV{DAILY_CORE_PATH}" "${CMAKE_CURRENT_LIST_DIR}/.."
)
unset(_daily_core_version)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DailyCore CONFIG_MODE)

set(DAILY_CORE_FOUND ${DailyCore_FOUND})
if(DailyCore_FOUND)
  if(TARGET DailyCore::DailyCoreStatic)
    set(DAILY_CORE_LIBRARIES DailyCore::DailyCoreStatic)
  else()
    set(DAILY_CORE_LIBRARIES DailyCore::DailyCore)
  endif()
  get_target_property(DAILY_CORE_INCLUDE_DIRS
    ${DAILY_CORE_LIBRARIES} INTERFACE_INCLUDE_DIRECTORIES
  )
endif()
