#include "NoteEventProcessor.h"

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

#include <cmath>
#include <utility>

using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::lcdgui::screens;
using namespace mpc::sampler;
using namespace mpc::engine::control;

void NoteEventProcessor::noteOn(
    Sampler& sampler,
    AudioMixer& mixer,
    MixerSetupScreen* mixerSetupScreen,
    std::vector<std::shared_ptr<Voice>>& voices,
    std::vector<std::shared_ptr<StereoMixer>>& stereoMixerChannels,
    std::vector<std::shared_ptr<IndivFxMixer>>& indivFxMixerChannels,
    std::vector<MixerInterconnection*>& mixerConnections,
    std::map<int,int>& simultA,
    std::map<int,int>& simultB,
    int drumIndex,
    int programNumber,
    int note, int velo, int varType, int varValue,
    int frameOffset, bool firstGeneration,
    int startTick, int durationFrames
)
{
    if (note < 35 || note > 98 || velo == 0)
        return;

    auto program = sampler.getProgram(programNumber);
    if (!program) return;

    auto np = program->getNoteParameters(note);
    if (!np) return;

    // --- Handle mute groups ---
    if (np->getMuteAssignA() != 34 || np->getMuteAssignB() != 34)
    {
        for (auto& v : voices)
        {
            if (v->isFinished()) continue;

            if (v->getMuteInfo().shouldMute(np->getMuteAssignA(), drumIndex)
                || v->getMuteInfo().shouldMute(np->getMuteAssignB(), drumIndex))
            {
                v->startDecay();
            }
        }
    }

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

    auto sound = sampler.getSound(soundNumber);
    if (!sound) return;

    auto smc = program->getStereoMixerChannel(note - 35);
    auto ifmc = program->getIndivFxMixerChannel(note - 35);

    bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
    bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

    if (sSrcDrum)
        smc = stereoMixerChannels[note - 35];
    if (iSrcDrum)
        ifmc = indivFxMixerChannels[note - 35];

    auto mixerControls = mixer.getMixerControls();

    auto sc = mixerControls->getStripControls(std::to_string(voice->getStripNumber()));

    auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
    std::dynamic_pointer_cast<PanControl>(mmc->find("Pan"))->setValue(static_cast<float>(smc->getPanning() / 100.0));
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(smc->getLevel()));

    sc = mixerControls->getStripControls(std::to_string(voice->getStripNumber() + 32));
    mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));

    // Silence duplicate strip for Main
    auto faderControl = std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"));
    faderControl->setValue(0);

    if (ifmc->getOutput() > 0)
    {
        if (sound->isMono())
        {
            if (ifmc->getOutput() % 2 == 1)
            {
                mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(true);
                mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(false);
            }
            else
            {
                mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(false);
                mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(true);
            }
        }
        else
        {
            mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(true);
            mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(true);
        }
    }

    // --- Handle assignable mix outs (AUX1-4) ---
    int selectedAssignableMixOutPair = static_cast<int>(ceil((ifmc->getOutput() - 2) * 0.5));

    for (int i = 0; i < 4; i++)
    {
        auto auxControl = std::dynamic_pointer_cast<CompoundControl>(sc->find("AUX#" + std::to_string(i + 1)));
        auto auxLevel = std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));

        if (i == selectedAssignableMixOutPair)
        {
            auto value = static_cast<float>(ifmc->getVolumeIndividualOut());
            auxLevel->setValue(value);
        }
        else
        {
            auxLevel->setValue(0);
        }
    }

    // --- Stop conflicting mono/poly voices ---
    if (!sound->isLoopEnabled() && np->getVoiceOverlap() == VoiceOverlapMode::MONO)
    {
        for (auto& v : voices)
        {
            if (v->getNoteParameters() == np)
            {
                if (v->getNote() == note)
                {
                    if (v->getVoiceOverlap() == VoiceOverlapMode::POLY ||
                        v->getVoiceOverlap() == VoiceOverlapMode::MONO)
                    {
                        v->startDecay();
                    }
                }
            }
        }
    }

    voice->init(velo, sound, note, np, varType, varValue, note, drumIndex,
                frameOffset, true, startTick, durationFrames,
                mixer.getSharedBuffer()->getSampleRate());

    // --- Simultaneous notes ---
    if (firstGeneration && np->getSoundGenerationMode() == 1)
    {
        int optA = np->getOptionalNoteA();
        int optB = np->getOptionalNoteB();

        if (optA != 34)
        {
            noteOn(sampler, mixer, mixerSetupScreen, voices,
                   stereoMixerChannels, indivFxMixerChannels, mixerConnections,
                   simultA, simultB, drumIndex, programNumber,
                   optA, velo, varType, varValue, frameOffset,
                   false, startTick, durationFrames);
            simultA[note] = optA;
        }

        if (optB != 34)
        {
            noteOn(sampler, mixer, mixerSetupScreen, voices,
                   stereoMixerChannels, indivFxMixerChannels, mixerConnections,
                   simultA, simultB, drumIndex, programNumber,
                   optB, velo, varType, varValue, frameOffset,
                   false, startTick, durationFrames);
            simultB[note] = optB;
        }
    }
}

void NoteEventProcessor::noteOff(
    std::vector<std::shared_ptr<Voice>>& voices,
    std::map<int,int>& simultA,
    std::map<int,int>& simultB,
    int drumIndex,
    int note,
    int frameOffset,
    int noteOnStartTick
)
{
    if (note < 35 || note > 98)
        return;

    auto startDecayForNote = [&](int noteToStop)
    {
        for (auto& voice : voices)
        {
            if (!voice->isFinished()
                && voice->getStartTick() == noteOnStartTick
                && voice->getNote() == noteToStop
                && voice->getVoiceOverlap() == VoiceOverlapMode::NOTE_OFF
                && !voice->isDecaying()
                && drumIndex == voice->getMuteInfo().getDrum())
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
