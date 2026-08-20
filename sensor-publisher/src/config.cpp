#include "config.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config app_config;

void print_help() {
    std::cout << "Usage: ./sensor-publisher [OPTIONS]\n"
              << "Options:\n  --config <path>   Path to configuration file\n";
}

bool load_config(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    try {
        json j; file >> j;
        app_config.broker_host = j.at("broker_host").get<std::string>();
        app_config.broker_port = j.at("broker_port").get<int>();
        app_config.raw_topic = j.at("raw_topic").get<std::string>();
        app_config.device_id = j.at("device_id").get<std::string>();
        app_config.i2c_device = j.at("i2c_device").get<std::string>();
        app_config.i2c_address = j.at("i2c_address").get<std::string>();
        app_config.ccs811_address = j.at("ccs811_address").get<std::string>();
        app_config.interval_ms = j.at("interval_ms").get<int>();
    } catch (...) {
        return false;
    }
    return true;
}