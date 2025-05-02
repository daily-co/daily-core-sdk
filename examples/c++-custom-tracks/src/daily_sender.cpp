//
// Copyright (c) 2024-2025 Daily
//

#include "daily_example.h"

#include "json.hpp"

#define _USE_MATH_DEFINES
#include <math.h>
#include <signal.h>
#include <time.h>

#include <atomic>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

static const char* DEFAULT_CLIENT_NAME = "Sender";

// NOTE: Do not modify. This is a way for the server to recognize a known
// client library.
static DailyAboutClient about_client = {
        .library = "daily-core-sdk",
        .version = "0.18.0"
};

static std::atomic<bool> running = true;
static std::atomic<bool> quit = false;

// Simple hashing function so we can fake pattern matching and switch on strings
// as a constexpr so it gets evaluated in compile time for static strings
constexpr unsigned int hash(const char* s, int off = 0) {
    return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
}

static void on_participant_joined(
        const nlohmann::json& participant,
        DailyExampleData* data
) {
    std::string participant_id = participant["id"].get<std::string>();

    std::cout << std::endl
              << "Participant joined " << participant_id << std::endl;
}

static void
on_participant_left(const nlohmann::json& participant, DailyExampleData* data) {
    std::string participant_id = participant["id"].get<std::string>();

    std::cout << std::endl
              << "Participant left " << participant_id << std::endl;
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
        auto request_id = json["requestId"]["id"].get<uint64_t>();
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
    DailyDeviceManager* device_manager =
            daily_core_context_create_device_manager();

    DailyExampleData* data = new DailyExampleData;
    data->device_manager = device_manager;
    data->request_id = 0;
    data->leave_request_id = -1;

    DailyWebRtcContextDelegate webrtc = {
            .ptr = (DailyWebRtcContextDelegatePtr*)data,
            .fns = {.get_user_media = get_user_media_cb,
                    .get_enumerated_devices = enumerate_devices_cb,
                    .create_audio_device_module = create_audio_device_module_cb,
                    .get_audio_device = get_audio_device_cb,
                    .set_audio_device = set_audio_device_cb}
    };
    return webrtc;
}

static void wave_audio_thread_handler(void* args) {
    DailyExampleData* data = static_cast<DailyExampleData*>(args);

    static const int64_t FRAME_COUNT = 160;  // 10 ms of audio at 16 kHz
    static const int16_t SAMPLE_RATE = 16000;
    static const double FREQUENCY = 440.0;
    static const double AMPLITUDE = 3000;

    double phase = 0.0;
    const double phase_increment = 2.0 * M_PI * FREQUENCY / SAMPLE_RATE;

    std::cout << std::endl << "Sending custom track 'cxx-wave'..." << std::endl;
    while (running) {
        int16_t frames[FRAME_COUNT];

        for (int i = 0; i < FRAME_COUNT; ++i) {
            frames[i] = (int16_t)(AMPLITUDE * sin(phase));
            phase += phase_increment;
            if (phase >= 2.0 * M_PI)
                phase -= 2.0 * M_PI;
        }

        daily_core_context_custom_audio_source_write_frames_sync(
                data->custom_audio_source,
                frames,
                16,
                SAMPLE_RATE,
                1,
                FRAME_COUNT
        );
    }
}

static void signal_handler(int signum) {
    std::cout << std::endl;
    std::cout << "Interrupted!" << std::endl;
    std::cout << std::endl;
    running = false;
}

static void usage() {
    std::cout << "Usage: daily_sender -m MEETING_URL [-t MEETING_TOKEN] -n "
                 "CLIENT_NAME"
              << std::endl;
    std::cout << "  -m    Daily meeting URL" << std::endl;
    std::cout << "  -t    Daily meeting token" << std::endl;
    std::cout << "  -n    This client's name (default: " << DEFAULT_CLIENT_NAME
              << ")" << std::endl;
}

int main(int argc, char* argv[]) {
    char* url = nullptr;
    char* token = nullptr;
    char* client_name = (char*)DEFAULT_CLIENT_NAME;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-m") == 0 && i + 1 < argc) {
            url = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            token = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            client_name = argv[++i];
        } else {
            usage();
            return EXIT_FAILURE;
        }
    }

    if (url == nullptr) {
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

    DailyAudioSource* audio_source =
            daily_core_context_create_custom_audio_source();

    DailyRawCallClient* client = daily_core_call_client_create();

    // Update the WebRTC context.
    app_data->client = client;
    app_data->custom_audio_source = audio_source;
    app_data->client_name = std::string(client_name);

    DailyCallClientDelegate delegate = {
            .ptr = app_data, .fns = {.on_event = event_listener}
    };

    // Setup delegate. This will handle events from the library.
    daily_core_call_client_set_delegate(client, delegate);

    // Subscriptions profiles. We don't want to subscribe to any track.
    nlohmann::json profiles = nlohmann::json::parse(R"({
      "base": {
        "camera": "unsubscribed",
        "microphone": "unsubscribed"
      }
    })");

    daily_core_call_client_update_subscription_profiles(
            client, app_data->request_id++, profiles.dump().c_str()
    );

    // Client settings
    nlohmann::json settings = nlohmann::json::parse(R"({
      "inputs": {
        "camera": false,
        "microphone": false
      },
      "publishing": {
        "customAudio": {
          "cxx-wave": true
        }
      }
    })");

    daily_core_call_client_set_user_name(
            app_data->client,
            app_data->request_id++,
            app_data->client_name.c_str()
    );

    // We will be sending an audio wave to a custom track name "cxx-wave".
    const DailyAudioTrack* audio_track =
            daily_core_context_create_custom_audio_track(audio_source);

    daily_core_call_client_add_custom_audio_track(
            client, app_data->request_id++, "cxx-wave", audio_track
    );

    std::cout << std::endl << "Joining " << url << std::endl;
    daily_core_call_client_join(
            app_data->client,
            app_data->request_id++,
            url,
            token,
            settings.dump().c_str()
    );

    // Create audio wave thread.
    std::thread wave_thread(wave_audio_thread_handler, app_data);

    // Wait for wave audio thread to finish.
    wave_thread.join();

    // Store the leave request id so we can detect when it finishes in the event
    // handler.
    app_data->leave_request_id = app_data->request_id++;
    daily_core_call_client_leave(app_data->client, app_data->leave_request_id);

    // Nicely wait until client properly leaves.
    while (!quit) {
        SLEEP_MS(1000);
    }

    daily_core_call_client_destroy(app_data->client);

    daily_core_context_destroy();

    return EXIT_SUCCESS;
}
