#
# Copyright (c) 2024-2026 Daily
#

cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(daily_example LANGUAGES CXX)

# The example is inside the SDK, two levels down. To use another copy of the
# SDK, set DailyCore_ROOT or CMAKE_PREFIX_PATH.
find_package(DailyCore REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_executable(daily_example
  include/json.hpp
  include/daily_example.h
  src/daily_example.cpp
)

target_include_directories(daily_example PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(daily_example PRIVATE DailyCore::DailyCore)

# Windows finds DLLs next to the app, so copy the Daily Core DLL there.
if(WIN32)
  get_target_property(_daily_core_type DailyCore::DailyCore TYPE)
  if(_daily_core_type STREQUAL "SHARED_LIBRARY")
    add_custom_command(TARGET daily_example POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:DailyCore::DailyCore> $<TARGET_FILE_DIR:daily_example>
    )
  endif()
endif()

if(UNIX)
  target_compile_options(daily_example PRIVATE -Wall -fno-rtti)
endif()

if(APPLE)
  target_compile_options(daily_example PRIVATE -fvisibility=hidden)
endif()
