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
}

namespace mpc::lcdgui::screens
{
    class VmpcSettingsScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VmpcSettingsScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;
        void function(int) override;
        void openWindow() override;
        void up() override;
        void down() override;

        bool isNameTypingWithKeyboardEnabled() const;
        bool isBigTimeShiftEnabled() const;
        bool arePhysicalSoundsEnabled() const;
        int getPhysicalSoundsMixMode() const;
        int getPhysicalSoundsLevel() const;

    private:
        std::shared_ptr<Background> easterEgg;
        const std::vector<std::string> initialPadMappingNames{"VMPC2000XL",
                                                              "ORIGINAL"};
        const std::vector<std::string> sixteenLevelsEraseModeNames{
            "All levels", "Only pressed level"};
        const std::vector<std::string> settingLabels{
            "Initial pad mapping :", "16 levels erase mode:",
            "Auto-convert WAVs   :", "Name typing w. keybd:",
            "Big time shift      :", "Physical sounds     :",
            "Physical mix mode   :", "Physical level      :"};
        const std::vector<std::string> settingNames{
            "initial-pad-mapping",      "16-levels-erase-mode",
            "auto-convert-wavs",        "name-typing-with-keyboard",
            "big-time-shift",           "physical-sounds-enabled",
            "physical-sounds-mix-mode", "physical-sounds-level"};

        static constexpr int VisibleRowCount = 5;
        static constexpr int SettingCount = 8;

        int initialPadMapping = 0;
        int sixteenLevelsEraseMode = 0;
        int autoConvertWavs = 1;
        bool nameTypingWithKeyboardEnabled = true;
        bool bigTimeShiftEnabled = false;
        bool physicalSoundsEnabled = false;
        int physicalSoundsMixMode = 0;
        int physicalSoundsLevel = 15;
        int row = 0;
        int rowOffset = 0;

        void setInitialPadMapping(int);
        void set16LevelsEraseMode(int);
        void setAutoConvertWavs(int);
        void setNameTypingWithKeyboard(bool);
        void setBigTimeShift(bool);
        void setPhysicalSoundsEnabled(bool);
        void setPhysicalSoundsMixMode(int);
        void setPhysicalSoundsLevel(int);

        void displayInitialPadMapping() const;
        void display16LevelsEraseMode() const;
        void displayAutoConvertWavs() const;
        void displayNameTypingWithKeyboard() const;
        void displayBigTimeShift() const;
        void displayRows() const;
        void displayUpAndDown() const;
        void selectSetting(int settingIndex);
        std::string getSettingValue(int settingIndex) const;

        friend class sampler::Pad;
        friend class nvram::NvRam;
        friend class sequencer::Track;
        friend class disk::SoundLoader;
        friend class audiomidi::MidiInput;
        friend class MidiSwScreen;
        friend class MidiInputScreen;
    };
} // namespace mpc::lcdgui::screens
