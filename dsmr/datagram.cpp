#include <charconv>
#include <iostream>
#include <sstream>

#include "datagram.h"
#include <exception.h>

namespace datagram {

void verify_crc(std::string datagram)
{
    // The CRC of the datagram is calculated over the fragment of text
    // where the forward slash starts, inclusive,
    // till where the exclamation mark starts, inclusive.
    std::size_t pos = datagram.find('/');
    if (pos == std::string::npos)
        throw exception::create<exception::base::datagram::Header>("The forward slash was not found to start the datagram");
    datagram.erase(0, pos);

    pos = datagram.find('!');
    if (pos == std::string::npos)
        throw exception::create<exception::base::datagram::Footer>("The exclamation mark was not found to end the datagram");

    const auto stated_hex_crc = datagram.substr(pos + 1, 4);
    datagram.erase(pos + 1);

    const auto calc_crc16 = [] (const std::uint8_t *data, std::size_t length)
    {
        std::uint16_t crc {0};
        while (length--) {
            crc ^= *data++;
            for (int i = 0; i < 8; ++i) {
                if (crc & 1)
                    crc = crc >> 1 ^ 0xa001;
                else
                    crc = crc >> 1;
            }
        }
        return crc;
    };

    const auto hex2int = [](const std::string& hex)
    {
        std::stringstream ss;
        ss << std::hex << hex;
        uint16_t i;
        ss >> i;
        return i;
    };

    const auto int2hex = [](const uint16_t i)
    {
        std::stringstream ss;
        ss << std::hex << i;
        return std::move(ss).str();
    };

    if (const std::uint16_t calculated_crc = calc_crc16(reinterpret_cast<const std::uint8_t*>(datagram.c_str()), datagram.length());
        calculated_crc != hex2int(stated_hex_crc))
        throw exception::create<exception::base::datagram::Crc>("Datagram has stated CRC of", stated_hex_crc, "but calculated CRC is", int2hex(calculated_crc));
}


Message parse(const std::string& datagram)
{
    const auto extract = [&datagram] (const std::string_view obis_code, const std::string_view unit) -> float
    {
        size_t begin = datagram.find(obis_code);
        if (begin == std::string::npos)
            return 0.0f;
        begin+= obis_code.length() + 1;
        size_t end = datagram.find(unit, begin);
        if (end == std::string::npos)
            return 0.0f;
        end--;
        float result{0.0f};
        std::from_chars(datagram.data() + begin, datagram.data() + end, result);
        return result;
    };

    return Message {
        .energy_delivered_to_client_tariff_1 =  extract("1-0:1.8.1", "kWh"),
        .energy_delivered_to_client_tariff_2 =  extract("1-0:1.8.2", "kWh"),
        .energy_received_from_client_tariff_1 = extract("1-0:2.8.1", "kWh"),
        .energy_received_from_client_tariff_2 = extract("1-0:2.8.2", "kWh"),
        .actual_power_delivering_to_client =     extract("1-0:1.7.0", "kW"),
        .actual_power_receiving_from_client =    extract("1-0:2.7.0", "kW"),
    };
}


}
