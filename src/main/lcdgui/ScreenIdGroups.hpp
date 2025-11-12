#pragma once

#include "ScreenId.hpp"

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::lcdgui::screengroups
{
    bool isSoundScreen(ScreenId);
    bool isPadDoesNotTriggerNoteEventScreen(ScreenId);
} // namespace mpc::lcdgui::screengroups
