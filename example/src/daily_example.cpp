//
// Copyright (c) 2024, Daily
//

#include "daily_example.h"

#include "json.hpp"

#include <pthread.h>
#include <signal.h>
#include <time.h>

#include <iostream>

static const char* DEFAULT_CLIENT_NAME = "Guest";

static DailyAboutClient about_client = {
        .library = "daily-example",
        .version = "0.0.1"};

static bool running = true;
static bool quit = false;

// Simple hashing function so we can fake pattern matching and switch on strings
// as a constexpr so it gets evaluated in compile time for static strings
constexpr unsigned int hash(const char* s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}

static void on_participant_joined(
        const nlohmann::json& participant,
        DailyExampleData* data
) {
    std::string user_id = participant["id"].get<std::string>();
    std::string user_name = participant["info"]["userName"].get<std::string>();

    std::cout << std::endl
              << "Participant joined: " << user_name << " (" << user_id << ")"
              << std::endl;

    data->first_participant_joined = true;
}

static void
on_participant_left(const nlohmann::json& participant, DailyExampleData* data) {
    std::string user_id = participant["id"].get<std::string>();
    std::string user_name = participant["info"]["userName"].get<std::string>();

    std::cout << std::endl
              << "Participant left: " << user_name << " (" << user_id << ")"
              << std::endl;
}

static void event_listener(
        DailyRawCallClientDelegate* delegate,
        const char* event_json,
        intptr_t json_len
) {
    nlohmann::json json = nlohmann::json::parse(event_json);

    auto app_data = static_cast<DailyExampleData*>(delegate);

    auto action = json["action"].get<std::string>();

    switch (hash(action.c_str())) {
    case hash("participant-joined"):
        on_participant_joined(json["participant"], app_data);
        break;
    case hash("participant-left"):
        on_participant_left(json["participant"], app_data);
        break;
    case hash("error"):
        std::cout << "Error: " << json.dump() << std::endl;
        break;
    case hash("request-completed"): {
        auto request_id = json["requestId"]["id"].get<u_int64_t>();
        if (app_data->leave_request_id == request_id) {
            std::cout << std::endl;
            std::cout << "Quitting..." << std::endl;
            quit = true;
        }
        break;
    }
    default:
        break;
    }
}

static WebrtcAudioDeviceModule* create_audio_device_module_cb(
        DailyRawWebRtcContextDelegate* delegate,
        WebrtcTaskQueueFactory* task_queue_factory
) {
    auto app_data = static_cast<DailyExampleData*>(delegate);

    return daily_core_context_create_audio_device_module(
            app_data->device_manager, task_queue_factory
    );
}

static void* get_user_media_cb(
        DailyRawWebRtcContextDelegate* delegate,
        WebrtcPeerConnectionFactory* peer_connection_factory,
        WebrtcThread* webrtc_signaling_thread,
        WebrtcThread* webrtc_worker_thread,
        WebrtcThread* webrtc_network_thread,
        const char* constraints
) {
    auto app_data = static_cast<DailyExampleData*>(delegate);

    return daily_core_context_device_manager_get_user_media(
            app_data->device_manager,
            peer_connection_factory,
            webrtc_signaling_thread,
            webrtc_worker_thread,
            webrtc_network_thread,
            constraints
    );
}

static char* enumerate_devices_cb(DailyRawWebRtcContextDelegate* delegate) {
    auto app_data = static_cast<DailyExampleData*>(delegate);

    return daily_core_context_device_manager_enumerated_devices(
            app_data->device_manager
    );
}

static const char* get_audio_device_cb(DailyRawWebRtcContextDelegate* delegate
) {
    return "";
}

static void set_audio_device_cb(
        DailyRawWebRtcContextDelegate* delegate,
        const char* device_id
) {}

static DailyContextDelegate context_delegate() {
    DailyContextDelegatePtr* ptr = nullptr;
    DailyContextDelegate driver = {.ptr = ptr};
    return driver;
}

static DailyWebRtcContextDelegate webrtc_context_delegate() {
    NativeDeviceManager* device_manager =
            daily_core_context_create_device_manager();

    DailyExampleData* data = new DailyExampleData;
    data->device_manager = device_manager;
    data->request_id = 0;
    data->leave_request_id = -1;
    data->first_participant_joined = false;

    DailyWebRtcContextDelegate webrtc = {
            .ptr = (DailyWebRtcContextDelegatePtr*)data,
            .fns = {.get_user_media = get_user_media_cb,
                    .get_enumerated_devices = enumerate_devices_cb,
                    .create_audio_device_module = create_audio_device_module_cb,
                    .get_audio_device = get_audio_device_cb,
                    .set_audio_device = set_audio_device_cb}};
    return webrtc;
}

