#pragma once

#include <cstdint>
#include <string>

extern "C" {
#include "bme280_defs.h"
}

/*
 * Linux platform layer used by the provided BME280 wrapper.
 *
 * This is the layer corresponding to Bosch examples/common:
 *
 *     Bosch callback
 *          ↓
 *     Linux i2c-dev
 *          ↓
 *     /dev/play/grove/i2c or /dev/play/qwiic/i2c
 *
 * The callbacks use the I2C_RDWR ioctl so a register read can be made as
 * one combined I2C transaction (write register address + repeated-start read).
 */

struct LinuxI2cContext {
    int fd {-1};
    std::uint8_t address {0};
};

class LinuxI2c {
public:
    LinuxI2c(std::string device, std::uint8_t address);
    ~LinuxI2c();

    LinuxI2c(const LinuxI2c&) = delete;
    LinuxI2c& operator=(const LinuxI2c&) = delete;

    void openDevice();
    void closeDevice() noexcept;

    bool isOpen() const noexcept;
    const std::string& device() const noexcept;
    std::uint8_t address() const noexcept;

    LinuxI2cContext* context() noexcept;

private:
    std::string device_;
    LinuxI2cContext context_;
};

BME280_INTF_RET_TYPE linuxBme280Read(
    std::uint8_t reg_addr,
    std::uint8_t* reg_data,
    std::uint32_t len,
    void* intf_ptr);

BME280_INTF_RET_TYPE linuxBme280Write(
    std::uint8_t reg_addr,
    const std::uint8_t* reg_data,
    std::uint32_t len,
    void* intf_ptr);

void linuxBme280DelayUs(std::uint32_t period, void* intf_ptr);
