#
# Copyright (c) 2024-2026 Daily
#

cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target")
endif()

if(MSVC)
  set(CMAKE_CXX_STANDARD 20)
else()
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(daily_example LANGUAGES CXX)

# The example is inside the SDK, two levels down. To use another copy of the
# SDK, set DailyCore_ROOT or CMAKE_PREFIX_PATH.
find_package(DailyCore REQUIRED HINTS "${CMAKE_CURRENT_SOURCE_DIR}/../..")

foreach(app daily_sender daily_receiver)
  add_executable(${app}
    include/json.hpp
    include/daily_example.h
    src/${app}.cpp
  )

  target_include_directories(${app} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

  target_link_libraries(${app} PRIVATE DailyCore::DailyCore)

  if(UNIX)
    target_compile_options(${app} PRIVATE -Wall -fno-rtti)
  endif()

  if(APPLE)
    target_compile_options(${app} PRIVATE -fvisibility=hidden)
  endif()
endforeach()
