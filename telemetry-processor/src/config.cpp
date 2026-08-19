#include "config.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config app_config;

void print_help() {
    std::cout << "Usage: ./telemetry-processor [OPTIONS]\n\n"
              << "Options:\n"
              << "  --help          Show help message\n"
              << "  --config <path> Path to configuration file (JSON)\n";
}

bool load_config(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Config] Error: Could not open " << path << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;
        app_config.broker_host = j.at("broker_host").get<std::string>();
        app_config.broker_port = j.at("broker_port").get<int>();
        app_config.raw_topic = j.at("raw_topic").get<std::string>();
        app_config.processed_topic = j.at("processed_topic").get<std::string>();
        app_config.gateway_id = j.at("gateway_id").get<std::string>();
    } catch (const json::exception& e) {
        std::cerr << "[Config] JSON Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}