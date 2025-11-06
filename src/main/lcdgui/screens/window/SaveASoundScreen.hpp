#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class SaveASoundScreen final : public ScreenComponent
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;

        SaveASoundScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        void setFileType(int i);
        int fileType = 0;
        void displayFileType() const;
        void displayFile() const;
    };
} // namespace mpc::lcdgui::screens::window
