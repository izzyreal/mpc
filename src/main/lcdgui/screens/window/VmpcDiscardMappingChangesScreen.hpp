#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::lcdgui::screens
{
    class VmpcKeyboardScreen;
    class VmpcMidiScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::lcdgui::screens::window
{
    class VmpcDiscardMappingChangesScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        VmpcDiscardMappingChangesScreen(mpc::Mpc &mpc, const int layerIndex);

        void function(int i) override;
        void close() override;

        std::function<void()> discardAndLeave = []() {};
        std::function<void()> saveAndLeave = []() {};
        std::string stayScreen;

    private:
        std::string nextScreen = "sequencer";

        friend class mpc::lcdgui::screens::VmpcKeyboardScreen;
        friend class mpc::lcdgui::screens::VmpcMidiScreen;
    };
} // namespace mpc::lcdgui::screens::window
