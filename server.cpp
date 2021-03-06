#include <cstdio>
 #include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "server.h"

namespace server
{
const std::string Server::FAILED_STRING = "FAILED";
const std::string Server::OK_STRING = "OK";
    
const Server::CommandMap Server::COMMAND_MAP = 
{
    {"set-led-state", Command::SET_STATE},
    {"get-led-state", Command::GET_STATE},
    {"set-led-color", Command::SET_COLOR},
    {"get-led-color", Command::GET_COLOR},
    {"set-led-rate", Command::SET_RATE},
    {"get-led-rate", Command::GET_RATE},
    {"stop-server", Command::STOP_SERVER}
};
    
bool Server::set_state(const std::string& state_string, std::string& answer)
{
    bool success = false;
    auto state_pair = led::LED::STATE_MAP.find(state_string);
    if (state_pair != led::LED::STATE_MAP.end())
    {
        if (_led.SetState(state_pair->second))
        {
            success = true;
        }
    }
    answer = success ? Server::OK_STRING : Server::FAILED_STRING;
    return success;
}

bool Server::set_color(const std::string& color_string, std::string& answer)
{
    bool success = false;
    auto color_pair = led::LED::COLOR_MAP.find(color_string);
    if (color_pair != led::LED::COLOR_MAP.end())
    {
        if (_led.SetColor(color_pair->second))
        {
            success = true;
        }
    }
    answer = success ? Server::OK_STRING : Server::FAILED_STRING;
    return success;
}

bool Server::set_rate(const std::string& rate_string, std::string& answer)
{
    bool success = false;
    int rate = led::LED::INVALID_RATE;
    try
    {
        rate = std::stoi(rate_string);
    }
    catch (...){}
    
    if (_led.SetRate(rate))
    {
        success = true;
    }
    answer = success ? Server::OK_STRING : Server::FAILED_STRING;
    return success;
}

bool Server::get_state(const std::string& args, std::string& answer)
{
    led::State state = _led.GetState();
    auto state_pair = std::find_if(led::LED::STATE_MAP.begin(), led::LED::STATE_MAP.end(),
                                   [&state](const led::LED::StateMap::value_type& pair)
                                   {return pair.second == state;});
    if (state_pair != led::LED::STATE_MAP.end())
    {
        answer = Server::OK_STRING + " " + state_pair->first;
        return true;
    }
    answer = Server::FAILED_STRING;
    return false;
}

bool Server::get_color(const std::string& args, std::string& answer)
{
    led::Color color = _led.GetColor();
    auto color_pair = std::find_if(led::LED::COLOR_MAP.begin(), led::LED::COLOR_MAP.end(),
                                   [&color](const led::LED::ColorMap::value_type& pair)
                                   {return pair.second == color;});
    if (color_pair != led::LED::COLOR_MAP.end())
    {
        answer = Server::OK_STRING + " " + color_pair->first;
        return true;
    }
    answer = Server::FAILED_STRING;
    return false;
}

bool Server::get_rate(const std::string& args, std::string& answer)
{
    led::Rate rate = _led.GetRate();
    if (rate >= led::LED::MIN_RATE && rate <= led::LED::MAX_RATE)
    {
        answer = Server::OK_STRING + " " + std::to_string(rate);
        return true;
    }
    answer = Server::FAILED_STRING;
    return false;
}

bool Server::unknown_command_handler(const std::string& args, std::string& answer)
{
    printf("Unknown command with arguments: %s\n", args.c_str());
    answer = Server::FAILED_STRING;
    return true;
}

bool Server::stop_server(const std::string& args, std::string& answer)
{
    answer = Server::OK_STRING;
    return true;
}

bool Server::make_clean_pipe(const std::string& pipe_name)
{
    if (pipe_name.empty())
    {
        printf("Pipe name is empty\n");
        return false;
    }
    if (access(pipe_name.c_str(), F_OK) == 0)
    {
        if (unlink(pipe_name.c_str()) != 0)
        {
            perror(pipe_name.c_str());
            return false;
        }
    }
    if (mkfifo(pipe_name.c_str(), 0666) != 0)
    {
        perror(pipe_name.c_str());
        return false;
    }
    return true;
}

int write_to_pipe(const std::string& pipe_out, const std::string& message)
{
    int fd_out = open(pipe_out.c_str(), O_WRONLY);
    if (fd_out == -1)
    {
        perror(pipe_out.c_str());
        return -1;
    }
    int bytes = write(fd_out, message.c_str(), message.size()+1);
    if (bytes == -1)
    {
        perror(pipe_out.c_str());
    }
    close(fd_out);
    return bytes;    
}

bool Server::parse_command(const std::string& buffer, const std::string& pipe_out)
{
    auto command_pair = Server::COMMAND_MAP.find(buffer);
    _last_command = (command_pair != Server::COMMAND_MAP.end()) ?
                        command_pair->second : Command::UNKNOWN;
    if (_last_command == Command::STOP_SERVER)
    {
        process_command("", pipe_out);
        printf("Stopping server\n");
        return false;
    }
    return true;
}

bool Server::process_command(const std::string &arguments, const std::string& pipe_out)
{
    std::string answer;
    _handlers_map[_last_command](*this, arguments, answer);
    write_to_pipe(pipe_out, answer);
    _led.PrintState();
}

Server::Server()
{
    _handlers_map = 
    {
        {Command::SET_STATE, &server::Server::set_state},
        {Command::GET_STATE, &server::Server::get_state},
        {Command::SET_COLOR, &server::Server::set_color},
        {Command::GET_COLOR, &server::Server::get_color},
        {Command::SET_RATE, &server::Server::set_rate},
        {Command::GET_RATE, &server::Server::get_rate},
        {Command::STOP_SERVER, &server::Server::stop_server},
        {Command::UNKNOWN, &server::Server::unknown_command_handler}
    };
    
    _last_command = Command::UNKNOWN;
}

void Server::Run(const std::string& pipe_in, const std::string& pipe_out)
{
    printf("Starting server with pipes: %s, %s\n", pipe_in.c_str(), pipe_out.c_str());
    
    if (!make_clean_pipe(pipe_in) || !make_clean_pipe(pipe_out))
    {
        return;
    }
    _led.PrintState();
    int fd_in = open(pipe_in.c_str(), O_RDONLY);
    if (fd_in == -1)
    {
        perror(pipe_in.c_str());
        return;
    }   
    
    char buffer[Server::MAX_COMMAND_LEN + 1] = "\0";
    int bytes_number_read, args_start = 0;
    bool is_reading_args = false;
    for (int i=0; ; )
    {
        if ((bytes_number_read = read(fd_in, buffer + i, 1)) == -1)
        {
            perror("Error has occurred while reading from pipe");
            return;
        }
        else if (bytes_number_read == 0)
        {
            sleep(1);
        }
        else
        {
            if (i >= Server::MAX_COMMAND_LEN - 1)
            {
                buffer[Server::MAX_COMMAND_LEN] = '\0';
                printf("Wrong command format: _%s", buffer);
                char c;
                while (read(fd_in, &c, 1) > 0)
                {
                    printf("%c", c);
                    if (c == '\n')
                    {
                        printf("_");
                        break;
                    }
                }
                _last_command = Command::UNKNOWN;
                process_command("", pipe_out);
                args_start = 0;
                is_reading_args = false;
                i=0;
                buffer[0] = '\0';
            }
            // Finished reading first word (command)
            else if (buffer[i] == ' ' && !is_reading_args)
            {
                args_start = i + 1;
                is_reading_args = true;
                buffer[i] = '\0';
                if (!parse_command(buffer, pipe_out))
                {
                    return;
                }
            }
            // Finished reading the whole message
            else if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                _last_arguments = "";
                if (is_reading_args)
                {
                    _last_arguments = buffer + args_start;
                }
                else if (!parse_command(buffer, pipe_out))
                {
                    return;
                }
                process_command(_last_arguments, pipe_out);
                i = 0;
                args_start = 0;
                is_reading_args = false;
                continue;
            }
            ++i;
        }
    }
}
}
