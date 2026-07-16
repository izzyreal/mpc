#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "Mpc.hpp"
#include "IntTypes.hpp"
#include "sequencer/BusType.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/EventData.hpp"
#include "sequencer/Sequencer.hpp"
#include "audiomidi/EventHandler.hpp"
#include "engine/EngineHost.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MixerControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"
#include "engine/FaderControl.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"
#include "sampler/NoteParameters.hpp"
#include "sampler/SoundGenerationMode.hpp"
#include "engine/Voice.hpp"
#include "engine/IndivFxMixer.hpp"
#include "Util.hpp"

#include <optional>
#include <memory>
#include <string>

using namespace mpc;
using namespace mpc::sequencer;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::control;

namespace
{
    static constexpr int kBufferSize = 64;
    static constexpr DrumNoteNumber kTestNote{35};
    static constexpr DrumNoteNumber kSwitchNoteA{36};
    static constexpr DrumNoteNumber kSwitchNoteB{37};
    static constexpr int kIndivLevel = 73;
    static constexpr float kPanLeft = 0.f;
    static constexpr float kPanCenter = 0.5f;

    std::shared_ptr<CompoundControl> getAuxControl(
        const std::shared_ptr<AudioMixer> &mixer, const std::string &stripName,
        const std::string &auxName)
    {
        const auto strip =
            mixer->getMixerControls()->getStripControls(stripName);
        REQUIRE(strip != nullptr);

        const auto auxControl =
            std::dynamic_pointer_cast<CompoundControl>(strip->find(auxName));
        REQUIRE(auxControl != nullptr);

        return auxControl;
    }

    std::shared_ptr<FaderControl> getAuxLevelControl(
        const std::shared_ptr<AudioMixer> &mixer, const std::string &stripName,
        const std::string &auxName)
    {
        const auto auxControl = getAuxControl(mixer, stripName, auxName);
        const auto levelControl =
            std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));
        REQUIRE(levelControl != nullptr);
        return levelControl;
    }

    std::shared_ptr<PanControl> getAuxPanControl(
        const std::shared_ptr<AudioMixer> &mixer, const std::string &stripName,
        const std::string &auxName)
    {
        const auto auxControl = getAuxControl(mixer, stripName, auxName);
        const auto panControl =
            std::dynamic_pointer_cast<PanControl>(auxControl->find("Pan"));
        REQUIRE(panControl != nullptr);
        return panControl;
    }

    void configureProgramNote(Mpc &mpc, const bool mono,
                              const DrumMixerIndividualOutput output)
    {
        const auto sampler = mpc.getSampler();
        const auto sound = sampler->addSound();
        REQUIRE(sound != nullptr);

        sound->setMono(mono);
        sound->insertFrame(mono ? std::vector<float>{1.f}
                                : std::vector<float>{1.f, 1.f},
                           0);
        sound->setStart(0);
        sound->setEnd(1);

        const auto program = sampler->getProgram(0);
        REQUIRE(program != nullptr);

        const auto np = program->getNoteParameters(kTestNote);
        REQUIRE(np != nullptr);

        np->setSoundIndex(0);
        np->getIndivFxMixer()->setOutput(output);
        np->getIndivFxMixer()->setVolumeIndividualOut(
            DrumMixerLevel(kIndivLevel));
    }

    void triggerUnfinalizedDrumNoteOn(Mpc &mpc)
    {
        const auto mixerSetupScreen =
            mpc.screens->get<lcdgui::ScreenId::MixerSetupScreen>();
        mixerSetupScreen->setStereoMixSourceDrum(false);
        mixerSetupScreen->setIndivFxSourceDrum(false);

        mpc.getSequencer()->getDrumBus(DrumBusIndex(0))->setProgramIndex(
            ProgramIndex(0));

        mpc.getEngineHost()->prepareProcessBlock(kBufferSize);

        EventData event;
        event.type = EventType::NoteOn;
        event.noteNumber = NoteNumber(kTestNote.get());
        event.velocity = Velocity(100);
        event.duration = NoDuration;

        mpc.getSequencer()->getEventHandler()->handleUnfinalizedNoteOn(
            event, std::nullopt, BusType::DRUM1);
    }

    void configureSwitchingProgramNote(
        Mpc &mpc, const mpc::sampler::SoundGenerationMode mode)
    {
        const auto sampler = mpc.getSampler();
        for (int i = 0; i < 3; ++i)
        {
            const auto sound = sampler->addSound();
            REQUIRE(sound != nullptr);
            sound->insertFrame(std::vector<float>{1.f}, 0);
            sound->setStart(0);
            sound->setEnd(1);
        }

        const auto program = sampler->getProgram(0);
        REQUIRE(program != nullptr);

        const auto note35 = program->getNoteParameters(kTestNote);
        const auto note36 = program->getNoteParameters(kSwitchNoteA);
        const auto note37 = program->getNoteParameters(kSwitchNoteB);
        REQUIRE(note35 != nullptr);
        REQUIRE(note36 != nullptr);
        REQUIRE(note37 != nullptr);

        note35->setSoundIndex(0);
        note35->setSoundGenMode(mpc::sampler::toRaw(mode));
        note35->setVeloRangeLower(44);
        note35->setVeloRangeUpper(88);
        note35->setOptionalNoteA(kSwitchNoteA);
        note35->setOptionalNoteB(kSwitchNoteB);

        note36->setSoundIndex(1);
        note37->setSoundIndex(2);
    }

    std::vector<int> getActiveVoiceNotes(Mpc &mpc)
    {
        std::vector<int> result;
        for (const auto &voice : mpc.getEngineHost()->getVoices())
        {
            if (!voice->isFinished())
            {
                result.push_back(voice->getNote());
            }
        }
        return result;
    }

    void triggerUnfinalizedDrumNoteOn(
        Mpc &mpc, const int velocity, const mpc::NoteVariationType variationType,
        const int variationValue)
    {
        const auto mixerSetupScreen =
            mpc.screens->get<lcdgui::ScreenId::MixerSetupScreen>();
        mixerSetupScreen->setStereoMixSourceDrum(false);
        mixerSetupScreen->setIndivFxSourceDrum(false);

        mpc.getSequencer()->getDrumBus(DrumBusIndex(0))->setProgramIndex(
            ProgramIndex(0));

        mpc.getEngineHost()->prepareProcessBlock(kBufferSize);

        EventData event;
        event.type = EventType::NoteOn;
        event.noteNumber = NoteNumber(kTestNote.get());
        event.velocity = Velocity(velocity);
        event.noteVariationType = variationType;
        event.noteVariationValue = NoteVariationValue(variationValue);
        event.duration = NoDuration;

        mpc.getSequencer()->getEventHandler()->handleUnfinalizedNoteOn(
            event, std::nullopt, BusType::DRUM1);
    }
} // namespace

