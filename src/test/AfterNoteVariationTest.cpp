#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "TestMpc.hpp"
#include "sequencer/EventRef.hpp"
#include "sequencer/Clock.hpp"
#include "engine/audio/server/NonRealTimeAudioServer.hpp"
#include "lcdgui/screens/SecondSeqScreen.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "performance/PerformanceManager.hpp"
#include "audiomidi/EventHandler.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/EngineHost.hpp"
#include "engine/Voice.hpp"
#include "file/kaitai/MidIo.hpp"
#include "hardware/Hardware.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/PgmSlider.hpp"
#include "sampler/SoundGenerationMode.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Transport.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc;
using namespace mpc::sequencer;

namespace
{
    struct AfterFixture
    {
        Mpc mpc;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<Sequence> sequence;
        std::shared_ptr<Track> track;

        AfterFixture()
        {
            TestMpc::initializeTestMpcWithoutIoServices(mpc);
            sequencer = mpc.getSequencer();
            sequence = sequencer->getSelectedSequence();
            sequence->init(1);
            drain();
            track = sequence->getTrack(0);
            track->setBusType(BusType::DRUM1);
            auto sampler = mpc.getSampler();
            for (int i = 0; i < 3; ++i)
            {
                auto sound = sampler->addSound();
                sound->setMono(true);
                sound->setSampleData(
                    std::make_shared<std::vector<float>>(10000, 1.f));
                sound->setEnd(10000);
            }
            auto program = sampler->getProgram(0);
            for (int i = 0; i < 3; ++i)
            {
                program->getNoteParameters(35 + i)->setSoundIndex(i);
            }
            auto parameters = program->getNoteParameters(35);
            parameters->setSoundGenMode(
                sampler::toRaw(sampler::SoundGenerationMode::DecaySwitch));
            parameters->setVeloRangeLower(44);
            parameters->setVeloRangeUpper(88);
            parameters->setOptionalNoteA(DrumNoteNumber(36));
            parameters->setOptionalNoteB(DrumNoteNumber(37));
            auto slider = program->getSlider();
            slider->setAssignNote(DrumNoteNumber(35));
            slider->setParameter(1);
            slider->setDecayLowRange(0);
            slider->setDecayHighRange(100);
            slider->setAttackLowRange(0);
            slider->setAttackHighRange(100);
            slider->setFilterLowRange(0);
            slider->setFilterHighRange(50);
            sequencer->getDrumBus(DrumBusIndex(0))
                ->setProgramIndex(ProgramIndex(0));
            EventData note;
            note.type = EventType::NoteOn;
            note.tick = 0;
            note.noteNumber = NoteNumber(35);
            note.duration = Duration(24);
            note.velocity = Velocity(100);
            note.noteVariationType = NoteVariationTypeDecay;
            note.noteVariationValue = NoteVariationValue(0);
            track->acquireAndInsertEvent(note);
            sequencer->getTransport()->setCountEnabled(false);
            mpc.clientEventController->setAfterEnabled(true);
            mpc.getHardware()->getSlider()->setValue(64);
            drain();
        }

        void drain()
        {
            for (int i = 0; i < 3; ++i)
            {
                mpc.getEngineHost()->drainAudioThreadStateQueues();
            }
        }

        void start(bool overdub)
        {
            if (overdub)
            {
                sequencer->getTransport()->overdubFromStart();
            }
            else
            {
                sequencer->getTransport()->play(true);
            }
            drain();
            REQUIRE(sequencer->getTransport()->isPlaying());
        }

        EventData stored() const
        {
            return track->getEventStates().at(0);
        }

        void play(bool allowMutation = true)
        {
            for (const auto &voice : mpc.getEngineHost()->getVoices())
            {
                voice->finish();
            }
            REQUIRE(track->playEventAtIndex(EventIndex(0), allowMutation));
            drain();
        }

        void requireVoice(int note)
        {
            std::vector<int> notes;
            for (const auto &voice : mpc.getEngineHost()->getVoices())
            {
                if (!voice->isFinished())
                {
                    notes.push_back(voice->getNote());
                }
            }
            REQUIRE(notes == std::vector<int>{note});
        }
    };
} // namespace

