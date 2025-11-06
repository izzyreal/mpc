#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog2
{
    class DeleteAllFilesScreen final : public ScreenComponent
    {

    public:
        DeleteAllFilesScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        std::vector<std::string> views{"All Files", ".SND", ".PGM",
                                       ".APS",      ".MID", ".ALL",
                                       ".WAV",      ".SEQ", ".SET"};

        int delete_ = 0;

        void displayDelete();
        void setDelete(int i);
    };
} // namespace mpc::lcdgui::screens::dialog2
