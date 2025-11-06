#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class AutoChromaticAssignmentScreen final : public ScreenComponent,
                                                public OpensNameScreen
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        AutoChromaticAssignmentScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void close() override;
        void update(Observable *o, Message message) override;

    private:
        const std::vector<std::string> letters{"A", "B", "C", "D"};
        std::string newName;
        int sourceSoundIndex = -1;
        int originalKey = 67;
        int tune = 0;
        void setSourceSoundIndex(int i);
        void setOriginalKey(int i);
        void setTune(int i);

        void displayOriginalKey() const;
        void displaySource() const;
        void displayTune() const;
        void displayProgramName() const;
        void displaySnd() const;
    };
} // namespace mpc::lcdgui::screens::window