TEST_CASE("AFTER previews playback and overdub stores the heard variation",
          "[after]")
{
    const bool overdub = GENERATE(false, true);
    AfterFixture f;
    f.start(overdub);
    f.play();
    f.requireVoice(36);
    REQUIRE(f.stored().noteVariationValue == (overdub ? 50 : 0));

    // A later pass samples the current fader position again.
    f.mpc.getHardware()->getSlider()->setValue(127);
    f.play();
    f.requireVoice(37);
    REQUIRE(f.stored().noteVariationValue == (overdub ? 99 : 0));

    f.sequencer->getTransport()->stop();
    f.drain();
    f.mpc.clientEventController->setAfterEnabled(false);
    f.mpc.getHardware()->getSlider()->setValue(0);
    f.start(false);
    f.play();
    f.requireVoice(overdub ? 37 : 35);
}

TEST_CASE("AFTER records all four parameters without changing other note data",
          "[after]")
{
    const int parameter = GENERATE(0, 1, 2, 3);
    CAPTURE(parameter);
    AfterFixture f;
    f.mpc.getSampler()->getProgram(0)->getSlider()->setParameter(parameter);
    auto expected = f.stored();
    f.start(true);
    f.play();
    expected.noteVariationType = NoteVariationType(parameter);
    expected.noteVariationValue = NoteVariationValue(parameter == 0   ? 60
                                                     : parameter == 3 ? 25
                                                                      : 50);
    REQUIRE(f.stored() == expected);

    f.sequencer->getTransport()->stop();
    f.drain();
    const auto bytes = file::kaitai::MidIo::saveBytes(f.sequence);
    const auto loaded = file::kaitai::MidIo::loadBytes(f.mpc, bytes, "AFTER");
    REQUIRE(loaded.has_value());
    f.drain();
    const auto events = loaded.value()->getTrack(0)->getEventStates();
    REQUIRE(events.size() == 1);
    REQUIRE(events[0].noteVariationType == expected.noteVariationType);
    REQUIRE(events[0].noteVariationValue == expected.noteVariationValue);
}

TEST_CASE("AFTER respects track recording scope and secondary playback",
          "[after]")
{
    const bool multi = GENERATE(false, true);
    const bool allowMutation = GENERATE(false, true);
    AfterFixture f;
    auto secondTrack = f.sequence->getTrack(1);
    secondTrack->setBusType(BusType::DRUM1);
    secondTrack->acquireAndInsertEvent(f.stored());
    f.sequencer->setRecordingModeMulti(multi);
    f.drain();
    f.start(true);
    f.track = secondTrack;
    f.play(allowMutation);
    f.requireVoice(36);
    REQUIRE(f.stored().noteVariationValue == (multi && allowMutation ? 50 : 0));
}

TEST_CASE("AFTER does not edit disabled, unmatched, or inaudible notes",
          "[after]")
{
    AfterFixture f;
    SECTION("AFTER disabled")
    {
        f.mpc.clientEventController->setAfterEnabled(false);
    }
    SECTION("unassigned note")
    {
        f.mpc.getSampler()->getProgram(0)->getSlider()->setAssignNote(
            DrumNoteNumber(36));
    }
    SECTION("MIDI track")
    {
        f.track->setBusType(BusType::MIDI);
    }
    SECTION("muted track")
    {
        f.track->setOn(false);
    }
    SECTION("another track soloed")
    {
        f.sequencer->setSelectedTrackIndex(TrackIndex(1));
        f.sequencer->setSoloEnabled(true);
        f.sequencer->setRecordingModeMulti(true);
    }
    f.drain();
    const auto before = f.stored();
    f.start(true);
    f.play();
    REQUIRE(f.stored() == before);
}

TEST_CASE("AFTER recording requires running overdub outside count-in",
          "[after]")
{
    AfterFixture f;
    SECTION("stopped with overdub armed")
    {
        f.sequencer->getTransport()->setOverdubbing(true);
    }
    SECTION("count-in")
    {
        f.start(true);
        f.sequencer->getTransport()->setCountingIn(true);
    }
    f.drain();
    f.play();
    REQUIRE(f.stored().noteVariationValue == 0);
}

TEST_CASE("AFTER honors punch boundaries", "[after]")
{
    const int mode = GENERATE(0, 1, 2);
    const int tick = GENERATE(95, 96, 191, 192);
    AfterFixture f;
    auto transport = f.sequencer->getTransport();
    transport->setAutoPunchMode(mode);
    transport->setPunchInTime(96);
    transport->setPunchOutTime(192);
    transport->setPunchEnabled(true);
    f.start(true);
    transport->setPosition(Sequencer::ticksToQuarterNotes(tick));
    f.drain();
    f.play();
    f.requireVoice(36);
    const bool writes = (mode == 0 || tick < 192) && (mode == 1 || tick >= 96);
    REQUIRE(f.stored().noteVariationValue == (writes ? 50 : 0));
}

