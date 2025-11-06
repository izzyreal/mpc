#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteSongScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        DeleteSongScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displaySong();
    };
} // namespace mpc::lcdgui::screens::dialog
