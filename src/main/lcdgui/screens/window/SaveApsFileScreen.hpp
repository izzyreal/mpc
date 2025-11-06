#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SaveApsFileScreen final : public ScreenComponent,
                                    public OpensNameScreen
    {
    public:
        SaveApsFileScreen(Mpc &mpc, int layerIndex);

        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        void open() override;

        std::string getFileName() const;

    private:
        const std::vector<std::string> apsSaveNames{"APS ONLY", "WITH SOUNDS",
                                                    "WITH .WAV"};

        std::string fileName = "ALL_PGMS";

        void displayFile() const;
        void displaySave() const;
        void displayReplaceSameSounds() const;
    };
} // namespace mpc::lcdgui::screens::window