TEST_CASE("AFTER edits support undo and redo from stopped and running playback",
          "[after]")
{
    const bool alreadyPlaying = GENERATE(false, true);
    AfterFixture f;
    if (alreadyPlaying)
    {
        f.start(false);
        f.sequencer->getTransport()->setOverdubbing(true);
        f.drain();
    }
    else
    {
        f.start(true);
    }
    f.play();
    REQUIRE(f.stored().noteVariationValue == 50);
    f.sequencer->getTransport()->stop();
    f.drain();
    f.sequencer->undoSeq();
    f.drain();
    REQUIRE(f.stored().noteVariationValue == 0);
    f.sequencer->undoSeq();
    f.drain();
    REQUIRE(f.stored().noteVariationValue == 50);
}

TEST_CASE("Direct event audition uses stored variation even with AFTER enabled",
          "[after]")
{
    AfterFixture f;
    f.start(true);
    f.mpc.getEventHandler()->handleFinalizedEvent(f.stored(), TrackIndex(0),
                                                  100, BusType::DRUM1, 0);
    f.requireVoice(35);
    REQUIRE(f.stored().noteVariationValue == 0);
}

TEST_CASE("AFTER does not rescue notes deleted by replace recording", "[after]")
{
    AfterFixture f;
    f.sequencer->getTransport()->recFromStart();
    f.drain();
    REQUIRE_FALSE(f.track->playEventAtIndex(EventIndex(0), true));
    f.drain();
    REQUIRE(f.track->getEventStates().empty());
}

TEST_CASE("AFTER follows the program of the playing drum bus", "[after]")
{
    AfterFixture f;
    auto otherProgram = f.mpc.getSampler()->addProgram(1).lock();
    otherProgram->getSlider()->setAssignNote(DrumNoteNumber(36));
    f.sequencer->getDrumBus(DrumBusIndex(1))->setProgramIndex(ProgramIndex(1));
    f.track->setBusType(BusType::DRUM2);
    f.drain();
    f.start(true);
    f.play();
    REQUIRE(f.stored().noteVariationValue == 0);
    otherProgram->getSlider()->setAssignNote(DrumNoteNumber(35));
    otherProgram->getSlider()->setParameter(2);
    otherProgram->getSlider()->setAttackLowRange(20);
    otherProgram->getSlider()->setAttackHighRange(80);
    f.play();
    REQUIRE(f.stored().noteVariationType == NoteVariationTypeAttack);
    REQUIRE(f.stored().noteVariationValue == 50);
}

TEST_CASE("Variation edits defer to the track lock and preserve newer fields",
          "[after]")
{
    AfterFixture f;
    auto manager = f.sequencer->getStateManager();
    auto event = f.track->getEvent(0);
    auto expected = f.stored();
    expected.velocity = Velocity(72);
    expected.duration = Duration(48);
    auto &lock =
        manager
            ->trackLocks[f.sequence->getSequenceIndex()][f.track->getIndex()];
    lock.acquire();
    manager->applyMessageImmediate(UpdateNoteVariation{
        f.sequence->getSequenceIndex(), f.track->getIndex(), event->handle,
        NoteVariationTypeAttack, NoteVariationValue(50)});
    const auto valueWhileLocked = event->handle->noteVariationValue;
    // Simulate the edit for which the other thread holds this lock.
    event->handle->velocity = expected.velocity;
    event->handle->duration = expected.duration;
    lock.release();
    REQUIRE(valueWhileLocked == 0);
    f.drain();
    expected.noteVariationType = NoteVariationTypeAttack;
    expected.noteVariationValue = NoteVariationValue(50);
    REQUIRE(f.stored() == expected);
}

TEST_CASE("Variation edits ignore removed and unfinalized events", "[after]")
{
    AfterFixture f;
    auto manager = f.sequencer->getStateManager();
    auto event = f.track->getEvent(0);
    SECTION("removed event")
    {
        manager->enqueue(RemoveEvent{event->handle});
        manager->enqueue(UpdateNoteVariation{
            f.sequence->getSequenceIndex(), f.track->getIndex(), event->handle,
            NoteVariationTypeAttack, NoteVariationValue(50)});
        f.drain();
        REQUIRE(f.track->getEventStates().empty());
    }
    SECTION("unfinalized event")
    {
        auto recordingNote = f.stored();
        recordingNote.beingRecorded = true;
        manager->enqueue(UpdateEvent{event->handle, recordingNote});
        manager->enqueue(UpdateNoteVariation{
            f.sequence->getSequenceIndex(), f.track->getIndex(), event->handle,
            NoteVariationTypeAttack, NoteVariationValue(50)});
        f.drain();
        REQUIRE(f.stored() == recordingNote);
        REQUIRE_FALSE(f.mpc.getEventHandler()->getAfterNoteVariation(
            f.stored(), BusType::DRUM1));
    }
}

