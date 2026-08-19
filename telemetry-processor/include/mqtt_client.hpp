#ifndef MQTT_CLIENT_HPP
#define MQTT_CLIENT_HPP

#include <mosquitto.h>

void on_connect(struct mosquitto* mosq, void* obj, int reason_code);
void on_message(struct mosquitto* mosq, void* obj, const struct mosquitto_message* msg);

#endif