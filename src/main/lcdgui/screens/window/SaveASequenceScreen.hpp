#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SaveASequenceScreen final : public ScreenComponent,
                                      public OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        SaveASequenceScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        void displaySaveAs() const;
        void displayFile() const;

        int saveSequenceAs = 1;
        void setSaveSequenceAs(int i);
    };
} // namespace mpc::lcdgui::screens::window
