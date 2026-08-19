#include "config.hpp"
#include "mqtt_shared.hpp"
#include "bme280_sensor.hpp"
#include <nlohmann/json.hpp>
#include <mosquitto.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <exception>

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    std::string config_path = "";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--config" && i + 1 < argc) {
            config_path = argv[++i];
        }
    }

    if (config_path.empty() || !load_config(config_path)) {
        std::cerr << "Error loading config. Use --config <path>" << std::endl;
        return 2;
    }

    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("sensor_publisher_client", true, NULL);
    
    if (!mqtt_connect(mosq, app_config.broker_host, app_config.broker_port)) {
        return 1;
    }
    
    mosquitto_loop_start(mosq);

    uint8_t i2c_addr = static_cast<uint8_t>(std::stoul(app_config.i2c_address, nullptr, 0));

    try {
        std::cout << "[Sensor] Connecting to " << app_config.i2c_device << " at " << app_config.i2c_address << std::endl;
        Bme280Sensor sensor(app_config.i2c_device, i2c_addr);
        sensor.init();
        std::cout << "[Sensor] Initialization successful. Starting publishing loop." << std::endl;

        while (true) {
            auto measurement = sensor.read();

            json payload;
            payload["device_id"] = app_config.device_id;
            payload["temperature_c"] = measurement.temperature_c;
            payload["humidity_pct"] = measurement.humidity_pct;
            payload["pressure_hpa"] = measurement.pressure_hpa;

            std::string payload_str = payload.dump();

            mqtt_publish(mosq, app_config.raw_topic, payload_str);

            std::this_thread::sleep_for(std::chrono::milliseconds(app_config.interval_ms));
        }

    } catch (const std::exception& e) {
        std::cerr << "[Fatal Error] BME280 exception: " << e.what() << std::endl;
    }

    mosquitto_loop_stop(mosq, true);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}