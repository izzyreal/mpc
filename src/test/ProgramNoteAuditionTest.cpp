#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "client/event/ClientMidiEvent.hpp"
#include "command/ReleaseFunctionCommand.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/Voice.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "performance/PerformanceManager.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "sampler/VoiceOverlapMode.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Sequencer.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

using namespace mpc;
using namespace mpc::client::event;
using namespace mpc::lcdgui;

namespace
{
    void drainAudioThreadQueues(Mpc &mpc)
    {
        for (int i = 0; i < 3; ++i)
        {
            mpc.getEngineHost()->drainAudioThreadStateQueues();
        }
    }

    DrumNoteNumber prepareMpcWithProgramSound(Mpc &mpc)
    {
        TestMpc::initializeTestMpc(mpc);

        const auto sound = mpc.getSampler()->addSound();
        REQUIRE(sound);
        sound->setName("audition");
        sound->setSampleData(std::make_shared<std::vector<float>>(200000, 1.f));
        sound->setMono(true);
        sound->setLevel(100);

        const auto program = mpc.getSampler()->getProgram(0);
        REQUIRE(program);
        program->setUsed();
        program->initPadAssign();

        const auto note = mpc.clientEventController->getSelectedNote();
        const auto noteParameters = program->getNoteParameters(note);
        noteParameters->setSoundIndex(0);
        noteParameters->setVoiceOverlapMode(
            sampler::VoiceOverlapMode::NOTE_OFF);

        mpc.getSequencer()
            ->getDrumBus(DrumBusIndex(0))
            ->setProgramIndex(ProgramIndex(0));
        drainAudioThreadQueues(mpc);

        return note;
    }

    std::shared_ptr<engine::Voice> findActiveVoice(Mpc &mpc,
                                                   const DrumNoteNumber note)
    {
        const auto &voices = mpc.getEngineHost()->getVoices();
        const auto result = std::find_if(voices.begin(), voices.end(),
                                         [note](const auto &voice)
                                         {
                                             return !voice->isFinished() &&
                                                    voice->getNote() == note;
                                         });

        REQUIRE(result != voices.end());
        return *result;
    }

    int getMidiOutputEventCount(Mpc &mpc,
                                const ClientMidiEvent::MessageType messageType)
    {
        int result = 0;
        std::vector<ClientMidiEvent> buffer(16);

        while (const auto count = mpc.getMidiOutput()->dequeue(buffer))
        {
            for (int i = 0; i < count; ++i)
            {
                if (buffer[i].getMessageType() == messageType)
                {
                    ++result;
                }
            }
        }

        return result;
    }

    void verifyAudition(const ScreenId screenId, const int functionIndex,
                        const std::optional<int> velocityWheelDelta,
                        const Velocity expectedVelocity)
    {
        Mpc mpc;
        const auto note = prepareMpcWithProgramSound(mpc);
        const auto layeredScreen = mpc.getLayeredScreen();
        layeredScreen->openScreenById(screenId);

        if (velocityWheelDelta)
        {
            REQUIRE(layeredScreen->setFocus("velo"));
            mpc.getScreen()->turnWheel(*velocityWheelDelta);
        }

        mpc.getScreen()->function(functionIndex);
        drainAudioThreadQueues(mpc);

        const auto voice = findActiveVoice(mpc, note);
        REQUIRE(voice->getVelocity() == expectedVelocity);
        REQUIRE(voice->getNoteEventId() != 0);
        REQUIRE(mpc.getPerformanceManager()
                    .lock()
                    ->getSnapshot()
                    .getTotalNoteOnCount() == 0);
        REQUIRE(getMidiOutputEventCount(mpc, ClientMidiEvent::NOTE_ON) == 0);

        command::ReleaseFunctionCommand(mpc, functionIndex).execute();
        drainAudioThreadQueues(mpc);

        REQUIRE(voice->isDecaying());
        REQUIRE(getMidiOutputEventCount(mpc, ClientMidiEvent::NOTE_OFF) == 0);
    }
} // namespace

TEST_CASE("Program editing PLAY auditions the selected program note",
          "[program-note-audition]")
{
    verifyAudition(ScreenId::PgmAssignScreen, 5, std::nullopt, MaxVelocity);
    verifyAudition(ScreenId::PgmParamsScreen, 5, std::nullopt, MaxVelocity);
    verifyAudition(ScreenId::VelocityModulationScreen, 4, -95, Velocity(32));
    verifyAudition(ScreenId::VeloEnvFilterScreen, 4, -95, Velocity(32));
    verifyAudition(ScreenId::VeloPitchScreen, 4, -95, Velocity(32));
    verifyAudition(ScreenId::MuteAssignScreen, 4, std::nullopt, MaxVelocity);
}

TEST_CASE("DRUM Pgm field opens the Program window", "[program-note-audition]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    const auto layeredScreen = mpc.getLayeredScreen();
    layeredScreen->openScreenById(ScreenId::DrumScreen);
    REQUIRE(layeredScreen->setFocus("pgm"));

    mpc.getScreen()->openWindow();

    REQUIRE(layeredScreen->isCurrentScreen({ScreenId::ProgramScreen}));
}

TEST_CASE("Releasing PLAY only releases its own auditioned note",
          "[program-note-audition]")
{
    Mpc mpc;
    const auto note = prepareMpcWithProgramSound(mpc);

    sequencer::EventData otherNoteOn;
    otherNoteOn.type = sequencer::EventType::NoteOn;
    otherNoteOn.noteNumber = note;
    otherNoteOn.velocity = MaxVelocity;
    mpc.getEventHandler()->handleUnfinalizedNoteOn(otherNoteOn, std::nullopt,
                                                   sequencer::BusType::DRUM1);

    mpc.getLayeredScreen()->openScreenById(ScreenId::PgmAssignScreen);
    mpc.getScreen()->function(5);
    drainAudioThreadQueues(mpc);

    std::shared_ptr<engine::Voice> otherVoice;
    std::shared_ptr<engine::Voice> auditionVoice;
    for (const auto &voice : mpc.getEngineHost()->getVoices())
    {
        if (voice->isFinished() || voice->getNote() != note)
        {
            continue;
        }

        if (voice->getNoteEventId() == 0)
        {
            otherVoice = voice;
        }
        else
        {
            auditionVoice = voice;
        }
    }

    REQUIRE(otherVoice);
    REQUIRE(auditionVoice);

    command::ReleaseFunctionCommand(mpc, 5).execute();
    drainAudioThreadQueues(mpc);

    REQUIRE_FALSE(otherVoice->isDecaying());
    REQUIRE(auditionVoice->isDecaying());
}
