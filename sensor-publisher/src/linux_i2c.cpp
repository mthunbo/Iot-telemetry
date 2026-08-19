#include "linux_i2c.hpp"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

LinuxI2c::LinuxI2c(std::string device, std::uint8_t address)
    : device_(std::move(device))
{
    context_.address = address;
}

LinuxI2c::~LinuxI2c()
{
    closeDevice();
}

void LinuxI2c::openDevice()
{
    if (isOpen()) {
        return;
    }

    context_.fd = ::open(device_.c_str(), O_RDWR);
    if (context_.fd < 0) {
        throw std::runtime_error(
            "Could not open " + device_ + ": " + std::strerror(errno));
    }
}

void LinuxI2c::closeDevice() noexcept
{
    if (context_.fd >= 0) {
        ::close(context_.fd);
        context_.fd = -1;
    }
}

bool LinuxI2c::isOpen() const noexcept
{
    return context_.fd >= 0;
}

const std::string& LinuxI2c::device() const noexcept
{
    return device_;
}

std::uint8_t LinuxI2c::address() const noexcept
{
    return context_.address;
}

LinuxI2cContext* LinuxI2c::context() noexcept
{
    return &context_;
}

BME280_INTF_RET_TYPE linuxBme280Read(
    std::uint8_t reg_addr,
    std::uint8_t* reg_data,
    std::uint32_t len,
    void* intf_ptr)
{
    auto* context = static_cast<LinuxI2cContext*>(intf_ptr);

    if (context == nullptr || context->fd < 0 || reg_data == nullptr || len == 0) {
        return static_cast<BME280_INTF_RET_TYPE>(-1);
    }

    // Message 1: select the BME280 register.
    struct i2c_msg messages[2] {};

    messages[0].addr = context->address;
    messages[0].flags = 0;
    messages[0].len = 1;
    messages[0].buf = &reg_addr;

    // Message 2: repeated-start and read N bytes.
    messages[1].addr = context->address;
    messages[1].flags = I2C_M_RD;
    messages[1].len = static_cast<__u16>(len);
    messages[1].buf = reg_data;

    struct i2c_rdwr_ioctl_data transfer {};
    transfer.msgs = messages;
    transfer.nmsgs = 2;

    if (::ioctl(context->fd, I2C_RDWR, &transfer) < 0) {
        return static_cast<BME280_INTF_RET_TYPE>(-1);
    }

    return BME280_INTF_RET_SUCCESS;
}

BME280_INTF_RET_TYPE linuxBme280Write(
    std::uint8_t reg_addr,
    const std::uint8_t* reg_data,
    std::uint32_t len,
    void* intf_ptr)
{
    auto* context = static_cast<LinuxI2cContext*>(intf_ptr);

    if (context == nullptr || context->fd < 0 || reg_data == nullptr || len == 0) {
        return static_cast<BME280_INTF_RET_TYPE>(-1);
    }

    // BME280 I2C write: first register address, then payload bytes.
    std::vector<std::uint8_t> buffer;
    buffer.reserve(static_cast<std::size_t>(len) + 1);
    buffer.push_back(reg_addr);
    buffer.insert(buffer.end(), reg_data, reg_data + len);

    struct i2c_msg message {};
    message.addr = context->address;
    message.flags = 0;
    message.len = static_cast<__u16>(buffer.size());
    message.buf = buffer.data();

    struct i2c_rdwr_ioctl_data transfer {};
    transfer.msgs = &message;
    transfer.nmsgs = 1;

    if (::ioctl(context->fd, I2C_RDWR, &transfer) < 0) {
        return static_cast<BME280_INTF_RET_TYPE>(-1);
    }

    return BME280_INTF_RET_SUCCESS;
}

void linuxBme280DelayUs(std::uint32_t period, void*)
{
    std::this_thread::sleep_for(std::chrono::microseconds(period));
}
