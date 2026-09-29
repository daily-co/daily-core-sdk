#
# Copyright (c) 2026, Daily
#
# Version of the Daily Core C++ SDK, read from include/daily_core_version.h.
#
# A newer version with the same major version is compatible, e.g. 0.23.0 is
# found by find_package(DailyCore 0.22). Version ranges, like
# find_package(DailyCore 0.22...0.24), are supported too.
#

set(PACKAGE_VERSION "")

file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/../include/daily_core_version.h"
  _daily_core_version_lines
  REGEX "^#define DAILY_CORE_VERSION_(MAJOR|MINOR|PATCH) +[0-9]+ *$"
)
foreach(_daily_core_line IN LISTS _daily_core_version_lines)
  if(_daily_core_line MATCHES
      "^#define DAILY_CORE_VERSION_(MAJOR|MINOR|PATCH) +([0-9]+)")
    set(_daily_core_version_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}")
  endif()
endforeach()

if(DEFINED _daily_core_version_MAJOR
    AND DEFINED _daily_core_version_MINOR
    AND DEFINED _daily_core_version_PATCH)
  set(PACKAGE_VERSION
    "${_daily_core_version_MAJOR}.${_daily_core_version_MINOR}.${_daily_core_version_PATCH}"
  )
endif()

if(PACKAGE_VERSION STREQUAL "")
  # No version header, so this isn't a complete SDK.
  set(PACKAGE_VERSION "unknown")
  set(PACKAGE_VERSION_UNSUITABLE TRUE)
elseif(CMAKE_SIZEOF_VOID_P AND NOT CMAKE_SIZEOF_VOID_P STREQUAL "8")
  # The SDK is only built for 64-bit.
  set(PACKAGE_VERSION "${PACKAGE_VERSION} (64-bit)")
  set(PACKAGE_VERSION_UNSUITABLE TRUE)
elseif(PACKAGE_FIND_VERSION_RANGE)
  set(PACKAGE_VERSION_COMPATIBLE TRUE)
  if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION_MIN
      OR (PACKAGE_FIND_VERSION_RANGE_MAX STREQUAL "INCLUDE"
        AND PACKAGE_VERSION VERSION_GREATER PACKAGE_FIND_VERSION_MAX)
      OR (PACKAGE_FIND_VERSION_RANGE_MAX STREQUAL "EXCLUDE"
        AND PACKAGE_VERSION VERSION_GREATER_EQUAL PACKAGE_FIND_VERSION_MAX))
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
  endif()
else()
  set(PACKAGE_VERSION_COMPATIBLE FALSE)
  if(NOT PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION
      AND PACKAGE_FIND_VERSION_MAJOR STREQUAL _daily_core_version_MAJOR)
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
  endif()
  if(PACKAGE_FIND_VERSION STREQUAL PACKAGE_VERSION)
    set(PACKAGE_VERSION_EXACT TRUE)
  endif()
endif()

unset(_daily_core_version_lines)
unset(_daily_core_line)
unset(_daily_core_version_MAJOR)
unset(_daily_core_version_MINOR)
unset(_daily_core_version_PATCH)
