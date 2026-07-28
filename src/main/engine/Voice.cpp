#include "Voice.hpp"

#include "VoiceConstants.hpp"

#include "sampler/Sound.hpp"

#include "EnvelopeControls.hpp"
#include "EnvelopeGenerator.hpp"
#include "Mpc2000XlAmpEnvelope.hpp"
#include "Mpc2000XlFilterEnvelope.hpp"

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/filter/StateVariableFilter.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::engine;
using namespace mpc::engine::audio::core;
using namespace mpc::engine::control;
using namespace mpc::engine::filter;
using namespace mpc::sampler;

typedef VoiceConstants C;

Voice::Voice(const int stripNumber, const bool isBasicVoice)
    : stripNumber(stripNumber), isBasicVoice(isBasicVoice),
      frame(C::EMPTY_FRAME), stateA(), stateB()
{
    staticEnvControls =
        new EnvelopeControls(0, "StaticAmpEnv", C::AMPENV_OFFSET);
    staticEnv = new EnvelopeGenerator(staticEnvControls);
    shold = std::dynamic_pointer_cast<LawControl>(
                staticEnvControls->getControls()[C::HOLD_INDEX])
                .get();

    const auto sattack = std::dynamic_pointer_cast<LawControl>(
                             staticEnvControls->getControls()[C::ATTACK_INDEX])
                             .get();
    const auto sdecay = std::dynamic_pointer_cast<LawControl>(
                            staticEnvControls->getControls()[C::DECAY_INDEX])
                            .get();

    sattack->setValue(C::STATIC_ATTACK_LENGTH);
    sdecay->setValue(C::STATIC_DECAY_LENGTH);

    if (!isBasicVoice)
    {
        ampEnv = new Mpc2000XlAmpEnvelope();
        filterEnv = new Mpc2000XlFilterEnvelope();
        svfLeft = new StateVariableFilter();
        svfRight = new StateVariableFilter();
    }
}

Voice::~Voice()
{
    delete staticEnvControls;
    delete staticEnv;

    if (!isBasicVoice)
    {
        delete ampEnv;
        delete filterEnv;
        delete svfLeft;
        delete svfRight;
    }
}

void Voice::init(const int velocity, const std::shared_ptr<Sound> &sound,
                 const int noteNumber,
                 const performance::NoteParameters &noteParameters,
                 const int varType, const int varValue, const int drumIndex,
                 const int frameOffset, const bool enableEnvs,
                 const int startTick, const float engineSampleRate,
                 const uint64_t noteEventId, const ProgramIndex programIndex)
{
    VoiceState *state = getInactiveState();

    state->drumBusIndex = DrumBusIndex(drumIndex);
    state->programIndex = programIndex;
    state->finished = false;

    state->noteEventId = noteEventId;

    state->noteParameters = noteParameters;
    state->startTick = startTick;

    state->enableEnvs = enableEnvs;
    state->frameOffset = frameOffset;
    state->noteNumber = noteNumber;
    state->velocity = velocity;
    state->varType = varType;
    state->varValue = varValue;

    state->staticDecay = false;
    state->muteInfo.setNote(noteNumber);
    state->muteInfo.setDrum(drumIndex);
    state->veloToStart = 0;
    state->attackValue = 0;
    state->decayValue = 2;
    state->veloToAttack = 0;
    state->decayMode = 0;
    state->veloToLevel = 100;

    state->tune = sound->getTune();

    if (!isBasicVoice)
    {
        state->tune += noteParameters.tune;
        state->veloToStart = noteParameters.velocityToStart;
        state->attackValue = noteParameters.attack;
        state->decayValue = noteParameters.decay;
        state->veloToAttack = noteParameters.velocityToAttack;
        state->decayMode = noteParameters.decayMode;
        state->veloToLevel = noteParameters.velocityToLevel;
        state->voiceOverlapMode = sound->isLoopEnabled()
                                      ? VoiceOverlapMode::NOTE_OFF
                                      : noteParameters.voiceOverlapMode;
    }

    switch (state->varType)
    {
        case 0:
            state->tune += (state->varValue - 64) * 2;
            break;
        case 1:
            state->decayValue = state->varValue;
            state->decayMode = 1;
            break;
        case 2:
            state->attackValue = state->varValue;
            break;
        default:;
    }

    state->start = sound->getStart();
    state->end = sound->getEnd();
    state->loopTo = sound->getLoopTo();
    state->lastFrameIndex = sound->getLastFrameIndex();
    state->loopEnabled = sound->isLoopEnabled();
    state->isMono = sound->isMono();
    state->sampleData = sound->getSampleData();
    state->soundLevel = sound->getSndLevel();
    state->sampleRate = engineSampleRate;

    staticEnv->reset();

    if (!isBasicVoice)
    {
        Mpc2000XlFilterEnvelope::Parameters parameters;
        parameters.cutoff = noteParameters.filterFrequency;
        parameters.resonance = noteParameters.filterResonance;
        parameters.attack = noteParameters.filterAttack;
        parameters.decay = noteParameters.filterDecay;
        parameters.amount = noteParameters.filterEnvelopeAmount;
        parameters.velocityToCutoff =
            noteParameters.velocityToFilterFrequency;
        parameters.velocity = state->velocity;
        parameters.filterNoteVariationEnabled = state->varType == 3;
        parameters.filterNoteVariation = state->varValue;
        parameters.outputSampleRate = state->sampleRate;
        filterEnv->configure(parameters);

        state->filterCutoff = filterEnv->getBaseIndex();
        state->filterResonance =
            std::clamp<int>(noteParameters.filterResonance, 0, 15);
        svfLeft->resetElementState();
        svfRight->resetElementState();
    }

    state->decayCounter = 0;

    initializeSamplerateDependents(state, true);

    swapStates();
}

