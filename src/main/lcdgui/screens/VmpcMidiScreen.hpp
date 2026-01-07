#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "input/midi/MidiControlPresetV3.hpp"

#include <atomic>

namespace mpc::input::midi
{
    struct MidiControlPresetV3;
}

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

        std::shared_ptr<input::midi::MidiControlPresetV3> getActivePreset();

        bool hasMappingChanged();

        std::atomic_bool shouldSwitch{false};

        std::shared_ptr<input::midi::MidiControlPresetV3> switchToPreset =
            std::make_shared<input::midi::MidiControlPresetV3>();

    private:
        int row = 0;
        int rowOffset = 0;
        int column = 0;
        std::optional<input::midi::Binding> learnCandidate;

        bool learning = false;

        std::shared_ptr<input::midi::MidiControlPresetV3>
            uneditedActivePresetCopy =
                std::make_shared<input::midi::MidiControlPresetV3>();

        void setLearning(bool b);
        void acceptLearnCandidate();
        void updateRows();
        void displayUpAndDown();
    };
} // namespace mpc::lcdgui::screens
