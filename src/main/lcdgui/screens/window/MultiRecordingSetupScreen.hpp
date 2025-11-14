#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "lcdgui/screens/window/MultiRecordingSetupLine.hpp"

namespace mpc::lcdgui::screens::window
{

    class MultiRecordingSetupScreen final : public ScreenComponent
    {

        int yPos = 0;
        int yOffset = 0;
        std::vector<MultiRecordingSetupLine> mrsLines =
            std::vector<MultiRecordingSetupLine>(34);
        std::vector<MultiRecordingSetupLine *> visibleMrsLines;
        std::vector<std::string> inNames = std::vector<std::string>(34);

    public:
        void left() override;
        void right() override;
        void up() override;
        void down() override;
        void turnWheel(int increment) override;
        void function(int i) override;

        MultiRecordingSetupScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayMrsLine(int i) const;
        void setYOffset(int i);
        void setMrsTrack(int inputNumber, int newTrackNumber);

    public:
        std::vector<MultiRecordingSetupLine *> getMrsLines();
    };
} // namespace mpc::lcdgui::screens::window
