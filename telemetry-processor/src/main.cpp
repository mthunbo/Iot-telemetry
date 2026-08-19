#include "config.hpp"
#include "mqtt_client.hpp"
#include "mqtt_shared.hpp"
#include <iostream>
#include <mosquitto.h>

int main(int argc, char* argv[]) {
    std::string config_path = "";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        }
    }

    if (config_path.empty() || !load_config(config_path)) {
        std::cerr << "Error: Valid configuration file required (--config <path>)" << std::endl;
        return 2;
    }

    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("telemetry_processor_client", true, NULL);
    
    if (!mosq) {
        std::cerr << "Error: Could not create Mosquitto instance." << std::endl;
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    if (!mqtt_connect(mosq, app_config.broker_host, app_config.broker_port)) {
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    std::cout << "[Processor] Starting loop... (Ready for systemd background execution)" << std::endl;
    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}