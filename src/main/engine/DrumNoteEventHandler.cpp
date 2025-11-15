#include "DrumNoteEventHandler.hpp"

#include "Voice.hpp"
#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"
#include "MixerInterconnection.hpp"
#include "FaderControl.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/NoteParameters.hpp"

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"

#include "MpcSpecs.hpp"
#include "sequencer/Bus.hpp"

#include <utility>

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::lcdgui::screens;
using namespace mpc::sampler;
using namespace mpc::engine::control;

static constexpr float PAN_SCALE = 100.0f;
static constexpr int STRIP_OFFSET = 32;

namespace
{
    void handleMuteGroups(const NoteParameters *noteParameters,
                          const int drumIndex,
                          const std::vector<std::shared_ptr<Voice>> *voices)
    {
        if (noteParameters->getMuteAssignA() !=
                Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED ||
            noteParameters->getMuteAssignB() !=
                Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED)
        {
            for (const auto &v : *voices)
            {
                if (!v->isFinished() &&
                    (v->getMuteInfo().shouldMute(
                         noteParameters->getMuteAssignA(), drumIndex) ||
                     v->getMuteInfo().shouldMute(
                         noteParameters->getMuteAssignB(), drumIndex)))
                {
                    v->startDecay();
                }
            }
        }
    }
} // namespace

void DrumNoteEventHandler::noteOn(const DrumNoteOnContext &c)
{
    if (c.note < Mpc2000XlSpecs::FIRST_DRUM_NOTE ||
        c.note > Mpc2000XlSpecs::LAST_DRUM_NOTE || c.velocity == 0)
    {
        return;
    }

    auto program = c.sampler->getProgram(c.drum->getProgramIndex());
    auto np = program ? program->getNoteParameters(c.note) : nullptr;

    if (!np)
    {
        return;
    }

    handleMuteGroups(np, c.drum->getIndex(), c.voices);

    auto soundNumber = np->getSoundIndex();

    std::shared_ptr<Voice> voice;

    for (auto &v : *c.voices)
    {
        if (v->isFinished())
        {
            voice = v;
            break;
        }
    }

    if (soundNumber == -1 || !voice)
    {
        return;
    }

    auto sound = c.sampler->getSound(soundNumber);

    if (!sound)
    {
        return;
    }

    auto smc = program->getStereoMixerChannel(c.note -
                                              Mpc2000XlSpecs::FIRST_DRUM_NOTE);
    auto ifmc = program->getIndivFxMixerChannel(
        c.note - Mpc2000XlSpecs::FIRST_DRUM_NOTE);

    if (c.mixerSetupScreen->isStereoMixSourceDrum())
    {
        smc = c.drum->getStereoMixerChannels().at(
            c.note - Mpc2000XlSpecs::FIRST_DRUM_NOTE);
    }

    if (c.mixerSetupScreen->isIndivFxSourceDrum())
    {
        ifmc = c.drum->getIndivFxMixerChannels().at(
            c.note - Mpc2000XlSpecs::FIRST_DRUM_NOTE);
    }

    auto mixerControls = c.mixer->getMixerControls();
    auto stripNumber = voice->getStripNumber();
    auto mainStrip =
        mixerControls->getStripControls(std::to_string(stripNumber));
    auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(mainStrip->find("Main"));

    std::dynamic_pointer_cast<PanControl>(mmc->find("Pan"))
        ->setValue(static_cast<float>(smc->getPanning()) / PAN_SCALE);
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))
        ->setValue(static_cast<float>(smc->getLevel()));

    auto duplicateStrip = mixerControls->getStripControls(
        std::to_string(stripNumber + STRIP_OFFSET));
    mmc = std::dynamic_pointer_cast<MainMixControls>(
        duplicateStrip->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(0);

    if (ifmc->getOutput() > 0)
    {
        if (sound->isMono())
        {
            auto connection = c.mixerConnections->at(stripNumber - 1);

            if (ifmc->getOutput() % 2 == 1)
            {
                connection->setLeftEnabled(true);
                connection->setRightEnabled(false);
            }
            else
            {
                connection->setLeftEnabled(false);
                connection->setRightEnabled(true);
            }
        }
        else
        {
            auto connection = c.mixerConnections->at(stripNumber - 1);
            connection->setLeftEnabled(true);
            connection->setRightEnabled(true);
        }
    }

    int selectedAssignableMixOutPair =
        static_cast<int>(ceil((ifmc->getOutput() - 2) * 0.5));

    for (int i = 0; i < 4; i++)
    {
        auto auxControl = std::dynamic_pointer_cast<CompoundControl>(
            mainStrip->find("AUX#" + std::to_string(i + 1)));
        auto auxLevel =
            std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));
        auxLevel->setValue(
            i == selectedAssignableMixOutPair
                ? static_cast<float>(ifmc->getVolumeIndividualOut())
                : 0);
    }

    if (!sound->isLoopEnabled() &&
        np->getVoiceOverlapMode() == VoiceOverlapMode::MONO)
    {
        for (const auto &v : *c.voices)
        {
            if (v->getNoteParameters() == np && v->getNote() == c.note)
            {
                v->startDecay();
            }
        }
    }

    voice->init(c.velocity, sound, c.note, np, c.varType, c.varValue,
                c.drum->getIndex(), c.frameOffset, true, c.startTick,
                c.mixer->getSharedBuffer()->getSampleRate(), c.noteEventId);

    if (c.firstGeneration && np->getSoundGenerationMode() == 1)
    {
        int optA = np->getOptionalNoteA();
        int optB = np->getOptionalNoteB();

        if (optA != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            auto ctxOptA = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                c.noteEventId, c.drum, c.sampler, c.mixer, c.mixerSetupScreen,
                c.voices, *c.mixerConnections, optA, c.velocity, c.varType,
                c.varValue, c.frameOffset, false, c.startTick,
                c.durationFrames);

            noteOn(ctxOptA);

            c.drum->getSimultA()[c.note] = optA;
        }

        if (optB != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            auto ctxOptB = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                c.noteEventId, c.drum, c.sampler, c.mixer, c.mixerSetupScreen,
                c.voices, *c.mixerConnections, optB, c.velocity, c.varType,
                c.varValue, c.frameOffset, false, c.startTick,
                c.durationFrames);

            noteOn(ctxOptB);

            c.drum->getSimultB()[c.note] = optB;
        }
    }
}