void Voice::initializeSamplerateDependents(VoiceState *state,
                                           const bool resetEnvelope)
{
    staticEnvControls->setSampleRate(state->sampleRate);

    if (!isBasicVoice)
    {
        filterEnv->setOutputSampleRate(state->sampleRate);
    }

    const auto pitchStep = std::clamp(
        static_cast<int>(std::lround(
            pow(2.0, static_cast<double>(state->tune) / 120.0) * 0x1000)),
        1, 0xffff);
    state->increment = static_cast<double>(pitchStep) / 0x1000 *
                       (44100.0 / state->sampleRate);

    if (resetEnvelope)
    {
        state->position =
            state->start +
            (isBasicVoice
                 ? 0
                 : Mpc2000XlAmpEnvelope::startOffsetFrames(
                       state->velocity, state->veloToStart));
    }

    const auto playableSampleLength =
        state->loopEnabled
            ? INT_MAX
            : static_cast<int>((state->end - state->position) /
                               state->increment);

    const auto staticEnvHoldSamples =
        static_cast<int>(playableSampleLength -
                         (C::STATIC_ATTACK_LENGTH + C::STATIC_DECAY_LENGTH) /
                             C::ENV_TIME_RATIO * state->sampleRate * 0.001);

    shold->setValue(staticEnvHoldSamples);

    if (!isBasicVoice)
    {
        if (resetEnvelope)
        {
            Mpc2000XlAmpEnvelope::Parameters parameters;
            parameters.attack = state->attackValue;
            parameters.decay = state->decayValue;
            parameters.velocity = state->velocity;
            parameters.velocityToLevel = state->veloToLevel;
            parameters.velocityToAttack = state->veloToAttack;
            parameters.velocityToStart = state->veloToStart;
            parameters.soundLevel = state->soundLevel;
            parameters.pitchStep = pitchStep;
            parameters.startFrame = state->start;
            parameters.endFrame = state->end;
            parameters.looping = state->loopEnabled;
            parameters.decayModeStart = state->decayMode == 1;
            parameters.outputSampleRate = state->sampleRate;
            ampEnv->configure(parameters);
        }
        else
        {
            ampEnv->setOutputSampleRate(state->sampleRate);
        }
    }
}

const std::vector<float> &Voice::getFrame()
{
    VoiceState *state = getActiveState();

    if (state->finished)
    {
        return C::EMPTY_FRAME;
    }

    if (state->frameOffset > 0)
    {
        state->frameOffset--;
        return C::EMPTY_FRAME;
    }

    state->envAmplitude = isBasicVoice ? 1.0f : ampEnv->next();
    state->staticEnvAmp =
        state->enableEnvs ? staticEnv->getEnvelope(state->staticDecay) : 1.0f;
    state->envAmplitude *= state->staticEnvAmp;

    readFrame();

    const auto filterCoefficient =
        isBasicVoice ? 0.0 : filterEnv->nextCoefficient();

    if (state->isMono)
    {
        if (!isBasicVoice)
        {
            frame[0] = svfLeft->filter(frame[0], filterCoefficient,
                                       state->filterResonance);
        }

        frame[0] *= state->envAmplitude;
        frame[1] = frame[0];
    }
    else
    {
        if (!isBasicVoice)
        {
            frame[0] = svfLeft->filter(frame[0], filterCoefficient,
                                       state->filterResonance);
            frame[1] = svfRight->filter(frame[1], filterCoefficient,
                                        state->filterResonance);
        }

        frame[0] *= state->envAmplitude;
        frame[1] *= state->envAmplitude;
    }

    return frame;
}

