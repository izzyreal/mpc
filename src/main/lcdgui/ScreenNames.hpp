#pragma once

#include <string>
#include <optional>
#include <vector>

#include "lcdgui/ScreenRegistry.hpp"
#include "lcdgui/ScreenId.hpp"

inline const std::vector<std::string> knownUnimplementedScreens{
    "format", // missing from json layouts
    "setup"   // missing from json layouts
};

struct ScreenNameEntry
{
    const char *name;
    mpc::lcdgui::ScreenId id;
};

inline constexpr ScreenNameEntry screenNames[] = {
#define X(ns, Class, nameStr) {nameStr, mpc::lcdgui::ScreenId::Class},
    SCREEN_LIST
#undef X
};

inline std::optional<mpc::lcdgui::ScreenId>
getScreenIdByName(const std::string &name)
{
    for (auto &entry : screenNames)
    {
        if (name == entry.name)
            return entry.id;
    }
    return std::nullopt;
}

inline bool isValidScreenName(const std::string &name)
{
    return getScreenIdByName(name).has_value();
}

