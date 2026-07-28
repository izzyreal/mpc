#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "engine/EngineHost.hpp"
#include "engine/Voice.hpp"
#include "hardware/ComponentId.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

#include <algorithm>
#include <memory>
#include <vector>

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::lcdgui;

namespace
{
    ClientHardwareEvent makePadEvent(const ClientHardwareEvent::Type type)
    {
        ClientHardwareEvent e;
        e.source = ClientHardwareEvent::Source::HostInputGesture;
        e.type = type;
        e.componentId = hardware::PAD_1_OR_AB;
        e.index = 0;

        if (type == ClientHardwareEvent::Type::PadRelease)
        {
            e.value = 0.f;
        }
        else
        {
            e.value = 1.f;
        }

        return e;
    }

    void prepareMpcWithSound(Mpc &mpc)
    {
        TestMpc::initializeTestMpc(mpc);

        auto sound = mpc.getSampler()->addSound();
        sound->setName("test");
        sound->setSampleData(std::make_shared<std::vector<float>>(1024, 1.f));
        sound->setMono(true);
        sound->setLevel(100);
        mpc.getSampler()->setSoundIndex(0);

        auto program = mpc.getSampler()->getProgram(0);
        program->setUsed();
        program->initPadAssign();
        mpc.getPerformanceManager().lock()->drainQueue();
        const auto padNote = program->getNoteFromPad(ProgramPadIndex(0));
        program->getNoteParameters(padNote)->setSoundIndex(0);
        mpc.getSequencer()
            ->getDrumBus(DrumBusIndex(0))
            ->setProgramIndex(ProgramIndex(0));
        mpc.getSequencer()->getSelectedSequence()->init(1);
        mpc.getSequencer()->getStateManager()->drainQueue();
        mpc.getPerformanceManager().lock()->drainQueue();
    }

    int getActiveNoteCount(Mpc &mpc)
    {
        auto performanceManager = mpc.getPerformanceManager().lock();
        performanceManager->drainQueue();
        performanceManager->drainQueue();
        return performanceManager->getSnapshot().getTotalNoteOnCount();
    }

    bool hasActiveVoiceForNote(Mpc &mpc, const NoteNumber note)
    {
        const auto &voices = mpc.getEngineHost()->getVoices();
        return std::any_of(voices.begin(), voices.end(),
                           [note](const auto &voice)
                           {
                               return !voice->isFinished() &&
                                      voice->getNote() == note;
                           });
    }

    std::vector<ClientMidiEvent>
    getMidiOutputEvents(Mpc &mpc,
                        const ClientMidiEvent::MessageType messageType)
    {
        std::vector<ClientMidiEvent> result;
        std::vector<ClientMidiEvent> buffer(16);

        while (const auto count = mpc.getMidiOutput()->dequeue(buffer))
        {
            for (int i = 0; i < count; ++i)
            {
                if (buffer[i].getMessageType() == messageType)
                {
                    result.push_back(buffer[i]);
                }
            }
        }

        return result;
    }

    int getMidiOutputEventCount(Mpc &mpc,
                                const ClientMidiEvent::MessageType messageType)
    {
        return static_cast<int>(getMidiOutputEvents(mpc, messageType).size());
    }

    void sendPadEvent(Mpc &mpc, const ClientHardwareEvent::Type type)
    {
        mpc.clientEventController->clientHardwareEventController
            ->handleClientHardwareEvent(makePadEvent(type));
    }
} // namespace

TEST_CASE("Shift+5 sampler screens suppress physical pad note generation",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::TrimScreen);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    sendPadEvent(mpc, ClientHardwareEvent::Type::PadAftertouch);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
}

TEST_CASE("Shift+5 sampler child screens suppress physical pad note generation",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::TrimScreen);
    layeredScreen->openScreenById(ScreenId::StartFineScreen);
    REQUIRE(layeredScreen->isCurrentScreenOrChildOf(ScreenId::TrimScreen));

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
}

TEST_CASE("Sequencer screen still generates notes for physical pad presses",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    mpc.getSequencer()->getSelectedTrack()->setDeviceIndex(1, false);
    mpc.getSequencer()->getStateManager()->drainQueue();

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    REQUIRE(getActiveNoteCount(mpc) == 1);
    REQUIRE(getMidiOutputEventCount(mpc, ClientMidiEvent::NOTE_ON) == 1);
}

TEST_CASE(
    "Unused sequence keeps drum generation but suppresses pad MIDI output",
    "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);

    auto sequencer = mpc.getSequencer();
    sequencer->setSelectedSequenceIndex(SequenceIndex(5), true);
    sequencer->getStateManager()->drainQueue();
    REQUIRE_FALSE(sequencer->getSelectedSequence()->isUsed());
    sequencer->getSelectedTrack()->setDeviceIndex(1, false);
    sequencer->getStateManager()->drainQueue();

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    sendPadEvent(mpc, ClientHardwareEvent::Type::PadAftertouch);
    REQUIRE(getActiveNoteCount(mpc) == 1);
    REQUIRE(hasActiveVoiceForNote(
        mpc,
        mpc.getSampler()->getProgram(0)->getNoteFromPad(ProgramPadIndex(0))));
    REQUIRE(getMidiOutputEventCount(mpc, ClientMidiEvent::NOTE_ON) == 0);

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);
    REQUIRE(getMidiOutputEventCount(mpc, ClientMidiEvent::NOTE_OFF) == 0);
}

TEST_CASE("Held pad keeps its captured MIDI output route",
          "[pad-note-suppression]")
{
    Mpc mpc;
    prepareMpcWithSound(mpc);
    mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);

    const auto sequencer = mpc.getSequencer();
    const auto track = sequencer->getSelectedTrack();
    track->setDeviceIndex(1, false);
    sequencer->getStateManager()->drainQueue();

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadPress);
    REQUIRE(getActiveNoteCount(mpc) == 1);

    const auto noteOnEvents =
        getMidiOutputEvents(mpc, ClientMidiEvent::NOTE_ON);
    REQUIRE(noteOnEvents.size() == 1);
    REQUIRE(noteOnEvents.front().getChannel() == 0);

    track->setDeviceIndex(2, false);
    sequencer->getStateManager()->drainQueue();

    sendPadEvent(mpc, ClientHardwareEvent::Type::PadRelease);
    REQUIRE(getActiveNoteCount(mpc) == 0);

    const auto noteOffEvents =
        getMidiOutputEvents(mpc, ClientMidiEvent::NOTE_OFF);
    REQUIRE(noteOffEvents.size() == 1);
    REQUIRE(noteOffEvents.front().getChannel() == 0);
}
