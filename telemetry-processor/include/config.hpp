#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

struct Config {
    std::string broker_host;
    int broker_port;
    std::string raw_topic;        // "sensors/+/raw"
    std::string processed_topic;  // "gateways/bp-01/weather"
    std::string gateway_id;       // "bp-01"
};

extern Config app_config;

void print_help();
bool load_config(const std::string& path);

#endif