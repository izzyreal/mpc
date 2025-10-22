#include "DrumNoteEventHandler.hpp"

#include "Voice.hpp"
#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"
#include "MixerInterconnection.hpp"
#include "FaderControl.hpp"

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Sound.hpp>

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"

#include "MpcSpecs.hpp"

#include <cmath>
#include <utility>

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::lcdgui::screens;
using namespace mpc::sampler;
using namespace mpc::engine::control;

namespace
{
    void handleMuteGroups(NoteParameters *noteParameters, int drumIndex, const std::vector<std::shared_ptr<Voice>>& voices)
    {
        if (noteParameters->getMuteAssignA() != Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED ||
                noteParameters->getMuteAssignB() != Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED)
        {
            for (const auto& v : voices)
            {
                if (!v->isFinished() && (v->getMuteInfo().shouldMute(noteParameters->getMuteAssignA(), drumIndex) ||
                                         v->getMuteInfo().shouldMute(noteParameters->getMuteAssignB(), drumIndex)))
                {
                    v->startDecay();
                }
            }
        }
    }
}

void DrumNoteEventHandler::noteOn(
    std::shared_ptr<Sampler> sampler,
    std::shared_ptr<AudioMixer> mixer,
    std::shared_ptr<MixerSetupScreen> mixerSetupScreen,
    std::vector<std::shared_ptr<Voice>>& voices,
    std::vector<std::shared_ptr<StereoMixer>>& drumStereoMixerChannels,
    std::vector<std::shared_ptr<IndivFxMixer>>& drumIndivFxMixerChannels,
    std::vector<MixerInterconnection*>& mixerConnections,
    std::map<int, int>& simultA,
    std::map<int, int>& simultB,
    int drumIndex,
    int programIndex,
    int note, int velo, int varType, int varValue,
    int frameOffset, bool firstGeneration,
    int startTick, int durationFrames
)
{
    if (note < Mpc2000XlSpecs::FIRST_DRUM_NOTE || note > Mpc2000XlSpecs::LAST_DRUM_NOTE || velo == 0)
        return;

    auto program = sampler->getProgram(programIndex);
    auto np = program ? program->getNoteParameters(note) : nullptr;

    if (!np)
    {
        return;
    }

    handleMuteGroups(np, drumIndex, voices);

    auto soundNumber = np->getSoundIndex();
    std::shared_ptr<Voice> voice;

    for (auto& v : voices)
    {
        if (v->isFinished())
        {
            voice = v;
            break;
        }
    }

    if (soundNumber == -1 || !voice)
        return;

    auto sound = sampler->getSound(soundNumber);
    if (!sound)
        return;

    auto smc = program->getStereoMixerChannel(note - Mpc2000XlSpecs::FIRST_DRUM_NOTE);
    auto ifmc = program->getIndivFxMixerChannel(note - Mpc2000XlSpecs::FIRST_DRUM_NOTE);

    if (mixerSetupScreen->isStereoMixSourceDrum())
        smc = drumStereoMixerChannels[note - Mpc2000XlSpecs::FIRST_DRUM_NOTE];

    if (mixerSetupScreen->isIndivFxSourceDrum())
        ifmc = drumIndivFxMixerChannels[note - Mpc2000XlSpecs::FIRST_DRUM_NOTE];

    auto mixerControls = mixer->getMixerControls();
    auto stripNumber = voice->getStripNumber();
    auto mainStrip = mixerControls->getStripControls(std::to_string(stripNumber));
    auto mmc = std::dynamic_pointer_cast<MainMixControls>(mainStrip->find("Main"));

    std::dynamic_pointer_cast<PanControl>(mmc->find("Pan"))->setValue(static_cast<float>(smc->getPanning() / PAN_SCALE));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(smc->getLevel()));

    auto duplicateStrip = mixerControls->getStripControls(std::to_string(stripNumber + STRIP_OFFSET));
    mmc = std::dynamic_pointer_cast<MainMixControls>(duplicateStrip->find("Main"));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(0);

    if (ifmc->getOutput() > 0)
    {
        if (sound->isMono())
        {
            auto connection = mixerConnections[stripNumber - 1];
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
            auto connection = mixerConnections[stripNumber - 1];
            connection->setLeftEnabled(true);
            connection->setRightEnabled(true);
        }
    }

    int selectedAssignableMixOutPair = static_cast<int>(ceil((ifmc->getOutput() - 2) * 0.5));
    for (int i = 0; i < 4; i++)
    {
        auto auxControl = std::dynamic_pointer_cast<CompoundControl>(mainStrip->find("AUX#" + std::to_string(i + 1)));
        auto auxLevel = std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));
        auxLevel->setValue(i == selectedAssignableMixOutPair ? static_cast<float>(ifmc->getVolumeIndividualOut()) : 0);
    }

    if (!sound->isLoopEnabled() && np->getVoiceOverlap() == VoiceOverlapMode::MONO)
    {
        for (auto& v : voices)
        {
            if (v->getNoteParameters() == np && v->getNote() == note &&
                (v->getVoiceOverlap() == VoiceOverlapMode::POLY || v->getVoiceOverlap() == VoiceOverlapMode::MONO))
            {
                v->startDecay();
            }
        }
    }

    voice->init(velo, sound, note, np, varType, varValue, note, drumIndex,
                frameOffset, true, startTick, durationFrames,
                mixer->getSharedBuffer()->getSampleRate());

    if (firstGeneration && np->getSoundGenerationMode() == 1)
    {
        int optA = np->getOptionalNoteA();
        int optB = np->getOptionalNoteB();

        if (optA != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            noteOn(sampler, mixer, mixerSetupScreen, voices,
                   drumStereoMixerChannels, drumIndivFxMixerChannels, mixerConnections,
                   simultA, simultB, drumIndex, programIndex,
                   optA, velo, varType, varValue, frameOffset,
                   false, startTick, durationFrames);
            simultA[note] = optA;
        }

        if (optB != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            noteOn(sampler, mixer, mixerSetupScreen, voices,
                   drumStereoMixerChannels, drumIndivFxMixerChannels, mixerConnections,
                   simultA, simultB, drumIndex, programIndex,
                   optB, velo, varType, varValue, frameOffset,
                   false, startTick, durationFrames);
            simultB[note] = optB;
        }
    }
}

void DrumNoteEventHandler::noteOff(
    std::vector<std::shared_ptr<Voice>>& voices,
    std::map<int, int>& simultA,
    std::map<int, int>& simultB,
    int drumIndex,
    int note,
    int frameOffset,
    int noteOnStartTick
)
{
    if (note < Mpc2000XlSpecs::FIRST_DRUM_NOTE ||
            note > Mpc2000XlSpecs::LAST_DRUM_NOTE)
    {
        return;
    }

    auto startDecayForNote = [&](int noteToStop)
    {
        for (auto& voice : voices)
        {
            if (!voice->isFinished() &&
                voice->getStartTick() == noteOnStartTick &&
                voice->getNote() == noteToStop &&
                voice->getVoiceOverlap() == VoiceOverlapMode::NOTE_OFF &&
                !voice->isDecaying() &&
                drumIndex == voice->getMuteInfo().getDrum())
            {
                voice->startDecay(frameOffset);
                break;
            }
        }
    };

    startDecayForNote(note);

    if (auto it = simultA.find(note); it != simultA.end())
    {
        startDecayForNote(it->second);
        simultA.erase(it);
    }

    if (auto it = simultB.find(note); it != simultB.end())
    {
        startDecayForNote(it->second);
        simultB.erase(it);
    }
}

