#include "bme280_sensor.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

Bme280Sensor::Bme280Sensor(std::string i2c_device, std::uint8_t address)
    : bus_(std::move(i2c_device), address)
{
}

void Bme280Sensor::init()
{
    if (initialized_) {
        return;
    }

    bus_.openDevice();

    /*
     * Connect Bosch SensorAPI to our Linux platform layer.
     *
     * Bosch knows the BME280 protocol.
     * linuxBme280Read/Write know Linux i2c-dev.
     */
    dev_.intf = BME280_I2C_INTF;
    dev_.intf_ptr = bus_.context();
    dev_.read = linuxBme280Read;
    dev_.write = linuxBme280Write;
    dev_.delay_us = linuxBme280DelayUs;

    checkBosch(bme280_init(&dev_), "bme280_init");

    /*
     * A moderate configuration suitable for periodic room/environment
     * telemetry. Forced mode is selected in read(), one measurement at a time.
     */
    settings_.osr_t = BME280_OVERSAMPLING_2X;
    settings_.osr_p = BME280_OVERSAMPLING_2X;
    settings_.osr_h = BME280_OVERSAMPLING_1X;
    settings_.filter = BME280_FILTER_COEFF_2;

    const std::uint8_t desired_settings =
        BME280_SEL_OSR_TEMP |
        BME280_SEL_OSR_PRESS |
        BME280_SEL_OSR_HUM |
        BME280_SEL_FILTER;

    checkBosch(
        bme280_set_sensor_settings(desired_settings, &settings_, &dev_),
        "bme280_set_sensor_settings");

    checkBosch(
        bme280_cal_meas_delay(&measurement_delay_us_, &settings_),
        "bme280_cal_meas_delay");

    // Small margin on top of Bosch's calculated measurement time.
    measurement_delay_us_ += 1000;

    initialized_ = true;
}

Bme280Measurement Bme280Sensor::read()
{
    if (!initialized_) {
        throw std::runtime_error("Bme280Sensor::read() called before init()");
    }

    /*
     * Forced mode performs one fresh measurement and automatically returns
     * the BME280 to sleep afterwards. This fits a publisher that samples
     * periodically.
     */
    checkBosch(
        bme280_set_sensor_mode(BME280_POWERMODE_FORCED, &dev_),
        "bme280_set_sensor_mode");

    dev_.delay_us(measurement_delay_us_, dev_.intf_ptr);

    struct bme280_data data {};
    checkBosch(
        bme280_get_sensor_data(BME280_ALL, &data, &dev_),
        "bme280_get_sensor_data");

    Bme280Measurement measurement {};
    measurement.temperature_c = data.temperature;
    measurement.humidity_pct = data.humidity;

    // Bosch floating-point pressure is in Pa. Course telemetry uses hPa.
    measurement.pressure_hpa = data.pressure / 100.0;

    return measurement;
}

bool Bme280Sensor::initialized() const noexcept
{
    return initialized_;
}

const std::string& Bme280Sensor::i2cDevice() const noexcept
{
    return bus_.device();
}

std::uint8_t Bme280Sensor::address() const noexcept
{
    return bus_.address();
}

void Bme280Sensor::checkBosch(int8_t code, const char* operation) const
{
    if (code == BME280_OK) {
        return;
    }

    std::ostringstream message;
    message << operation << " failed: " << boschError(code)
            << " (code " << static_cast<int>(code) << ")";
    throw std::runtime_error(message.str());
}

std::string Bme280Sensor::boschError(int8_t code)
{
    switch (code) {
        case BME280_E_NULL_PTR:
            return "null pointer";
        case BME280_E_COMM_FAIL:
            return "I2C communication failed";
        case BME280_E_INVALID_LEN:
            return "invalid transfer length";
        case BME280_E_DEV_NOT_FOUND:
            return "BME280 not found / wrong chip ID";
        case BME280_E_SLEEP_MODE_FAIL:
            return "could not enter sleep mode";
        case BME280_E_NVM_COPY_FAILED:
            return "NVM copy failed";
        case BME280_W_INVALID_OSR_MACRO:
            return "invalid oversampling setting";
        default:
            return "Bosch SensorAPI error";
    }
}
