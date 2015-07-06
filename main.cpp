#include <iostream>
#include <string>

#include "led.h"
#include "server.h"

int main(int argc, char** argv)
{
    const std::string pipe_in("/tmp/led_request");
    const std::string pipe_out("/tmp/led_response");
    server::Server led_server;
    led_server.Run(pipe_in, pipe_out);
    return 0;
}
