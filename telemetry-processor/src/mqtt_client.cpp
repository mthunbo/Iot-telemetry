#include "mqtt_client.hpp"
#include "config.hpp"
#include "telemetry_processor.hpp"
#include "mqtt_shared.hpp"
#include <iostream>
#include <cstring>

void on_connect(struct mosquitto* mosq, void* obj, int reason_code) {
    (void)obj;
    if (reason_code != 0) {
        std::cerr << "[MQTT] Connection error. Code: " << reason_code << std::endl;
        return;
    }
    std::cout << "[MQTT] Connected. Subscribing to: " << app_config.raw_topic << std::endl;
    mosquitto_subscribe(mosq, NULL, app_config.raw_topic.c_str(), 1);
}

void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg) {
    (void)obj;
    if (msg->payload) {
        std::string raw_payload = static_cast<char*>(msg->payload);
        
        std::string processed_payload = process_telemetry(raw_payload);

        if (!processed_payload.empty()) {
            mqtt_publish(mosq, app_config.processed_topic, processed_payload);
        }
    }
}