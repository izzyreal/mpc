#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class Mpc2000XlAllFileScreen;
}

namespace mpc::lcdgui::screens::window
{
    class LoadASequenceFromAllScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        LoadASequenceFromAllScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        int sourceSeqIndex = 0;
        void setSourceSeqIndex(int i);
        void displayFile();
        void displayLoadInto();

        std::vector<std::shared_ptr<mpc::sequencer::Sequence>>
            sequencesFromAllFile;

        friend class mpc::lcdgui::screens::window::Mpc2000XlAllFileScreen;
    };
} // namespace mpc::lcdgui::screens::window
