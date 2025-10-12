#pragma once

#include "lcdgui/ScreenGroups.h"

#include <string>
#include <unordered_map>

namespace mpc::lcdgui::util {

static std::vector<std::string> getTypableFields(const std::string screenName)
{
    static const std::unordered_map<std::string, std::vector<std::string>> typableFields {
        { "sequencer",      { "tempo", "now0", "now1", "now2", "velo" } },
        { "trim",           { "st", "end" } },
        { "loop-end-fine",  { "end", "lngth" } },
        { "start-fine",     { "start" } },
        { "zone-end-fine",  { "end" } },
        { "end-fine",       { "end" } },
        { "zone-start-fine",{ "start" } },
        { "loop-to-fine",   { "to", "lngth" } },
        { "zone",           { "st", "end" } },
        { "loop",           { "to", "endlengthvalue" } },
    };

    if (typableFields.count(screenName) == 0) return {};

    return typableFields.at(screenName);
}

static bool isTypableField(const std::string screenName, const std::string fieldName)
{
    for (auto &f : getTypableFields(screenName))
    {
        if (f == fieldName) return true;
    }

    return false;
}

static bool isFieldSplittable(const std::string screenName, const std::string fieldName)
{
    return lcdgui::screengroups::isSamplerScreen(screenName) &&
        (fieldName == "st" ||
         fieldName == "end" ||
         fieldName == "to" ||
         fieldName == "endlengthvalue" ||
         fieldName == "start");
}
}

