#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "input/midi/legacy/MidiControlPersistence.hpp"

#include <atomic>

namespace mpc::lcdgui::screens
{
    class VmpcMidiScreen final : public ScreenComponent
    {
    public:
        VmpcMidiScreen(Mpc &, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void function(int i) override;
        void openWindow() override;
        void turnWheel(int i) override;

        bool isLearning();
        void setLearnCandidate(bool isNote, int8_t channelIndex, int8_t number,
                               int8_t value);
        void updateOrAddActivePresetCommand(input::midi::legacy::MidiControlCommand &c);
        std::shared_ptr<input::midi::legacy::MidiControlPreset> getActivePreset();
        bool hasMappingChanged();

        std::atomic_bool shouldSwitch{false};
        std::shared_ptr<input::midi::legacy::MidiControlPreset> switchToPreset =
            std::make_shared<input::midi::legacy::MidiControlPreset>();
        std::shared_ptr<input::midi::legacy::MidiControlPreset> activePreset =
            std::make_shared<input::midi::legacy::MidiControlPreset>();

    private:
        int row = 0;
        int rowOffset = 0;
        int column = 0;
        input::midi::legacy::MidiControlCommand learnCandidate;

        bool learning = false;
        std::shared_ptr<input::midi::legacy::MidiControlPreset> uneditedActivePresetCopy =
            std::make_shared<input::midi::legacy::MidiControlPreset>();

        void setLearning(bool b);
        void acceptLearnCandidate();
        void updateRows();
        void displayUpAndDown();
    };
} // namespace mpc::lcdgui::screens
