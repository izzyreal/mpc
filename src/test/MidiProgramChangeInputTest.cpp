#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/window/MidiInputScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

namespace
{
    void drain(Mpc &mpc)
    {
        mpc.getSequencer()->getStateManager()->drainQueue();
        mpc.getPerformanceManager().lock()->drainQueue();
    }

    ClientMidiEvent makeProgramChangeEvent(const int channel,
                                           const int rawProgramNumber)
    {
        ClientMidiEvent event{ClientMidiEvent::PROGRAM_CHANGE};
        event.setChannel(channel);
        event.setProgramNumber(rawProgramNumber);
        return event;
    }

    void prepareSelectedTrack(Mpc &mpc, const TrackIndex trackIndex,
                              const BusType busType)
    {
        const auto sequencer = mpc.getSequencer();
        sequencer->getSequence(0)->init(1);
        drain(mpc);

        sequencer->getSequence(0)->getTrack(trackIndex)->setBusType(busType);
        drain(mpc);

        sequencer->setSelectedTrackIndex(trackIndex);
        drain(mpc);
    }

    ProgramIndex createUsedProgramWithMidiProgramChange(Mpc &mpc,
                                                        const int midiPc)
    {
        const auto program =
            mpc.getSampler()->createNewProgramAddFirstAvailableSlot().lock();
        REQUIRE(program);
        program->setMidiProgramChange(midiPc);
        drain(mpc);
        return program->getProgramIndex();
    }
} // namespace

TEST_CASE("Incoming MIDI program change switches drum program by program MIDI program change number",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), BusType::DRUM1);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(false);

    const auto drumBus = mpc.getSequencer()->getDrumBus(BusType::DRUM1);
    drumBus->setReceivePgmChange(true);
    drumBus->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto matchingProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 10);

    auto event = makeProgramChangeEvent(0, 9);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(drumBus->getProgramIndex() == matchingProgramIndex);
}

TEST_CASE("Incoming MIDI program change respects drum bus receive ignore setting",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), BusType::DRUM1);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(false);

    const auto drumBus = mpc.getSequencer()->getDrumBus(BusType::DRUM1);
    drumBus->setReceivePgmChange(false);
    drumBus->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto targetProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 2);

    auto event = makeProgramChangeEvent(0, 1);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(targetProgramIndex != ProgramIndex(0));
    REQUIRE(drumBus->getProgramIndex() == ProgramIndex(0));
}

TEST_CASE("Incoming MIDI program change respects MIDI input receive channel",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), BusType::DRUM1);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(false);
    midiInputScreen->setReceiveCh(1);

    const auto drumBus = mpc.getSequencer()->getDrumBus(BusType::DRUM1);
    drumBus->setReceivePgmChange(true);
    drumBus->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto targetProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 2);

    auto event = makeProgramChangeEvent(0, 1);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(targetProgramIndex != ProgramIndex(0));
    REQUIRE(drumBus->getProgramIndex() == ProgramIndex(0));
}

TEST_CASE("Incoming MIDI program change does not depend on selected track bus type",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), BusType::MIDI);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(false);

    const auto drumBus1 = mpc.getSequencer()->getDrumBus(BusType::DRUM1);
    const auto drumBus2 = mpc.getSequencer()->getDrumBus(BusType::DRUM2);
    drumBus1->setReceivePgmChange(true);
    drumBus2->setReceivePgmChange(false);
    drumBus1->setProgramIndex(ProgramIndex(0));
    drumBus2->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto matchingProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 3);

    auto event = makeProgramChangeEvent(0, 2);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(drumBus1->getProgramIndex() == matchingProgramIndex);
    REQUIRE(drumBus2->getProgramIndex() == ProgramIndex(0));
}

TEST_CASE("Incoming MIDI program change updates all drum buses set to receive",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), BusType::MIDI);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(false);

    const auto drumBus1 = mpc.getSequencer()->getDrumBus(BusType::DRUM1);
    const auto drumBus2 = mpc.getSequencer()->getDrumBus(BusType::DRUM2);
    drumBus1->setReceivePgmChange(true);
    drumBus2->setReceivePgmChange(true);
    drumBus1->setProgramIndex(ProgramIndex(0));
    drumBus2->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto matchingProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 4);

    auto event = makeProgramChangeEvent(0, 3);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(drumBus1->getProgramIndex() == matchingProgramIndex);
    REQUIRE(drumBus2->getProgramIndex() == matchingProgramIndex);
}

TEST_CASE("Incoming MIDI program change with prog change to seq enabled selects sequence when stopped",
          "[midi-program-change-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto sequencer = mpc.getSequencer();
    sequencer->getSequence(0)->init(1);
    sequencer->getSequence(1)->init(1);
    drain(mpc);

    sequencer->setSelectedSequenceIndex(SequenceIndex(0), false);
    sequencer->setSelectedTrackIndex(7);
    drain(mpc);

    const auto midiInputScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MidiInputScreen>();
    midiInputScreen->setProgChangeSeq(true);

    auto event = makeProgramChangeEvent(0, 1);
    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(sequencer->getSelectedSequenceIndex() == SequenceIndex(1));
    REQUIRE(sequencer->getSelectedTrackIndex() == TrackIndex(7));
}