TEST_CASE(
    "Switching from record to overdub retains the original undo checkpoint",
    "[after]")
{
    const bool explicitSwitch = GENERATE(false, true);
    AfterFixture f;
    auto transport = f.sequencer->getTransport();
    transport->recFromStart();
    f.drain();
    REQUIRE_FALSE(f.track->playEventAtIndex(EventIndex(0), true));
    if (explicitSwitch)
    {
        transport->switchRecordToOverdub();
    }
    else
    {
        transport->setOverdubbing(true);
        transport->setRecording(false);
    }
    f.drain();
    transport->stop();
    f.drain();
    f.sequencer->undoSeq();
    f.drain();
    REQUIRE(f.track->getEventStates().size() == 1);
    REQUIRE(f.stored().noteVariationValue == 0);
}

TEST_CASE("AFTER leaves erase behavior intact", "[after]")
{
    AfterFixture f;
    f.start(true);
    using client::event::ClientHardwareEvent;
    f.mpc.clientEventController->clientHardwareEventController
        ->handleClientHardwareEvent(ClientHardwareEvent{
            ClientHardwareEvent::Source::HostInputGesture,
            ClientHardwareEvent::Type::MpcButtonPress, std::nullopt,
            hardware::ComponentId::ERASE, 1.f, std::nullopt, std::nullopt});
    auto program = f.mpc.getSampler()->getProgram(0);
    const auto pad = program->getPadIndexFromNote(DrumNoteNumber(35));
    f.mpc.getPerformanceManager().lock()->registerProgramPadPress(
        performance::PerformanceEventSource::VirtualMpcHardware, std::nullopt,
        lcdgui::ScreenId::SequencerScreen, TrackIndex(0), BusType::DRUM1, pad,
        Velocity(100), ProgramIndex(0), PhysicalPadIndex(0));
    f.drain();
    REQUIRE(f.track->shouldRemovePlayIndexEventDueToErasePressed());
    REQUIRE_FALSE(f.track->playEventAtIndex(EventIndex(0), true));
    f.drain();
    REQUIRE(f.track->getEventStates().empty());
}

TEST_CASE(
    "Playback engine records AFTER across loops but leaves second sequence "
    "intact",
    "[after]")
{
    AfterFixture f;
    auto secondary = f.sequencer->getSequence(1);
    secondary->init(0);
    f.drain();
    auto secondaryTrack = secondary->getTrack(0);
    secondaryTrack->setBusType(BusType::DRUM1);
    secondaryTrack->acquireAndInsertEvent(f.stored());
    f.mpc.getLayeredScreen()->openScreenById(lcdgui::ScreenId::SecondSeqScreen);
    auto screen = f.mpc.screens->get<lcdgui::ScreenId::SecondSeqScreen>();
    screen->turnWheel(1 - screen->getSq());
    f.sequencer->setSecondSequenceEnabled(true);
    f.drain();
    f.start(true);

    const auto engine = f.mpc.getEngineHost();
    const auto server = engine->getAudioServer();
    server->resizeBuffers(512);
    server->setSampleRate(44100);
    server->start();
    const auto process = [&]
    {
        engine->prepareProcessBlock(512);
        f.mpc.getClock()->processBufferInternal(
            f.sequencer->getTransport()->getTempo(), 44100, 512,
            f.sequencer->getTransport()->getPlayStartPositionQuarterNotes());
        server->work(nullptr, nullptr, 512, {}, {}, {}, {});
    };
    process();
    f.drain();
    REQUIRE(f.stored().noteVariationValue == 50);
    REQUIRE(secondaryTrack->getEventStates().at(0).noteVariationValue == 0);
    std::vector<int> playedNotes;
    for (const auto &voice : engine->getVoices())
    {
        if (!voice->isFinished())
        {
            playedNotes.push_back(voice->getNote());
        }
    }
    REQUIRE(playedNotes == std::vector<int>{36, 36});
    f.mpc.getHardware()->getSlider()->setValue(127);
    for (int i = 0; i < 800 && f.stored().noteVariationValue != 99; ++i)
    {
        process();
    }
    REQUIRE(f.stored().noteVariationValue == 99);
    REQUIRE(secondaryTrack->getEventStates().at(0).noteVariationValue == 0);
}
