#ifndef MQTT_SHARED_HPP
#define MQTT_SHARED_HPP

#include <mosquitto.h>
#include <string>

bool mqtt_connect(struct mosquitto* mosq, const std::string& host, int port);
bool mqtt_publish(struct mosquitto* mosq, const std::string& topic, const std::string& payload);

#endif