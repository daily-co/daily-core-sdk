//
// Copyright (c) 2024-2025 Daily
//

#ifndef DAILY_EXAMPLE_H
#define DAILY_EXAMPLE_H

extern "C" {
#include "daily_core.h"
}

#include <string>

// Random grab-bag of application state for the purpose of the example.
struct DailyExampleData {
    DailyRawCallClient* client;
    DailyDeviceManager* device_manager;
    DailyAudioSource* custom_audio_source;

    uint64_t request_id;
    uint64_t leave_request_id;
    bool first_participant_joined;
    std::string client_name;
};

#endif