void DrumNoteEventHandler::noteOff(const DrumNoteOffContext &c)
{
    if (c.note < Mpc2000XlSpecs::FIRST_DRUM_NOTE ||
        c.note > Mpc2000XlSpecs::LAST_DRUM_NOTE)
    {
        return;
    }

    auto startDecayForNote = [&](int noteToStop, uint64_t noteEventId)
    {
        for (const auto &voice : *c.voices)
        {
            if (!voice->isFinished() &&
                voice->getStartTick() == c.noteOnStartTick &&
                voice->getNote() == noteToStop &&
                voice->getVoiceOverlapMode() == VoiceOverlapMode::NOTE_OFF &&
                !voice->isDecaying() &&
                c.drumIndex == voice->getMuteInfo().getDrum() &&
                (c.noteEventId == 0 || voice->getNoteEventId() == noteEventId))
            {
                voice->startDecay();
                break;
            }
        }
    };

    startDecayForNote(c.note, c.noteEventId);

    if (auto it = c.drumSimultA->find(c.note); it != c.drumSimultA->end())
    {
        startDecayForNote(it->second, c.noteEventId);
        c.drumSimultA->erase(it);
    }

    if (auto it = c.drumSimultB->find(c.note); it != c.drumSimultB->end())
    {
        startDecayForNote(it->second, c.noteEventId);
        c.drumSimultB->erase(it);
    }
}
