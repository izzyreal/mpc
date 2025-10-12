#pragma once

#include <string>
#include <functional>

namespace mpc::lcdgui {
    class ScreenComponent;
}

namespace mpc::controls {

struct PushPadScreenUpdateContext {
    std::string currentScreenName;
    bool isSixteenLevelsEnabled;
    bool isDrumNote;
    bool isAllowCentralNoteAndPadUpdateScreen;
    std::shared_ptr<mpc::lcdgui::ScreenComponent> screenComponent;
    std::function<void(int)> setMpcNote;
    std::function<void(int)> setMpcPad;
    std::string currentFieldName;
    int bank;
};

}
