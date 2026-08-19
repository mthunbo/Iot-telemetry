#include "mqtt_shared.hpp"
#include <iostream>

bool mqtt_connect(struct mosquitto* mosq, const std::string& host, int port) {
    int rc = mosquitto_connect(mosq, host.c_str(), port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr << "[MQTT Shared] Error connecting to broker: " << mosquitto_strerror(rc) << std::endl;
        return false;
    }
    return true;
}

bool mqtt_publish(struct mosquitto* mosq, const std::string& topic, const std::string& payload) {
    int rc = mosquitto_publish(mosq, NULL, topic.c_str(), payload.length(), payload.c_str(), 1, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        std::cerr << "[MQTT Shared] Publish failed to " << topic << ": " << mosquitto_strerror(rc) << std::endl;
        return false;
    }
    return true;
}