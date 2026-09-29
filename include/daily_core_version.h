/* Copyright (c) 2026, Daily */

#pragma once

/*
 * Version of the Daily Core C++ SDK.
 *
 * Only the three numbers below need to change for a release. The string is
 * built from them.
 */

#define DAILY_CORE_VERSION_MAJOR 0
#define DAILY_CORE_VERSION_MINOR 23
#define DAILY_CORE_VERSION_PATCH 0

#define DAILY_CORE_VERSION_STR_(x) #x
#define DAILY_CORE_VERSION_STR(x) DAILY_CORE_VERSION_STR_(x)

/* The version as a string, e.g. "0.22.0". */
#define DAILY_CORE_VERSION                                                     \
    DAILY_CORE_VERSION_STR(DAILY_CORE_VERSION_MAJOR) "."                       \
    DAILY_CORE_VERSION_STR(DAILY_CORE_VERSION_MINOR) "."                       \
    DAILY_CORE_VERSION_STR(DAILY_CORE_VERSION_PATCH)
