#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <map>
#include <string>
#include <led.h>

namespace server
{
enum class Command
{
    UNKNOWN,
    SET_STATE, SET_COLOR, SET_RATE,
    GET_STATE, GET_COLOR, GET_RATE,
    STOP_SERVER
};

enum class ResultStatus {FAILED, OK};

class Server
{
public:
    typedef std::map <std::string, Command> CommandMap;
//     typedef std::map <std::string, led::State> StateMap;
//     typedef std::map <std::string, led::Color> ColorMap;
    
    static const std::string FAILED_STRING;
    static const std::string OK_STRING;
    
//     static const StateMap STATE_MAP;
//     static const ColorMap COLOR_MAP;
    static const CommandMap COMMAND_MAP;
    static const int MAX_COMMAND_LEN = 30;
    
    typedef std::function <bool(Server&, const std::string&, std::string&)> Handler;
    typedef std::map <Command, Handler> HandlersMap;
    Server();
    void Run(const std::string& pipe_in, const std::string& pipe_out);
private:
    bool set_state(const std::string& state_string, std::string& answer);
    bool set_color(const std::string& color_string, std::string& answer);
    bool set_rate(const std::string& rate_string, std::string& answer);
    
    bool get_state(const std::string& args, std::string& answer);
    bool get_color(const std::string& args, std::string& answer);
    bool get_rate(const std::string& args, std::string& answer);
    
    bool unknown_command_handler(const std::string& args, std::string& answer);
    bool stop_server(const std::string& args, std::string& answer);
    bool make_clean_pipe(const std::string& pipe_name);
    bool parse_command(const std::string& buffer, const std::string &pipe_out);
    bool process_command(const std::string& arguments, const std::string &pipe_out);
//     bool read_request();
//     bool write_answer();
    led::LED _led;
    HandlersMap _handlers_map;
    Command _last_command;
    std::string _last_arguments;
};
}

#endif // SERVER_H
