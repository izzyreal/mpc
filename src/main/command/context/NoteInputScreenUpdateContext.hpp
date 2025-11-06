#pragma once

#include <functional>
#include <memory>

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::command::context
{

    struct NoteInputScreenUpdateContext
    {
        bool isSixteenLevelsEnabled;
        bool isAllowCentralNoteAndPadUpdateScreen;
        std::shared_ptr<lcdgui::ScreenComponent> screenComponent;
        std::function<void(int)> setSelectedNote;
        bool currentFieldNameIsFromNote;
    };

} // namespace mpc::command::context
