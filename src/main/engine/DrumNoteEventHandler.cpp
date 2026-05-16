#include "DrumNoteEventHandler.hpp"

#include "Voice.hpp"
#include "FaderControl.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"

#include "engine/audio/mixer/AudioMixer.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"

#include "MpcSpecs.hpp"
#include "sequencer/Bus.hpp"

#include <utility>

using namespace mpc;
using namespace mpc::performance;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::lcdgui::screens;
using namespace mpc::engine::control;

static constexpr float PAN_SCALE = 100.0f;
static constexpr float AUX_PAN_LEFT = 0.0f;
static constexpr float AUX_PAN_CENTER = 0.5f;
static constexpr float AUX_PAN_RIGHT = 1.0f;

namespace
{
    void handleMuteGroups(const performance::NoteParameters &noteParameters,
                          const int drumIndex,
                          const std::vector<std::shared_ptr<Voice>> *voices)
    {
        if (noteParameters.muteAssignA !=
                Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED ||
            noteParameters.muteAssignB != Mpc2000XlSpecs::MUTE_ASSIGN_DISABLED)
        {
            for (const auto &v : *voices)
            {
                if (!v->isFinished() &&
                    (v->getMuteInfo().shouldMute(noteParameters.muteAssignA,
                                                 drumIndex) ||
                     v->getMuteInfo().shouldMute(noteParameters.muteAssignB,
                                                 drumIndex)))
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

    const auto program = c.drumBus->getPerformanceProgram();
    const auto np = program.getNoteParameters(c.note);

    handleMuteGroups(np, c.drum.drumBusIndex, c.voices);

    auto soundNumber = np.soundIndex;

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

    auto stereoMixer = c.mixerSetupScreen->isStereoMixSourceDrum()
                           ? c.drum.getStereoMixer(c.note)
                           : program.getNoteParameters(c.note).stereoMixer;

    auto indivFxMixer = c.mixerSetupScreen->isStereoMixSourceDrum()
                            ? c.drum.getIndivFxMixer(c.note)
                            : program.getNoteParameters(c.note).indivFxMixer;

    auto mixerControls = c.mixer->getMixerControls();
    auto stripNumber = voice->getStripNumber();
    auto voiceStrip =
        mixerControls->getStripControls(std::to_string(stripNumber));
    auto mmc =
        std::dynamic_pointer_cast<MainMixControls>(voiceStrip->find("Main"));

    std::dynamic_pointer_cast<PanControl>(mmc->find("Pan"))
        ->setValue(static_cast<float>(stereoMixer.panning) / PAN_SCALE);
    std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))
        ->setValue(stereoMixer.level);

    const int individualOutput = static_cast<int>(indivFxMixer.individualOutput);
    const int selectedAssignableMixOutPair =
        individualOutput > 0 ? (individualOutput - 1) / 2 : -1;

    for (int i = 0; i < 4; i++)
    {
        auto auxControl = std::dynamic_pointer_cast<CompoundControl>(
            voiceStrip->find("AUX#" + std::to_string(i + 1)));
        auto auxLevel =
            std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));
        auto auxPan =
            std::dynamic_pointer_cast<PanControl>(auxControl->find("Pan"));

        const bool isSelectedPair = i == selectedAssignableMixOutPair;
        auxLevel->setValue(
            isSelectedPair ? static_cast<float>(indivFxMixer.individualOutLevel)
                           : 0);

        if (!isSelectedPair || !sound->isMono())
        {
            auxPan->setValue(AUX_PAN_CENTER);
        }
        else if (individualOutput % 2 == 1)
        {
            auxPan->setValue(AUX_PAN_LEFT);
        }
        else
        {
            auxPan->setValue(AUX_PAN_RIGHT);
        }
    }

    if (!sound->isLoopEnabled() &&
        np.voiceOverlapMode == sampler::VoiceOverlapMode::MONO)
    {
        for (const auto &v : *c.voices)
        {
            if (v->isPlayingDrumProgramNoteCombination(
                    c.drum.drumBusIndex, c.drum.programIndex, c.note))
            {
                v->startDecay();
            }
        }
    }

    voice->init(c.velocity, sound, c.note, np, c.varType, c.varValue,
                c.drum.drumBusIndex, c.frameOffset, true, c.startTick,
                c.mixer->getSharedBuffer()->getSampleRate(), c.noteEventId,
                c.drum.programIndex);

    if (c.firstGeneration && np.soundGenerationMode == 1)
    {
        int optA = np.optionalNoteA;
        int optB = np.optionalNoteB;

        if (optA != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            auto ctxOptA = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                c.noteEventId, c.drum, c.drumBus, c.sampler, c.mixer,
                c.mixerSetupScreen, c.voices, optA, c.velocity, c.varType,
                c.varValue, c.frameOffset, false, c.startTick,
                c.durationFrames);

            noteOn(ctxOptA);

            c.setOptA(c.note, optA);
        }

        if (optB != Mpc2000XlSpecs::OPTIONAL_NOTE_DISABLED)
        {
            auto ctxOptB = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                c.noteEventId, c.drum, c.drumBus, c.sampler, c.mixer,
                c.mixerSetupScreen, c.voices, optB, c.velocity, c.varType,
                c.varValue, c.frameOffset, false, c.startTick,
                c.durationFrames);

            noteOn(ctxOptB);

            c.setOptB(c.note, optB);
        }
    }
}

void DrumNoteEventHandler::noteOff(const DrumNoteOffContext &ctx)
{
    if (ctx.note < Mpc2000XlSpecs::FIRST_DRUM_NOTE ||
        ctx.note > Mpc2000XlSpecs::LAST_DRUM_NOTE)
    {
        return;
    }

    auto startDecayForNote =
        [&](const int noteToStop, const uint64_t noteEventId)
    {
        for (const auto &voice : *ctx.voices)
        {
            if (!voice->isFinished() &&
                voice->getStartTick() == ctx.noteOnStartTick &&
                voice->getNote() == noteToStop &&
                voice->getVoiceOverlapMode() ==
                    sampler::VoiceOverlapMode::NOTE_OFF &&
                !voice->isDecaying() &&
                ctx.drumIndex == voice->getMuteInfo().getDrum() &&
                (ctx.noteEventId == 0 ||
                 voice->getNoteEventId() == noteEventId))
            {
                voice->startDecay();
                break;
            }
        }
    };

    startDecayForNote(ctx.note, ctx.noteEventId);

    if (const auto it = ctx.drumSimultA->find(ctx.note);
        it != ctx.drumSimultA->end())
    {
        startDecayForNote(it->second, ctx.noteEventId);
        ctx.drumSimultA->erase(it);
    }

    if (const auto it = ctx.drumSimultB->find(ctx.note);
        it != ctx.drumSimultB->end())
    {
        startDecayForNote(it->second, ctx.noteEventId);
        ctx.drumSimultB->erase(it);
    }
}
