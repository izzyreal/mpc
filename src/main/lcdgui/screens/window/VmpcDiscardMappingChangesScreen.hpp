#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <functional>

namespace mpc::lcdgui::screens::window
{
    class VmpcDiscardMappingChangesScreen final : public ScreenComponent
    {

    public:
        VmpcDiscardMappingChangesScreen(Mpc &mpc, int layerIndex);

        void function(int i) override;
        void close() override;

        void setNextScreen(std::string nextScreen);

        std::function<void()> discardAndLeave = [] {};
        std::function<void()> saveAndLeave = [] {};
        std::string stayScreen;

    private:
        std::string nextScreen = "sequencer";
    };
} // namespace mpc::lcdgui::screens::window
