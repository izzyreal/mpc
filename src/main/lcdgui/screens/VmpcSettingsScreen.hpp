#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::audiomidi
{
    class MidiInput;
}
namespace mpc::sampler
{
    class Pad;
}
namespace mpc::sequencer
{
    class Track;
}
namespace mpc::nvram
{
    class NvRam;
}
namespace mpc::disk
{
    class SoundLoader;
}
namespace mpc::lcdgui::screens
{
    class MidiSwScreen;
}
namespace mpc::lcdgui::screens::window
{
    class MidiInputScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui::screens
{
    class VmpcSettingsScreen final : public ScreenComponent
    {

    public:
        enum MidiControlMode
        {
            VMPC,
            ORIGINAL
        };
        void turnWheel(int i) override;

        VmpcSettingsScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;
        void function(int) override;

        int getMidiControlMode() const;
        const bool isNameTypingWithKeyboardEnabled() const;

    private:
        std::shared_ptr<Background> easterEgg;
        const std::vector<std::string> initialPadMappingNames{"VMPC2000XL",
                                                              "ORIGINAL"};
        const std::vector<std::string> _16LevelsEraseModeNames{
            "All levels", "Only pressed level"};
        const std::vector<std::string> midiControlModeNames{
            "VMPC, see MIDI tab", "ORIGINAL"};

        int initialPadMapping = 0;
        int _16LevelsEraseMode = 0;
        int autoConvertWavs = 1;
        int midiControlMode = 0;
        bool nameTypingWithKeyboardEnabled = true;

        void setInitialPadMapping(int i);
        void set16LevelsEraseMode(int i);
        void setAutoConvertWavs(int i);
        void setMidiControlMode(int i);
        void setNameTypingWithKeyboard(bool);

        void displayInitialPadMapping() const;
        void display16LevelsEraseMode() const;
        void displayAutoConvertWavs() const;
        void displayMidiControlMode() const;
        void displayNameTypingWithKeyboard() const;

        friend class sampler::Pad;
        friend class nvram::NvRam;
        friend class sequencer::Track;
        friend class disk::SoundLoader;
        friend class audiomidi::MidiInput;
        friend class MidiSwScreen;
        friend class MidiInputScreen;
    };
} // namespace mpc::lcdgui::screens
