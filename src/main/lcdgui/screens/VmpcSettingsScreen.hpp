#pragma once
#include <lcdgui/ScreenComponent.hpp>

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
    class MultiRecordingSetupScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui::screens
{
    class VmpcSettingsScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        enum MidiControlMode
        {
            VMPC,
            ORIGINAL
        };
        void turnWheel(int i) override;

        VmpcSettingsScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
        void close() override;
        void function(int) override;

        int getMidiControlMode();
        const bool isNameTypingWithKeyboardEnabled();

    private:
        std::shared_ptr<mpc::lcdgui::Background> easterEgg;
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
        void setNameTypingWithKeyboard(const bool);

        void displayInitialPadMapping();
        void display16LevelsEraseMode();
        void displayAutoConvertWavs();
        void displayMidiControlMode();
        void displayNameTypingWithKeyboard();

        friend class mpc::sampler::Pad;
        friend class mpc::nvram::NvRam;
        friend class mpc::sequencer::Track;
        friend class mpc::disk::SoundLoader;
        friend class mpc::audiomidi::MidiInput;
        friend class mpc::lcdgui::screens::MidiSwScreen;
        friend class mpc::lcdgui::screens::window::MidiInputScreen;
        friend class mpc::lcdgui::screens::window::MultiRecordingSetupScreen;
    };
} // namespace mpc::lcdgui::screens
