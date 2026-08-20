#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

struct Config {
    std::string broker_host;
    int broker_port;
    std::string raw_topic;
    std::string device_id;
    std::string i2c_device;
    std::string i2c_address;
    std::string ccs811_address;
    int interval_ms;
};

extern Config app_config;

void print_help();
bool load_config(const std::string& path);

#endif