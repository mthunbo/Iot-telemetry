#include "ccs811_sensor.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <chrono>

Ccs811Sensor::Ccs811Sensor(const std::string& device, uint8_t address)
    : device_path(device), i2c_address(address), file_descriptor(-1) {}

Ccs811Sensor::~Ccs811Sensor() {
    if (file_descriptor >= 0) {
        ::close(file_descriptor);
    }
}

bool Ccs811Sensor::write_command(uint8_t cmd) {
    if (::write(file_descriptor, &cmd, 1) != 1) return false;
    return true;
}

bool Ccs811Sensor::write_register(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    if (::write(file_descriptor, buffer, 2) != 2) return false;
    return true;
}

bool Ccs811Sensor::read_registers(uint8_t reg, uint8_t* buffer, int length) {
    if (::write(file_descriptor, &reg, 1) != 1) return false;
    if (::read(file_descriptor, buffer, length) != length) return false;
    return true;
}

bool Ccs811Sensor::init() {
    file_descriptor = open(device_path.c_str(), O_RDWR);
    if (file_descriptor < 0) {
        throw std::runtime_error("CCS811: Could not open I2C device");
    }

    if (ioctl(file_descriptor, I2C_SLAVE, i2c_address) < 0) {
        throw std::runtime_error("CCS811: Could not set I2C address");
    }

    uint8_t hw_id = 0;
    if (!read_registers(0x20, &hw_id, 1) || hw_id != 0x81) {
        throw std::runtime_error("CCS811: Invalid Hardware ID found. Is it connected properly?");
    }

    if (!write_command(0xF4)) {
        throw std::runtime_error("CCS811: Failed to start application");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (!write_register(0x01, 0x10)) {
        throw std::runtime_error("CCS811: Failed to set measurement mode");
    }

    return true;
}

Ccs811Measurement Ccs811Sensor::read() {
    Ccs811Measurement meas = {0, 0, false};
    uint8_t buffer[4];

    if (read_registers(0x02, buffer, 4)) {
        meas.eco2_ppm = (buffer[0] << 8) | buffer[1];
        meas.tvoc_ppb = (buffer[2] << 8) | buffer[3];
        meas.valid = true;
    }
    return meas;
}