#pragma once

#include "client/event/ClientMidiEvent.hpp"
#include "hardware/ComponentId.hpp"
#include "midi/input/MidiControlTargetBinding.hpp"

#include <memory>
#include <vector>

namespace mpc::lcdgui::screens
{
    class MidiSwScreen;
}
namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::controller
{

    enum class MidiFootswitchFunction
    {
        PLAY_START,
        PLAY,
        STOP,
        REC_PLAY,
        ODUB_PLAY,
        REC_PUNCH,
        ODUB_PUNCH,
        TAP,
        PAD_BANK_A,
        PAD_BANK_B,
        PAD_BANK_C,
        PAD_BANK_D,
        PAD_1,
        PAD_2,
        PAD_3,
        PAD_4,
        PAD_5,
        PAD_6,
        PAD_7,
        PAD_8,
        PAD_9,
        PAD_10,
        PAD_11,
        PAD_12,
        PAD_13,
        PAD_14,
        PAD_15,
        PAD_16,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6
    };

    class ClientHardwareEventController;

    class ClientMidiFootswitchAssignmentController
    {
    public:
        static constexpr int SWITCH_COUNT = 40;

        ClientMidiFootswitchAssignmentController(
            const std::shared_ptr<ClientHardwareEventController> &,
            const std::shared_ptr<lcdgui::screens::MidiSwScreen> &,
            const std::shared_ptr<sequencer::Sequencer> &);

        void handleEvent(const client::event::ClientMidiEvent &);

        std::vector<midi::input::MidiControlTargetBinding> bindings;

        void initializeDefaultBindings();

    private:
        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;
        std::shared_ptr<lcdgui::screens::MidiSwScreen> midiSwScreen;
        std::shared_ptr<sequencer::Sequencer> sequencer;

        void dispatchSequencerCommand(
            midi::input::MidiControlTarget::SequencerTarget::Command);

        void triggerDualButtonCombo(hardware::ComponentId,
                                    hardware::ComponentId);
        void handleRecPunch();
        void handleOdubPunch();
        void pressButton(hardware::ComponentId) const;
        void releaseButton(hardware::ComponentId) const;
        void handleStopToPlay();
        void handleRecordingToPlay();
    };

} // namespace mpc::controller
