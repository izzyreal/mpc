#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/window/MultiRecordingSetupScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc;
using namespace mpc::client::event;

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

    void prepareSelectedTrack(Mpc &mpc, const TrackIndex trackIndex,
                              const sequencer::BusType busType)
    {
        const auto sequencer = mpc.getSequencer();
        sequencer->getSequence(0)->init(1);
        drain(mpc);

        sequencer->getSequence(0)->getTrack(trackIndex)->setBusType(busType);
        drain(mpc);

        sequencer->setSelectedTrackIndex(trackIndex);
        drain(mpc);
    }

    ClientMidiEvent makeNoteOnEvent(const int channel, const int note,
                                    const int velocity)
    {
        ClientMidiEvent event{ClientMidiEvent::NOTE_ON};
        event.setChannel(channel);
        event.setNoteNumber(note);
        event.setVelocity(velocity);
        return event;
    }

    ClientMidiEvent makeNoteOffEvent(const int channel, const int note)
    {
        ClientMidiEvent event{ClientMidiEvent::NOTE_OFF};
        event.setChannel(channel);
        event.setNoteNumber(note);
        event.setVelocity(0);
        return event;
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

    ClientMidiEvent makeChannelPressureEvent(const int channel,
                                             const int pressure)
    {
        ClientMidiEvent event{ClientMidiEvent::CHANNEL_PRESSURE};
        event.setChannel(channel);
        event.setChannelPressure(pressure);
        return event;
    }

    ClientMidiEvent makeControlChangeEvent(const int channel,
                                           const int controllerNumber,
                                           const int controllerValue)
    {
        ClientMidiEvent event{ClientMidiEvent::CONTROLLER};
        event.setChannel(channel);
        event.setControllerNumber(controllerNumber);
        event.setControllerValue(controllerValue);
        return event;
    }

    ClientMidiEvent makeProgramChangeEvent(const int channel,
                                           const int rawProgramNumber)
    {
        ClientMidiEvent event{ClientMidiEvent::PROGRAM_CHANGE};
        event.setChannel(channel);
        event.setProgramNumber(rawProgramNumber);
        return event;
    }

    ProgramIndex createUsedProgramWithMidiProgramChange(Mpc &mpc,
                                                        const int midiPc)
    {
        const auto program =
            mpc.getSampler()->createNewProgramAddFirstAvailableSlotAndThen({}).lock();
        REQUIRE(program);
        program->setMidiProgramChange(midiPc);
        drain(mpc);
        return program->getProgramIndex();
    }

    void playUserReportedPolyAftertouchBurst(Mpc &mpc)
    {
        constexpr int channel = 0;
        constexpr int note = 51;
        constexpr int velocity = 34;
        const int pressures[] = {127, 126, 125, 123, 121, 120, 117,
                                 114, 109, 105, 100, 95,  91,  89,
                                 85,  81,  73,  49,  17,  1};

        auto *controller =
            mpc.clientEventController->getClientMidiEventController().get();

        controller->handleClientMidiEvent(
            makeNoteOnEvent(channel, note, velocity));
        for (const auto pressure : pressures)
        {
            controller->handleClientMidiEvent(
                makePolyAftertouchEvent(channel, note, pressure));
        }
        controller->handleClientMidiEvent(makeNoteOffEvent(channel, note));
    }
} // namespace

TEST_CASE("Incoming poly aftertouch on an OFF multi-record input line is ignored",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    mpc.getSequencer()->setRecordingModeMulti(true);
    const auto multiRecordingSetupScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();
    multiRecordingSetupScreen->getMrsLines()[0]->setTrack(-1);

    auto event = makePolyAftertouchEvent(0, 60, 100);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 0);
}

TEST_CASE("Incoming note on on an OFF multi-record input line is ignored",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    mpc.getSequencer()->setRecordingModeMulti(true);
    const auto multiRecordingSetupScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();
    multiRecordingSetupScreen->getMrsLines()[0]->setTrack(-1);

    auto event = makeNoteOnEvent(0, 60, 100);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);
    drain(mpc);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 0);
}

TEST_CASE("User reported poly aftertouch burst does not crash on a drum track",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), sequencer::BusType::DRUM1);

    playUserReportedPolyAftertouchBurst(mpc);
    drain(mpc);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 0);
}

TEST_CASE("User reported poly aftertouch burst does not crash on a MIDI track",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), sequencer::BusType::MIDI);

    playUserReportedPolyAftertouchBurst(mpc);
    drain(mpc);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 0);
}

TEST_CASE("Incoming channel pressure on an OFF multi-record input line is ignored",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    mpc.getSequencer()->setRecordingModeMulti(true);
    const auto multiRecordingSetupScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();
    multiRecordingSetupScreen->getMrsLines()[0]->setTrack(-1);

    auto event = makeChannelPressureEvent(0, 100);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(event);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 0);
}

TEST_CASE("Incoming control change on an OFF multi-record input line is ignored",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    prepareSelectedTrack(mpc, TrackIndex(0), sequencer::BusType::DRUM1);
    mpc.getSequencer()->setRecordingModeMulti(true);

    const auto multiRecordingSetupScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();
    multiRecordingSetupScreen->getMrsLines()[0]->setTrack(-1);
    multiRecordingSetupScreen->getMrsLines()[1]->setTrack(0);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(makeNoteOnEvent(1, 60, 100));
    drain(mpc);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 1);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(makeControlChangeEvent(0, 123, 0));
    drain(mpc);

    REQUIRE(mpc.getPerformanceManager()
                .lock()
                ->getSnapshot()
                .getTotalNoteOnCount() == 1);
}

TEST_CASE("Incoming program change on an OFF multi-record input line is ignored",
          "[midi-aftertouch-input]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    mpc.getSequencer()->setRecordingModeMulti(true);
    const auto multiRecordingSetupScreen =
        mpc.screens->get<mpc::lcdgui::ScreenId::MultiRecordingSetupScreen>();
    multiRecordingSetupScreen->getMrsLines()[0]->setTrack(-1);

    const auto drumBus = mpc.getSequencer()->getDrumBus(sequencer::BusType::DRUM1);
    drumBus->setReceivePgmChange(true);
    drumBus->setProgramIndex(ProgramIndex(0));
    drain(mpc);

    const auto matchingProgramIndex =
        createUsedProgramWithMidiProgramChange(mpc, 10);

    mpc.clientEventController->getClientMidiEventController()
        ->handleClientMidiEvent(makeProgramChangeEvent(0, 9));
    drain(mpc);

    REQUIRE(matchingProgramIndex != ProgramIndex(0));
    REQUIRE(drumBus->getProgramIndex() == ProgramIndex(0));
}
