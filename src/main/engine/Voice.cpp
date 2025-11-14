#include "Voice.hpp"

#include "VoiceConstants.hpp"

#include "sampler/NoteParameters.hpp"
#include "sampler/Sound.hpp"

#include "EnvelopeControls.hpp"
#include "EnvelopeGenerator.hpp"

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/filter/StateVariableFilter.hpp"
#include "engine/filter/StateVariableFilterControls.hpp"

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
        ampEnvControls = new EnvelopeControls(0, "AmpEnv", C::AMPENV_OFFSET);
        filterEnvControls =
            new EnvelopeControls(0, "StaticAmpEnv", C::AMPENV_OFFSET);
        ampEnv = new EnvelopeGenerator(ampEnvControls);
        filterEnv = new EnvelopeGenerator(filterEnvControls);
        svfControls = new StateVariableFilterControls("Filter", C::SVF_OFFSET);
        svfControls->createControls();
        svfLeft = new StateVariableFilter(svfControls);
        svfRight = new StateVariableFilter(svfControls);
        fattack = std::dynamic_pointer_cast<LawControl>(
                      filterEnvControls->getControls()[C::ATTACK_INDEX])
                      .get();
        fhold = std::dynamic_pointer_cast<LawControl>(
                    filterEnvControls->getControls()[C::HOLD_INDEX])
                    .get();
        fdecay = std::dynamic_pointer_cast<LawControl>(
                     filterEnvControls->getControls()[C::DECAY_INDEX])
                     .get();
        attack = std::dynamic_pointer_cast<LawControl>(
                     ampEnvControls->getControls()[C::ATTACK_INDEX])
                     .get();
        hold = std::dynamic_pointer_cast<LawControl>(
                   ampEnvControls->getControls()[C::HOLD_INDEX])
                   .get();
        decay = std::dynamic_pointer_cast<LawControl>(
                    ampEnvControls->getControls()[C::DECAY_INDEX])
                    .get();
        reso = std::dynamic_pointer_cast<LawControl>(
                   svfControls->getControls()[C::RESO_INDEX])
                   .get();
    }
}

Voice::~Voice()
{
    delete staticEnvControls;
    delete staticEnv;

    if (!isBasicVoice)
    {
        delete ampEnvControls;
        delete filterEnvControls;
        delete ampEnv;
        delete filterEnv;
        delete svfControls;
        delete svfLeft;
        delete svfRight;
    }
}

