#pragma once

#include <string>
#include <functional>
#include <memory>

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::sampler
{
    class Program;
}

namespace mpc::command::context
{

    struct NoteInputScreenUpdateContext
    {
        bool isSixteenLevelsEnabled;
        bool isAllowCentralNoteAndPadUpdateScreen;
        std::shared_ptr<mpc::lcdgui::ScreenComponent> screenComponent;
        std::function<void(int)> setMpcNote;
        std::string currentFieldName;
    };

} // namespace mpc::command::context
