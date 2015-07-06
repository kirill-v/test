#ifndef LED_HPP
#define LED_HPP

#include <algorithm>
#include <array>
#include <iostream>
#include <map>

namespace led
{
enum class State {OFF, ON, LAST};
enum class Color {RED, GREEN, BLUE, LAST};
const int STATES_NUMBER = (const int)State::LAST;
const int COLORS_NUMBER = (const int)Color::LAST;
typedef unsigned int Rate;

class LED
{
public:
    typedef std::map <std::string, led::State> StateMap;
    typedef std::map <std::string, led::Color> ColorMap;
    
    static const State DEFAULT_STATE = State::OFF;
    static const Color DEFAULT_COLOR = Color::GREEN;
    static const Rate DEFAULT_RATE = 3;
    
    static const Rate MIN_RATE = 0;
    static const Rate MAX_RATE = 5;
    static const Rate INVALID_RATE = MAX_RATE + 1;
    
    static const std::array< State, STATES_NUMBER> VALID_STATES;
    static const std::array< Color, COLORS_NUMBER > VALID_COLORS;
    
    static const StateMap STATE_MAP;
    static const ColorMap COLOR_MAP;
            
    bool SetState(const State& led_state)
    {
        if (std::find(LED::VALID_STATES.begin(), LED::VALID_STATES.end(), led_state) == LED::VALID_STATES.end())
        {
            return false;
        }
        _state = led_state;
        return true;
    }
    
    bool SetColor(const Color& color)
    {
        if (std::find(LED::VALID_COLORS.begin(), LED::VALID_COLORS.end(), color) == LED::VALID_COLORS.end())
        {
            return false;
        }
        _color = color;
        return true;
    }
    
    bool SetRate(const Rate& rate)
    {
        if (rate < MIN_RATE || rate > MAX_RATE)
        {
            return false;
        }
        _rate = rate;
        return true;
    }
            
    void PrintState() const
    {
        State state = _state;
        Color color = _color;
        auto state_pair = std::find_if(led::LED::STATE_MAP.begin(), led::LED::STATE_MAP.end(),
                                       [state](const led::LED::StateMap::value_type& pair)
                                       {return pair.second == state;});
        auto color_pair = std::find_if(led::LED::COLOR_MAP.begin(), led::LED::COLOR_MAP.end(),
                                       [color](const led::LED::ColorMap::value_type& pair)
                                       {return pair.second == color;});
        std::cout << "LED: state: " << state_pair->first << ", color: " << color_pair->first << ", rate: " << _rate << std::endl;
    }
    
    State GetState() const { return _state; }
    Color GetColor() const { return _color; }
    Rate GetRate() const { return _rate; }
private:
    State _state = DEFAULT_STATE;
    Color _color = DEFAULT_COLOR;
    Rate _rate = DEFAULT_RATE;
};
}

#endif // LED_HPP