void Voice::init(const int velocity, const std::shared_ptr<Sound> &sound,
                 const int noteNumber, NoteParameters *noteParameters,
                 const int varType, const int varValue, const int drumIndex,
                 const int frameOffset, const bool enableEnvs,
                 const int startTick, const float engineSampleRate,
                 const uint64_t noteEventId)
{
    VoiceState *state = getInactiveState();

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

    if (noteParameters != nullptr)
    {
        state->tune += noteParameters->getTune();
        state->veloToStart = noteParameters->getVelocityToStart();
        state->attackValue = noteParameters->getAttack();
        state->decayValue = noteParameters->getDecay();
        state->veloToAttack = noteParameters->getVelocityToAttack();
        state->decayMode = noteParameters->getDecayMode();
        state->veloToLevel = noteParameters->getVeloToLevel();
        state->voiceOverlapMode = sound->isLoopEnabled()
                                      ? VoiceOverlapMode::NOTE_OFF
                                      : noteParameters->getVoiceOverlapMode();
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

    const auto veloFactor = state->velocity / 127.f;
    const auto inverseVeloFactor = 1.f - veloFactor;

    state->start = sound->getStart();
    state->end = sound->getEnd();
    state->loopTo = sound->getLoopTo();
    state->lastFrameIndex = sound->getLastFrameIndex();
    state->loopEnabled = sound->isLoopEnabled();
    state->isMono = sound->isMono();
    state->sampleData = sound->getSampleData();

    state->position = sound->getStart() + inverseVeloFactor *
                                              (state->veloToStart / 100.0) *
                                              sound->getLastFrameIndex();
    state->attackMs = static_cast<float>(state->attackValue / 100.0 *
                                         C::MAX_ATTACK_LENGTH_MS);
    state->attackMs += static_cast<float>(state->veloToAttack / 100.0 *
                                          C::MAX_ATTACK_LENGTH_MS * veloFactor);
    state->finalDecayValue = state->decayValue < 2 ? 2 : state->decayValue;
    state->decayMs = static_cast<float>(state->finalDecayValue / 100.0 *
                                        C::MAX_DECAY_LENGTH_MS);
    staticEnv->reset();
    state->veloToLevelFactor = static_cast<float>(state->veloToLevel * 0.01);
    state->amplitude =
        veloFactor * state->veloToLevelFactor + 1.0f - state->veloToLevelFactor;
    state->amplitude *= sound->getSndLevel() * 0.01;

    if (!isBasicVoice)
    {
        assert(noteParameters);
        ampEnv->reset();
        state->filtParam = noteParameters->getFilterFrequency();

        if (state->varType == 3)
        {
            state->filtParam = state->varValue;
        }

        state->initialFilterValue =
            state->filtParam +
            veloFactor * noteParameters->getVelocityToFilterFrequency();
        state->initialFilterValue =
            static_cast<float>(17.0 + state->initialFilterValue * 0.75);
        filterEnv->reset();
        fattack->setValue(
            static_cast<float>(noteParameters->getFilterAttack() * 0.002) *
            C::MAX_ATTACK_LENGTH_SAMPLES);
        fhold->setValue(0);
        fdecay->setValue(
            static_cast<float>(noteParameters->getFilterDecay() * 0.002) *
            C::MAX_DECAY_LENGTH_SAMPLES);
        reso->setValue(static_cast<float>(
            0.0625 + noteParameters->getFilterResonance() / 26.0));
        svfLeft->update();
        svfRight->update();
    }

    state->decayCounter = 0;

    state->sampleRate = engineSampleRate;

    initializeSamplerateDependents();

    swapStates();
}

void Voice::initializeSamplerateDependents()
{
    VoiceState *state = getInactiveState();

    staticEnvControls->setSampleRate(state->sampleRate);

    if (!isBasicVoice)
    {
        ampEnvControls->setSampleRate(state->sampleRate);
        filterEnvControls->setSampleRate(state->sampleRate);
    }

    state->increment = pow(2.0, static_cast<double>(state->tune) / 120.0) *
                       (44100.0 / state->sampleRate);

    const auto veloFactor = 1.f - state->velocity / 127.f;
    const auto start = state->start + veloFactor *
                                          (state->veloToStart / 100.0) *
                                          state->lastFrameIndex;

    const auto playableSampleLength =
        state->loopEnabled
            ? INT_MAX
            : static_cast<int>((state->end - start) / state->increment);

    auto attackLengthSamples =
        static_cast<int>(state->attackMs * state->sampleRate * 0.001);
    auto decayLengthSamples =
        static_cast<int>(state->decayMs * state->sampleRate * 0.001);

    if (attackLengthSamples > C::MAX_ATTACK_LENGTH_SAMPLES)
    {
        attackLengthSamples = C::MAX_ATTACK_LENGTH_SAMPLES;
    }

    if (decayLengthSamples > C::MAX_DECAY_LENGTH_SAMPLES)
    {
        decayLengthSamples = C::MAX_DECAY_LENGTH_SAMPLES;
    }

    const auto staticEnvHoldSamples =
        static_cast<int>(playableSampleLength -
                         (C::STATIC_ATTACK_LENGTH + C::STATIC_DECAY_LENGTH) /
                             C::ENV_TIME_RATIO * state->sampleRate * 0.001);

    shold->setValue(staticEnvHoldSamples);

    if (!isBasicVoice)
    {
        attack->setValue(state->attackMs * C::ENV_TIME_RATIO);
        const auto holdLengthSamples =
            playableSampleLength - attackLengthSamples - decayLengthSamples;
        hold->setValue(state->decayMode == 1 ? 0 : holdLengthSamples);
        decay->setValue(state->decayMs * C::ENV_TIME_RATIO);
        state->inverseNyquist = VoiceUtil::getInverseNyquist(state->sampleRate);
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

    state->envAmplitude = isBasicVoice ? 1.0f : ampEnv->getEnvelope(false);
    state->staticEnvAmp =
        state->enableEnvs ? staticEnv->getEnvelope(state->staticDecay) : 1.0f;
    state->envAmplitude *= state->staticEnvAmp;

    float filterFreq = 0;

    if (!isBasicVoice)
    {
        filterFreq = VoiceUtil::midiFreq(state->initialFilterValue * 1.44f) *
                     state->inverseNyquist;
        const auto filterEnvFactor = static_cast<float>(
            filterEnv->getEnvelope(false) *
            (state->noteParameters->getFilterEnvelopeAmount() * 0.01));
        filterFreq +=
            VoiceUtil::midiFreq(144) * state->inverseNyquist * filterEnvFactor;
    }

    readFrame();

    if (state->isMono)
    {
        frame[0] *= state->envAmplitude * state->amplitude;

        if (!isBasicVoice)
        {
            frame[0] = svfLeft->filter(frame[0], filterFreq);
        }

        frame[1] = frame[0];
    }
    else
    {
        frame[0] *= state->envAmplitude * state->amplitude;
        frame[1] *= state->envAmplitude * state->amplitude;

        if (!isBasicVoice)
        {
            frame[0] = svfLeft->filter(frame[0], filterFreq);
            frame[1] = svfRight->filter(frame[1], filterFreq);
        }
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
        initializeSamplerateDependents();
    }

    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    const auto masterLevelToUse = masterLevel.load();

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
    masterLevel.store(masterLevelToUse);
}

int Voice::getNote() const
{
    const VoiceState *state = getActiveState();
    return state->noteNumber;
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

const NoteParameters *Voice::getNoteParameters() const
{
    const VoiceState *state = getActiveState();
    return state->noteParameters;
}
