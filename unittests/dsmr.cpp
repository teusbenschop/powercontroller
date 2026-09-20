/*
Copyright (©) 2026-2026 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wcharacter-conversion"
#pragma GCC diagnostic ignored "-Wmissing-noreturn"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#pragma GCC diagnostic pop
#include <dsmr/datagram.h>

#include "exception.h"

TEST(dsmr, crc)
{
    const std::string datagram =
      "/TEST-METER\r\n"
      "\r\n"
      "1-0:1.8.1(000123.456*kWh)\r\n"
      "1-0:1.7.0(00.345*kW)\r\n"
      "!3E94\r\n";
    datagram::verify_crc(datagram);
    {
        std::string invalid {datagram};
        invalid.erase(invalid.find('/'));
        EXPECT_THROW(datagram::verify_crc(invalid), exception::base::datagram::Header);
    }
    {
        std::string invalid {datagram};
        invalid.erase(invalid.find('!'));
        EXPECT_THROW(datagram::verify_crc(invalid), exception::base::datagram::Footer);
    }
    {
        std::string invalid {datagram};
        char& c = invalid.at(invalid.find('!') + 1);
        ++c;
        EXPECT_THROW(datagram::verify_crc(invalid), exception::base::datagram::Crc);
    }
}

TEST(dsmr, parse)
{
    // Datagram to parse.
    const std::string datagram =
      "/TEST-METER\r\n"
      "\r\n"
      "1-3:0.2.8(40)\r\n"
      "0-0:1.0.0(150117185916W)\r\n"
      "0-0:96.1.1(0000000000000000000000000000000000)\r\n"
      "1-0:1.8.1(000123.456*kWh)\r\n"
      "1-0:1.8.2(000234.567*kWh)\r\n"
      "1-0:2.8.1(000012.345*kWh)\r\n"
      "1-0:2.8.2(000023.321*kWh)\r\n"
      "0-0:96.14.0(0001)\r\n"
      "1-0:1.7.0(01.234*kW)\r\n"
      "1-0:2.7.0(02.321*kW)\r\n"
      "0-0:17.0.0(000.0*kW)\r\n"
      "0-0:96.3.10(1)\r\n"
      "0-0:96.7.21(00008)\r\n"
      "0-0:96.7.9(00007)\r\n"
      "1-0:99.97.0(1)(0-0:96.7.19)(000101000001W)(2147483647*s)\r\n"
      "1-0:32.32.0(00000)\r\n"
      "1-0:32.36.0(00000)\r\n"
      "0-0:96.13.1()\r\n"
      "0-0:96.13.0()\r\n"
      "1-0:31.7.0(001*A)\r\n"
      "1-0:21.7.0(00.332*kW)\r\n"
      "1-0:22.7.0(00.000*kW)\r\n"
      "0-1:24.1.0(003)\r\n"
      "0-1:96.1.0(0000000000000000000000000000000000)\r\n"
      "0-1:24.2.1(150117180000W)(00473.789*m3)\r\n"
      "0-1:24.4.0(1)\r\n"
      "!ABCD\r\n";

    datagram::Message message = datagram::parse(datagram);
    EXPECT_FLOAT_EQ(message.energy_delivered_to_client_tariff_1, 123.456f);
    EXPECT_FLOAT_EQ(message.energy_delivered_to_client_tariff_2, 234.567f);
    EXPECT_FLOAT_EQ(message.energy_received_from_client_tariff_1, 12.345f);
    EXPECT_FLOAT_EQ(message.energy_received_from_client_tariff_2, 23.321f);
    EXPECT_FLOAT_EQ(message.actual_power_delivering_to_client,     1.234f);
    EXPECT_FLOAT_EQ(message.actual_power_receiving_from_client,    2.321f);
}
