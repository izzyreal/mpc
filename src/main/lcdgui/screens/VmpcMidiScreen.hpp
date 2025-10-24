#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "nvram/MidiControlPersistence.hpp"

#include <atomic>

namespace mpc::audiomidi
{
    class VmpcMidiControlMode;
    class MidiDeviceDetector;
    class AudioMidiServices;
} // namespace mpc::audiomidi
namespace mpc::lcdgui::screens::window
{
    class VmpcKnownControllerDetectedScreen;
}

namespace mpc::lcdgui::screens
{

    class VmpcMidiScreen
        : public mpc::lcdgui::ScreenComponent
    {
    public:
        VmpcMidiScreen(mpc::Mpc &, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void function(int i) override;
        void openWindow() override;
        void turnWheel(int i) override;

        bool isLearning();
        void setLearnCandidate(const bool isNote, const int8_t channelIndex, const int8_t number, const int8_t value);
        void updateOrAddActivePresetCommand(mpc::nvram::MidiControlCommand &c);
        std::shared_ptr<mpc::nvram::MidiControlPreset> getActivePreset();
        bool hasMappingChanged();

    private:
        int row = 0;
        int rowOffset = 0;
        int column = 0;
        mpc::nvram::MidiControlCommand learnCandidate;

        bool learning = false;
        std::shared_ptr<mpc::nvram::MidiControlPreset> activePreset = std::make_shared<mpc::nvram::MidiControlPreset>();
        std::shared_ptr<mpc::nvram::MidiControlPreset> uneditedActivePresetCopy = std::make_shared<mpc::nvram::MidiControlPreset>();

        std::atomic_bool shouldSwitch = ATOMIC_VAR_INIT(false);
        std::shared_ptr<mpc::nvram::MidiControlPreset> switchToPreset = std::make_shared<mpc::nvram::MidiControlPreset>();

        void setLearning(bool b);
        void acceptLearnCandidate();
        void updateRows();
        void displayUpAndDown();

        friend class mpc::nvram::MidiControlPersistence;
        friend class mpc::audiomidi::VmpcMidiControlMode;
        friend class mpc::audiomidi::MidiDeviceDetector;
        friend class mpc::audiomidi::AudioMidiServices;
        friend class mpc::lcdgui::screens::window::VmpcKnownControllerDetectedScreen;
    };
} // namespace mpc::lcdgui::screens
