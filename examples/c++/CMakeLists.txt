#
# Copyright (c) 2024, Daily
#

cmake_minimum_required(VERSION 3.16)

project(daily_example LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
  set(CMAKE_CXX_STANDARD 20)
else()
  set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DAILY_EXAMPLE_SOURCES
  src/daily_example.cpp
)

set(DAILY_EXAMPLE_HEADERS
  include/json.hpp
  include/daily_example.h
)

#
# DAILY_CORE_PATH environment variable should be defined.
#
if (NOT DEFINED ENV{DAILY_CORE_PATH})
  message(FATAL_ERROR "You must define DAILY_CORE_PATH environment variable.")
endif()

set(DAILY_CORE_PATH "$ENV{DAILY_CORE_PATH}")

add_executable(daily_example ${DAILY_EXAMPLE_HEADERS} ${DAILY_EXAMPLE_SOURCES})

find_package(DailyCore)

#
# This project header directories.
#
target_include_directories(daily_example
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${DAILY_CORE_INCLUDE_DIRS}
)

target_link_libraries(daily_example
  PRIVATE
  ${DAILY_CORE_LIBRARIES}
)

#
# Specific headers, libraries and flags for each paltform.
#
if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti")
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")

  find_library(CORE_GRAPHICS CoreGraphics)
  find_library(CORE_MEDIA CoreMedia)
  find_library(CORE_AUDIO CoreAudio)
  find_library(CORE_VIDEO CoreVideo)
  find_library(AUDIO_TOOLBOX AudioToolbox)
  find_library(VIDEO_TOOLBOX VideoToolbox)
  find_library(SECURITY Security)
  find_library(FOUNDATION Foundation)
  # The ones below are needed when linking with -ObjC
  find_library(APP_KIT AppKit)
  find_library(AVFOUNDATION AVFoundation)
  find_library(METAL Metal)
  find_library(METAL_KIT MetalKit)
  find_library(OPENGL OpenGL)
  find_library(QUARTZ_CORE QuartzCore)

  target_link_libraries(daily_example
    PRIVATE
    ${CORE_GRAPHICS}
    ${CORE_MEDIA}
    ${CORE_AUDIO}
    ${CORE_VIDEO}
    ${AUDIO_TOOLBOX}
    ${VIDEO_TOOLBOX}
    ${SECURITY}
    ${FOUNDATION}
    # The ones below are needed when linking with -ObjC
    -ObjC
    ${APP_KIT}
    ${AVFOUNDATION}
    ${METAL}
    ${METAL_KIT}
    ${OPENGL}
    ${QUARTZ_CORE}
  )
endif()

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_ITERATOR_DEBUG_LEVEL=0")

  # System libraries.
  target_link_libraries(daily_example
    PRIVATE
    msdmo
    wmcodecdspuuid
    dmoguids
    iphlpapi
    ole32
    secur32
    winmm
    ws2_32
    strmiids
    d3d11
    gdi32
    dxgi
    dwmapi
    shcore
    ntdll
    userenv
    bcrypt
  )
endif()
