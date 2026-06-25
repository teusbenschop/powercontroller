#pragma once
#include <string>

namespace datagram {

void verify_crc(std::string datagram);

using kWh = float;
using kW = float;

struct Message
{
    kWh energy_delivered_to_client_tariff_1{0.0f};
    kWh energy_delivered_to_client_tariff_2{0.0f};
    kWh energy_received_from_client_tariff_1{0.0f};
    kWh energy_received_from_client_tariff_2{0.0f};
    kW actual_power_delivering_to_client{0.0f};
    kW actual_power_receiving_from_client{0.0f};
};

Message parse(const std::string& datagram);

}
