#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <thread>

namespace mpc::lcdgui::screens::window
{
    class LoadASetReplaceAddScreen final : public ScreenComponent
    {
    public:
        LoadASetReplaceAddScreen(Mpc &mpc, int layerIndex);
        ~LoadASetReplaceAddScreen() override;
        void waitForLoadThread();

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        std::thread loadThread;
        bool loadReplaceSameSound = false;

        void displayLoadReplaceSound() const;
    };
} // namespace mpc::lcdgui::screens::window
