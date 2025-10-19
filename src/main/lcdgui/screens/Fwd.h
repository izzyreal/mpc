#pragma once

#include "lcdgui/ScreenRegistry.h"

namespace mpc::lcdgui {
#define X(ns, Class, name) namespace ns { class Class; }
SCREEN_LIST
#undef X
}

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;