void Voice::readFrame()
{
    VoiceState *state = getActiveState();

    if (state->loopEnabled && state->position > state->end - 1)
    {
        state->position = state->loopTo;
    }

    if ((state->position >= state->end - 1 && !state->loopEnabled) ||
        (staticEnv != nullptr && staticEnv->isComplete()) ||
        (ampEnv != nullptr && ampEnv->isComplete()))
    {
        frame = C::EMPTY_FRAME;
        state->finished = true;
        return;
    }

    const auto k = static_cast<int>(std::ceil(state->position));
    const auto j = k != 0 ? k - 1 : 0;
    const auto frac = state->position - static_cast<double>(j);
    const auto &sampleData = state->sampleData;

    if (state->isMono)
    {
        frame[0] = (*sampleData)[j] * (1.0f - frac) + (*sampleData)[k] * frac;
    }
    else
    {
        frame[0] = (*sampleData)[j] * (1.0f - frac) + (*sampleData)[k] * frac;
        const auto rOffset = sampleData->size() * 0.5;
        frame[1] = (*sampleData)[j + rOffset] * (1.0f - frac) +
                   (*sampleData)[k + rOffset] * frac;
    }

    state->position += state->increment;
}

int Voice::processAudio(AudioBuffer *buffer, const int nFrames)
{
    VoiceState *state = getActiveState();

    if (state->finished)
    {
        buffer->makeSilence();
        return AUDIO_SILENCE;
    }

    if (buffer->getSampleRate() != state->sampleRate)
    {
        state->sampleRate = buffer->getSampleRate();
        initializeSamplerateDependents(state, false);
    }

    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    const auto masterLevelToUse = masterLevel.load(std::memory_order_relaxed);

    const auto masterLevelFactor =
        masterLevelToUse > -128
            ? std::pow(10.f, static_cast<float>(masterLevelToUse) * 0.04f)
            : 0;

    for (int i = 0; i < nFrames; i++)
    {
        auto &frameToUse = getFrame();

        left[i] = frameToUse[0] * masterLevelFactor;
        right[i] = frameToUse[1] * masterLevelFactor;

        if (state->decayCounter != 0)
        {
            if (state->decayCounter == 1)
            {
                startDecay();
            }

            state->decayCounter--;
        }
    }

    if (state->finished)
    {
        state->noteNumber = -1;
    }

    return AUDIO_OK;
}

void Voice::finish()
{
    VoiceState *state = getActiveState();
    state->finished = true;
}

void Voice::startDecay()
{
    VoiceState *state = getActiveState();
    state->staticDecay = true;
}

void Voice::startDecay(const int offset)
{
    VoiceState *state = getActiveState();

    if (offset > 0)
    {
        state->decayCounter = offset;
    }
    else
    {
        startDecay();
    }
}

void Voice::setMasterLevel(const int8_t masterLevelToUse)
{
    masterLevel.store(masterLevelToUse, std::memory_order_relaxed);
}

int Voice::getNote() const
{
    const VoiceState *state = getActiveState();
    return state->noteNumber;
}

int Voice::getVelocity() const
{
    const VoiceState *state = getActiveState();
    return state->velocity;
}

uint64_t Voice::getNoteEventId()
{
    const VoiceState *state = getActiveState();
    return state->noteEventId;
}

bool Voice::isFinished() const
{
    const VoiceState *state = getActiveState();
    return state->finished;
}

VoiceOverlapMode Voice::getVoiceOverlapMode() const
{
    const VoiceState *state = getActiveState();
    return state->voiceOverlapMode;
}

int Voice::getStripNumber() const
{
    return stripNumber;
}

int Voice::getStartTick() const
{
    const VoiceState *state = getActiveState();
    return state->startTick;
}

bool Voice::isDecaying() const
{
    const VoiceState *state = getActiveState();
    return state->staticDecay;
}

const MuteInfo &Voice::getMuteInfo() const
{
    const VoiceState *state = getActiveState();
    return state->muteInfo;
}

bool Voice::isPlayingDrumProgramNoteCombination(
    const DrumBusIndex drumBusIndex, const ProgramIndex programIndex,
    const DrumNoteNumber drumNoteNumber) const
{
    const VoiceState *state = getActiveState();
    return state->drumBusIndex == drumBusIndex &&
           state->programIndex == programIndex &&
           state->noteNumber == drumNoteNumber;
}
