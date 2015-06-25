#include "led.h"

namespace led
{
const std::array<State, STATES_NUMBER> LED::VALID_STATES = {State::OFF, State::ON};
const std::array<Color, COLORS_NUMBER> LED::VALID_COLORS = {Color::RED, Color::GREEN, Color::BLUE};

const LED::StateMap LED::STATE_MAP =
{
    {"on", State::ON},
    {"off", State::OFF}
};

const LED::ColorMap LED::COLOR_MAP = 
{
    {"red", Color::RED},
    {"green", Color::GREEN},
    {"blue", Color::BLUE}
};
}