#include "telemetry_processor.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

std::string get_current_iso_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&in_time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

std::string process_telemetry(const std::string& raw_payload) {
    json parsed_json;
    
    try {
        parsed_json = json::parse(raw_payload);
    } catch (const json::parse_error& e) {
        std::cerr << "[Processor] Malformed JSON: " << e.what() << std::endl;
        return "";
    }

    if (!parsed_json.contains("device_id") || !parsed_json.contains("temperature_c") || !parsed_json.contains("humidity_pct") || !parsed_json.contains("pressure_hpa")) {
        std::cerr << "[Processor] Missing required fields in payload." << std::endl;
        return "";
    }

    json processed_json;
    processed_json["device_id"] = parsed_json["device_id"];
    processed_json["gateway_id"] = app_config.gateway_id;
    processed_json["timestamp"] = get_current_iso_timestamp();
    processed_json["temperature_c"] = parsed_json["temperature_c"];
    processed_json["humidity_pct"] = parsed_json["humidity_pct"];
    processed_json["pressure_hpa"] = parsed_json["pressure_hpa"];
    if (parsed_json.contains("eco2_ppm") && parsed_json.contains("tvoc_ppb")) {
        processed_json["eco2_ppm"] = parsed_json["eco2_ppm"];
        processed_json["tvoc_ppb"] = parsed_json["tvoc_ppb"];
    }

    processed_json["status"] = "valid";

    return processed_json.dump();
}