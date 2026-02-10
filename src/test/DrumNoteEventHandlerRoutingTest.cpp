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
#include "engine/MixerInterconnection.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MixerControls.hpp"
#include "engine/control/CompoundControl.hpp"
#include "engine/FaderControl.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Program.hpp"
#include "sampler/NoteParameters.hpp"
#include "engine/IndivFxMixer.hpp"

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
    static constexpr int kIndivLevel = 73;

    std::shared_ptr<FaderControl> getAuxLevelControl(
        const std::shared_ptr<AudioMixer> &mixer, const std::string &stripName,
        const std::string &auxName)
    {
        const auto strip =
            mixer->getMixerControls()->getStripControls(stripName);
        REQUIRE(strip != nullptr);

        const auto auxControl =
            std::dynamic_pointer_cast<CompoundControl>(strip->find(auxName));
        REQUIRE(auxControl != nullptr);

        const auto levelControl =
            std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));
        REQUIRE(levelControl != nullptr);

        return levelControl;
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
} // namespace

TEST_CASE("Mono sound routed to individual out uses one channel and duplicate strip AUX",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, true, DrumMixerIndividualOutput(3));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto &connections = mpc.getEngineHost()->getMixerConnections();
    REQUIRE(connections.at(0)->isLeftEnabled());
    REQUIRE(!connections.at(0)->isRightEnabled());

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "33", "AUX#2")->getValue() ==
            static_cast<float>(kIndivLevel));
}

TEST_CASE("Mono sound routed to main keeps both channels and no individual AUX send",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, true, DrumMixerIndividualOutput(0));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto &connections = mpc.getEngineHost()->getMixerConnections();
    REQUIRE(connections.at(0)->isLeftEnabled());
    REQUIRE(connections.at(0)->isRightEnabled());

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "33", "AUX#1")->getValue() == 0.f);
}

TEST_CASE("Stereo sound to indiv out keeps both chans & uses duplicate AUX",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, false, DrumMixerIndividualOutput(3));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto &connections = mpc.getEngineHost()->getMixerConnections();
    REQUIRE(connections.at(0)->isLeftEnabled());
    REQUIRE(connections.at(0)->isRightEnabled());

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#2")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "33", "AUX#2")->getValue() ==
            static_cast<float>(kIndivLevel));
}

TEST_CASE("Stereo sound routed to main keeps both channels and no individual AUX send",
          "[drum-note-routing]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);

    configureProgramNote(mpc, false, DrumMixerIndividualOutput(0));
    triggerUnfinalizedDrumNoteOn(mpc);

    const auto &connections = mpc.getEngineHost()->getMixerConnections();
    REQUIRE(connections.at(0)->isLeftEnabled());
    REQUIRE(connections.at(0)->isRightEnabled());

    const auto mixer = mpc.getEngineHost()->getMixer();
    REQUIRE(getAuxLevelControl(mixer, "1", "AUX#1")->getValue() == 0.f);
    REQUIRE(getAuxLevelControl(mixer, "33", "AUX#1")->getValue() == 0.f);
}
