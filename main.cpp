#include <iostream>

#include "led.h"
#include "server.h"

int main(int argc, char** argv)
{
    led::LED default_led;
    default_led.PrintState();
//     led::LED led {led::LEDState::ON, led::LEDColor::BLUE, 1};
//     led.PrintState();
    std::cout << "Color.last: " << (int)led::Color::LAST << std::endl;
    std::cout <<"SetLEDColor(Red): " << default_led.SetColor(led::Color::RED) << std::endl;
    std::cout <<"SetLEDColor(LAST): " << default_led.SetColor(led::Color::LAST) << std::endl;
    
    server::Server led_server;
    led_server.Run(argv[1], argv[2]); 
    return 0;
}