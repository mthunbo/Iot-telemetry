#ifndef CCS811_SENSOR_HPP
#define CCS811_SENSOR_HPP

#include <string>
#include <cstdint>

struct Ccs811Measurement {
    uint16_t eco2_ppm;
    uint16_t tvoc_ppb;
    bool valid;
};

class Ccs811Sensor {
public:
    Ccs811Sensor(const std::string& device, uint8_t address);
    ~Ccs811Sensor();

    bool init();
    Ccs811Measurement read();

private:
    std::string device_path;
    uint8_t i2c_address;
    int file_descriptor;

    bool write_register(uint8_t reg, uint8_t value);
    bool write_command(uint8_t cmd);
    bool read_registers(uint8_t reg, uint8_t* buffer, int length);
};

#endif