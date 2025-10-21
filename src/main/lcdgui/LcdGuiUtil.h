#pragma once

#include "lcdgui/ScreenGroups.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mpc::lcdgui::util {

inline const std::vector<std::string>& getTypableFields(const std::string& screenName)
{
    static const std::unordered_map<std::string, std::vector<std::string>> typableFields
    {
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

    static const std::vector<std::string> empty;

    auto it = typableFields.find(screenName);

    if (it != typableFields.end())
    {
        return it->second;
    }
    else
    {
        return empty;
    }
}

inline bool isTypableField(const std::string& screenName, const std::string& fieldName)
{
    for (const auto& f : getTypableFields(screenName))
    {
        if (f == fieldName)
        {
            return true;
        }
    }

    return false;
}

inline bool isFieldSplittable(const std::shared_ptr<mpc::lcdgui::ScreenComponent>& screen,
                              const std::string& fieldName)
{
    using namespace mpc::lcdgui::screengroups;

    if (!isSamplerScreen(screen))
    {
        return false;
    }

    if (fieldName == "st"
        || fieldName == "end"
        || fieldName == "to"
        || fieldName == "endlengthvalue"
        || fieldName == "start")
    {
        return true;
    }

    return false;
}

} // namespace mpc::lcdgui::util

