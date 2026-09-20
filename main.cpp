#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <pthread.h>
#include <string_view>
#include <unistd.h>
#include "serial.h"
#include "exception.h"
#include "dsmr/datagram.h"


int main()
{
    try
    {
        constexpr std::string_view smart_meter_port {"/dev/ttyUSB0"};
        constexpr speed_t speed {B115200};
        const Serial serial (smart_meter_port, speed);
        while (true)
        {
            std::string datagram = serial.read();
            const datagram::Message message = datagram::parse(datagram);
            std::cout << std::fixed << std::setprecision(3) << message.actual_power_delivering_to_client << " kW" << std::endl;
            std::cout << std::fixed << std::setprecision(3) << message.energy_delivered_to_client_tariff_1 + message.energy_delivered_to_client_tariff_2 <<  " kWh" << std::endl;
        }
        pause();
    }
    catch (const exception::base::Serial& exception)
    {
        std::cout << exception.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
