#ifndef TELEMETRY_PROCESSOR_HPP
#define TELEMETRY_PROCESSOR_HPP

#include <string>

std::string process_telemetry(const std::string& raw_payload);

std::string get_current_iso_timestamp();

#endif