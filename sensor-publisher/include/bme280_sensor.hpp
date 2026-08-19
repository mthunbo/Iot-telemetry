#pragma once

#include <cstdint>
#include <string>

#include "linux_i2c.hpp"

extern "C" {
#include "bme280.h"
}

/*
 * Student-facing abstraction for a Bosch BME280 on Linux/BeaglePlay.
 *
 * Standard assignment:
 *   - You USE this class.
 *   - You do not need to rewrite the Bosch/Linux integration.
 *   - You should be able to explain the overall flow.
 *
 * Advanced extension:
 *   - Replace this implementation with your own integration of
 *     Bosch BME280 SensorAPI and Linux i2c-dev.
 */

struct Bme280Measurement {
    double temperature_c;
    double humidity_pct;
    double pressure_hpa;
};

class Bme280Sensor {
public:
    Bme280Sensor(std::string i2c_device, std::uint8_t address);

    Bme280Sensor(const Bme280Sensor&) = delete;
    Bme280Sensor& operator=(const Bme280Sensor&) = delete;

    void init();
    Bme280Measurement read();

    bool initialized() const noexcept;
    const std::string& i2cDevice() const noexcept;
    std::uint8_t address() const noexcept;

private:
    static std::string boschError(int8_t code);
    void checkBosch(int8_t code, const char* operation) const;

    LinuxI2c bus_;
    struct bme280_dev dev_ {};
    struct bme280_settings settings_ {};
    std::uint32_t measurement_delay_us_ {0};
    bool initialized_ {false};
};
