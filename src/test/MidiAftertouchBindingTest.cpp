#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"
#include "input/midi/MidiControlPresetV3.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::input::midi;
using namespace mpc::hardware;

namespace
{
    void drain(Mpc &mpc)
    {
        for (int i = 0; i < 3; ++i)
        {
            mpc.getSequencer()->getStateManager()->drainQueue();
            mpc.getPerformanceManager().lock()->drainQueue();
        }
    }

    void prepareSelectedDrumTrackWithProgram(Mpc &mpc, const TrackIndex trackIndex)
    {
        const auto sequencer = mpc.getSequencer();
        sequencer->getSequence(0)->init(1);
        drain(mpc);

        sequencer->getSequence(0)->getTrack(trackIndex)->setBusType(
            sequencer::BusType::DRUM1);
        drain(mpc);

        sequencer->setSelectedTrackIndex(trackIndex);
        drain(mpc);

        const auto program =
            mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
        REQUIRE(program);

        mpc.getSequencer()
            ->getDrumBus(DrumBusIndex(0))
            ->setProgramIndex(program->getProgramIndex());
        drain(mpc);
    }

    ClientMidiEvent makePolyAftertouchEvent(const int channel, const int note,
                                            const int pressure)
    {
        ClientMidiEvent event{ClientMidiEvent::AFTERTOUCH};
        event.setChannel(channel);
        event.setAftertouchNote(note);
        event.setAftertouchValue(pressure);
        return event;
    }

    Binding makeNoteBinding(const std::string &target, const int midiNote)
    {
        Binding binding;
        binding.setTarget(target);
        binding.setMessageType(BindingMessageType::Note);
        binding.setMidiNumber(midiNote);
        return binding;
    }
} // namespace

TEST_CASE("Poly aftertouch ignores non-pad note bindings", "[midi-aftertouch-binding]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    prepareSelectedDrumTrackWithProgram(mpc, TrackIndex(0));

    auto preset = std::make_shared<MidiControlPresetV3>();
    preset->setBindings({
        makeNoteBinding("hardware:pad-15-or-hyphen-exclamation-mark", 51),
        makeNoteBinding("hardware:play", 51),
        makeNoteBinding("hardware:play-start", 51),
    });

    mpc.clientEventController->getClientMidiEventController()
        ->getExtendedController()
        ->setActivePreset(preset);

    auto *controller =
        mpc.clientEventController->getClientMidiEventController().get();

    controller->handleClientMidiEvent(makePolyAftertouchEvent(0, 51, 127));

    REQUIRE_FALSE(mpc.getHardware()->getButton(PLAY)->isPressed());
    REQUIRE_FALSE(mpc.getHardware()->getButton(PLAY_START)->isPressed());
}