static void* mirror_audio_thread(void* args) {
    DailyExampleData* data = (DailyExampleData*)args;
    std::cout << std::endl
              << "Waiting for participants to join..." << std::endl;
    while (running) {
        if (data->first_participant_joined) {
            int16_t frames[320];  // 320 = 16000 / 100 * 2 bytes * 1 channels
            daily_core_context_virtual_speaker_device_read_frames(
                    data->speaker, frames, 160, 0, nullptr, nullptr
            );
            daily_core_context_virtual_microphone_device_write_frames(
                    data->microphone, frames, 160, 0, nullptr, nullptr
            );
        } else {
            usleep(10000);
        }
    }
    pthread_exit(NULL);
}

static void signal_handler(int signum) {
    std::cout << std::endl;
    std::cout << "Interrupted!" << std::endl;
    std::cout << std::endl;
    running = false;
}

static void usage() {
    std::cout << "Usage: daily-example -m MEETING_URL [-t MEETING_TOKEN] -n "
                 "CLIENT_NAME"
              << std::endl;
    std::cout << "  -m    Daily meeting URL" << std::endl;
    std::cout << "  -t    Daily meeting token" << std::endl;
    std::cout << "  -n    This client's name (default: Guest)" << std::endl;
}

int main(int argc, char* argv[]) {
    char* url = nullptr;
    char* token = nullptr;
    char* client_name = (char*)DEFAULT_CLIENT_NAME;

    int opt;
    while ((opt = getopt(argc, argv, "m:t:n:")) != -1) {
        switch (opt) {
        case 'm':
            url = strdup(optarg);
            break;
        case 't':
            token = strdup(optarg);
            break;
        case 'n':
            client_name = strdup(optarg);
            break;
        default:
            usage();
            return EXIT_FAILURE;
        }
    }

    if (optind < argc || url == nullptr) {
        usage();
        return EXIT_SUCCESS;
    }

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    daily_core_set_log_level(DailyLogLevel_Off);

    std::cout << std::endl << "Initializing Daily Core..." << std::endl;
    DailyContextDelegate driver = context_delegate();
    DailyWebRtcContextDelegate webrtc = webrtc_context_delegate();
    daily_core_context_create(driver, webrtc, about_client);

    DailyExampleData* app_data = (DailyExampleData*)webrtc.ptr;

    DailyVirtualSpeakerDevice* speaker =
            daily_core_context_create_virtual_speaker_device(
                    app_data->device_manager, "speaker", 16000, 1, false
            );
    daily_core_context_select_speaker_device(
            app_data->device_manager, "speaker"
    );

    DailyVirtualMicrophoneDevice* microphone =
            daily_core_context_create_virtual_microphone_device(
                    app_data->device_manager, "mic", 16000, 1, false
            );

    DailyRawCallClient* client = daily_core_call_client_create();

    // Update the WebRTC context.
    app_data->client = client;
    app_data->speaker = speaker;
    app_data->microphone = microphone;
    app_data->client_name = std::string(client_name);

    DailyCallClientDelegate delegate = {
            .ptr = app_data, .fns = {.on_event = event_listener}};

    // Setup delegate. This will handle events from the library.
    daily_core_call_client_set_delegate(client, delegate);

    // Subscriptions profiles
    nlohmann::json profiles = nlohmann::json::parse(R"({
      "base": {
        "camera": "unsubscribed",
        "microphone": "subscribed"
      }
    })");

    daily_core_call_client_update_subscription_profiles(
            client, app_data->request_id++, profiles.dump().c_str()
    );

    // Client settings
    nlohmann::json settings = nlohmann::json::parse(R"({
      "inputs": {
        "camera": false,
        "microphone": {
          "isEnabled": true,
          "settings": {
            "deviceId": "mic"
          }
        }
      }
    })");

    daily_core_call_client_set_user_name(
            app_data->client,
            app_data->request_id++,
            app_data->client_name.c_str()
    );

    std::cout << std::endl << "Joining " << url << std::endl;
    daily_core_call_client_join(
            app_data->client,
            app_data->request_id++,
            url,
            token,
            settings.dump().c_str()
    );

    // Create audio mirror thread.
    pthread_t mirror_thread;
    if (pthread_create(&mirror_thread, NULL, mirror_audio_thread, app_data) !=
        0) {
        perror("Error creating mirror thread");
        return EXIT_FAILURE;
    }

    // Wait for mirror thread to finish.
    if (pthread_join(mirror_thread, NULL) != 0) {
        perror("Error joining mirror thread");
        return EXIT_FAILURE;
    }

    // Store the leave request id so we can detect when it finishes in the event
    // handler.
    app_data->leave_request_id = app_data->request_id++;
    daily_core_call_client_leave(app_data->client, app_data->leave_request_id);

    // Nicely wait until client properly leaves.
    while (!quit) {
        sleep(1);
    }

    daily_core_call_client_destroy(app_data->client);

    daily_core_context_destroy();

    return EXIT_SUCCESS;
}
