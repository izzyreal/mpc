#pragma once

#include <memory>

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::lcdgui::screengroups
{

    bool isStepEditorScreen(const std::shared_ptr<ScreenComponent> &);
    bool isNextSeqScreen(const std::shared_ptr<ScreenComponent> &);
    bool isScreenThatIsNotAllowedToOpenMainScreen(
        const std::shared_ptr<ScreenComponent> &);
    bool isPlayOnlyScreen(const std::shared_ptr<ScreenComponent> &);
    bool
    isCentralNoteAndPadUpdateScreen(const std::shared_ptr<ScreenComponent> &);
    bool isPlayAndRecordScreen(const std::shared_ptr<ScreenComponent> &);
    bool isPlayScreen(const std::shared_ptr<ScreenComponent> &);
    bool isSamplerScreen(const std::shared_ptr<ScreenComponent> &);
    bool isSoundScreen(const std::shared_ptr<ScreenComponent> &);
    bool isPadDoesNotTriggerNoteEventScreen(
        const std::shared_ptr<ScreenComponent> &);

} // namespace mpc::lcdgui::screengroups
