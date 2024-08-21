//
// Copyright (c) 2024, Daily
//

#ifndef _DAILY_EXAMPLE_H
#define _DAILY_EXAMPLE_H

extern "C" {
#include "daily_core.h"
}

#include <string>

// Random grab-bag of application state for the purpose of the example.
struct DailyExampleData {
    DailyRawCallClient* client;
    NativeDeviceManager* device_manager;
    DailyVirtualSpeakerDevice* speaker;
    DailyVirtualMicrophoneDevice* microphone;

    u_int64_t request_id;
    u_int64_t leave_request_id;
    std::string client_name;
    bool first_participant_joined;
};

#endif