TEST_CASE("Mono sound routed to individual out uses a single strip AUX send",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, true, DrumMixerIndividualOutput(3));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#3")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#4")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() ==
            static_cast<float>(kIndivLevel));
    REQUIRE(getAuxPanControl(mixer, "1", "AUX#2")->getValue() == kPanLeft);
}

TEST_CASE("Mono sound routed to main keeps both channels and no individual AUX send",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, true, DrumMixerIndividualOutput(0));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#3")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#4")->getValue() == 0.f);
}

TEST_CASE("Stereo sound to indiv out keeps both chans on a single strip AUX",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, false, DrumMixerIndividualOutput(3));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#3")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#4")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() ==
            static_cast<float>(kIndivLevel));
    REQUIRE(getAuxPanControl(mixer, "1", "AUX#2")->getValue() == kPanCenter);
}

TEST_CASE("Stereo sound routed to main keeps both channels and no individual AUX send",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, false, DrumMixerIndividualOutput(0));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#3")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#4")->getValue() == 0.f);
}

TEST_CASE("Velocity switch mode keeps the original note at or below lower threshold",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::VelocitySwitch);
    triggerUnfinalizedDrumNoteOn(mpc, 44, mpc::NoteVariationTypeTune, 64);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kTestNote.get());
}

TEST_CASE("Velocity switch mode selects the configured alternate notes",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::VelocitySwitch);
    triggerUnfinalizedDrumNoteOn(mpc, 45, mpc::NoteVariationTypeTune, 64);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kSwitchNoteA.get());
}

TEST_CASE("Velocity switch mode uses the second alternate note above the upper threshold",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::VelocitySwitch);
    triggerUnfinalizedDrumNoteOn(mpc, 89, mpc::NoteVariationTypeTune, 64);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kSwitchNoteB.get());
}

TEST_CASE("Decay switch mode keeps the original note at or below lower threshold",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::DecaySwitch);
    triggerUnfinalizedDrumNoteOn(
        mpc, 64, mpc::NoteVariationTypeDecay, 44);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kTestNote.get());
}

TEST_CASE("Decay switch mode selects the first alternate note above lower threshold",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::DecaySwitch);
    triggerUnfinalizedDrumNoteOn(
        mpc, 64, mpc::NoteVariationTypeDecay, 45);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kSwitchNoteA.get());
}

TEST_CASE("Decay switch mode uses note variation decay when selecting the target note",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureSwitchingProgramNote(
        mpc, mpc::sampler::SoundGenerationMode::DecaySwitch);
    triggerUnfinalizedDrumNoteOn(
        mpc, 64, mpc::NoteVariationTypeDecay, 89);

    const auto activeNotes = getActiveVoiceNotes(mpc);
    REQUIRE(activeNotes.size() == 1U);
    REQUIRE(activeNotes.front() == kSwitchNoteB.get());
}

TEST_CASE("Slider decay note variation includes configured low range",
          "[drum-note-routing]")
{
    const Util::SliderNoteVariationContext lowContext{
        0, mpc::NoteVariationTypeDecay, -120, 120, 12, 45, 0, 20, -50, 50};
    const auto [lowType, lowValue] =
        Util::getSliderNoteVariationTypeAndValue(lowContext);
    REQUIRE(lowType == mpc::NoteVariationTypeDecay);
    REQUIRE(lowValue == 12);

    const Util::SliderNoteVariationContext highContext{
        127, mpc::NoteVariationTypeDecay, -120, 120, 12, 45, 0, 20, -50, 50};
    const auto [highType, highValue] =
        Util::getSliderNoteVariationTypeAndValue(highContext);
    REQUIRE(highType == mpc::NoteVariationTypeDecay);
    REQUIRE(highValue == 44);
}
