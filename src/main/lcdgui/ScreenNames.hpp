#pragma once

#include <vector>
#include <string>

#include "lcdgui/ScreenRegistry.hpp"

inline const std::vector<std::string> knownUnimplementedScreens{
    "format", // missing from json layouts
    "setup"   // missing from json layouts
};

inline const std::vector<std::string> screenNames = {
#define X(ns, Class, name) name,
    SCREEN_LIST
#undef X
};

static bool isValidScreenName(const std::string screenName)
{
    return std::find(screenNames.begin(), screenNames.end(), screenName) !=
           screenNames.end();
}
