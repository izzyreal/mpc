#include "Voice.hpp"

#include "VoiceConstants.hpp"

#include "sampler/NoteParameters.hpp"
#include <sampler/Sound.hpp>

#include "EnvelopeControls.hpp"
#include "EnvelopeGenerator.hpp"

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/filter/StateVariableFilter.hpp"
#include "engine/filter/StateVariableFilterControls.hpp"

#include <cmath>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::engine;
using namespace mpc::engine::audio::core;
using namespace mpc::engine::control;
using namespace mpc::engine::filter;
using namespace mpc::sampler;

typedef mpc::engine::VoiceConstants C;

Voice::Voice(const int stripNumberToUse, const bool basicToUse)
    : stripNumber(stripNumberToUse), basic(basicToUse), frame(C::EMPTY_FRAME)
{
    frame = C::EMPTY_FRAME;

    staticEnvControls = new EnvelopeControls(0, "StaticAmpEnv", C::AMPENV_OFFSET);
    staticEnv = new EnvelopeGenerator(staticEnvControls);
    shold = std::dynamic_pointer_cast<LawControl>(staticEnvControls->getControls()[C::HOLD_INDEX]).get();

    auto sattack = std::dynamic_pointer_cast<LawControl>(staticEnvControls->getControls()[C::ATTACK_INDEX]).get();
    auto sdecay = std::dynamic_pointer_cast<LawControl>(staticEnvControls->getControls()[C::DECAY_INDEX]).get();

    sattack->setValue(C::STATIC_ATTACK_LENGTH);
    sdecay->setValue(C::STATIC_DECAY_LENGTH);

    if (!basic)
    {
        ampEnvControls = new EnvelopeControls(0, "AmpEnv", C::AMPENV_OFFSET);
        filterEnvControls = new EnvelopeControls(0, "StaticAmpEnv", C::AMPENV_OFFSET);
        ampEnv = new EnvelopeGenerator(ampEnvControls);
        filterEnv = new EnvelopeGenerator(filterEnvControls);
        svfControls = new StateVariableFilterControls("Filter", C::SVF_OFFSET);
        svfControls->createControls();
        svfLeft = new StateVariableFilter(svfControls);
        svfRight = new StateVariableFilter(svfControls);
        fattack = std::dynamic_pointer_cast<LawControl>(filterEnvControls->getControls()[C::ATTACK_INDEX]).get();
        fhold = std::dynamic_pointer_cast<LawControl>(filterEnvControls->getControls()[C::HOLD_INDEX]).get();
        fdecay = std::dynamic_pointer_cast<LawControl>(filterEnvControls->getControls()[C::DECAY_INDEX]).get();
        attack = std::dynamic_pointer_cast<LawControl>(ampEnvControls->getControls()[C::ATTACK_INDEX]).get();
        hold = std::dynamic_pointer_cast<LawControl>(ampEnvControls->getControls()[C::HOLD_INDEX]).get();
        decay = std::dynamic_pointer_cast<LawControl>(ampEnvControls->getControls()[C::DECAY_INDEX]).get();
        reso = std::dynamic_pointer_cast<LawControl>(svfControls->getControls()[C::RESO_INDEX]).get();
    }
}

Voice::~Voice()
{
    delete staticEnvControls;
    delete staticEnv;

    if (!basic)
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

void Voice::init(
        int newVelocity,
        std::shared_ptr<Sound> newMpcSound,
        int newNote,
        NoteParameters *np,
        int newVarType,
        int newVarValue,
        int muteNote,
        int muteDrum,
        int newFrameOffset,
        bool newEnableEnvs,
        int newStartTick,
        int newDuration)
{
    finished = false;

    duration = newDuration;

    noteParameters = np;
    startTick = newStartTick;

    enableEnvs = newEnableEnvs;
    frameOffset = newFrameOffset;
    note = newNote;
    velocity = newVelocity;
    mpcSound = newMpcSound;
    varType = newVarType;
    varValue = newVarValue;

    staticDecay = false;
    muteInfo.setNote(muteNote);
    muteInfo.setDrum(muteDrum);
    veloToStart = 0;
    attackValue = 0;
    decayValue = 2;
    veloToAttack = 0;
    decayMode = 0;
    veloToLevel = 100;

    tune = mpcSound->getTune();

    if (np != nullptr)
    {
        tune += np->getTune();
        veloToStart = np->getVelocityToStart();
        attackValue = np->getAttack();
        decayValue = np->getDecay();
        veloToAttack = np->getVelocityToAttack();
        decayMode = np->getDecayMode();
        veloToLevel = np->getVeloToLevel();
        voiceOverlapMode = mpcSound->isLoopEnabled() ? 2 : np->getVoiceOverlap();
    }

    switch (varType) {
        case 0:
            tune += (varValue - 64) * 2;
            break;
        case 1:
            decayValue = varValue;
            decayMode = 1;
            break;
        case 2:
            attackValue = varValue;
            break;
    }

    const auto veloFactor = velocity / 127.f;
    const auto inverseVeloFactor = 1.f - veloFactor;

    end = mpcSound->getEnd();

    position = mpcSound->getStart() + (inverseVeloFactor * (veloToStart / 100.0) * mpcSound->getLastFrameIndex());
    attackMs = (float) ((attackValue / 100.0) * C::MAX_ATTACK_LENGTH_MS);
    attackMs += (float) ((veloToAttack / 100.0) * C::MAX_ATTACK_LENGTH_MS * veloFactor);
    finalDecayValue = decayValue < 2 ? 2 : decayValue;
    decayMs = (float) ((finalDecayValue / 100.0) * C::MAX_DECAY_LENGTH_MS);
    staticEnv->reset();
    veloToLevelFactor = (float) (veloToLevel * 0.01);
    amplitude = (float) ((veloFactor * veloToLevelFactor) + 1.0f - veloToLevelFactor);
    amplitude *= mpcSound->getSndLevel() * 0.01;

    if (!basic)
    {
        ampEnv->reset();
        filtParam = np->getFilterFrequency();

        if (varType == 3)
        {
            filtParam = varValue;
        }

        initialFilterValue = (float) (filtParam + (veloFactor * np->getVelocityToFilterFrequency()));
        initialFilterValue = (float) (17.0 + (initialFilterValue * 0.75));
        filterEnv->reset();
        fattack->setValue((float) (np->getFilterAttack() * 0.002) * C::MAX_ATTACK_LENGTH_SAMPLES);
        fhold->setValue(0);
        fdecay->setValue((float) (np->getFilterDecay() * 0.002) * C::MAX_DECAY_LENGTH_SAMPLES);
        reso->setValue((float) (0.0625 + (np->getFilterResonance() / 26.0)));
        svfLeft->update();
        svfRight->update();
    }

    decayCounter = 0;

    initializeSamplerateDependents();
}

void Voice::initializeSamplerateDependents()
{
    staticEnvControls->setSampleRate(sampleRate);

    if (!basic)
    {
        ampEnvControls->setSampleRate(sampleRate);
        filterEnvControls->setSampleRate(sampleRate);
    }

    increment = pow(2.0, ((double) (tune) / 120.0)) * (44100.0 / sampleRate);

    const auto veloFactor = 1.f - (velocity / 127.f);
    const auto start = mpcSound->getStart() + (veloFactor * (veloToStart / 100.0) * mpcSound->getLastFrameIndex());

    const auto playableSampleLength = mpcSound->isLoopEnabled() ? INT_MAX : (int) ((end - start) / increment);

    auto attackLengthSamples = (int) (attackMs * sampleRate * 0.001);
    auto decayLengthSamples = (int) (decayMs * sampleRate * 0.001);

    if (attackLengthSamples > C::MAX_ATTACK_LENGTH_SAMPLES)
    {
        attackLengthSamples = C::MAX_ATTACK_LENGTH_SAMPLES;
    }

    if (decayLengthSamples > C::MAX_DECAY_LENGTH_SAMPLES)
    {
        decayLengthSamples = C::MAX_DECAY_LENGTH_SAMPLES;
    }

    auto staticEnvHoldSamples = (int) (playableSampleLength - ((C::STATIC_ATTACK_LENGTH + C::STATIC_DECAY_LENGTH) / C::ENV_TIME_RATIO) * (sampleRate) * 0.001);

    if (duration != -1 && duration < playableSampleLength)
    {
        staticEnvHoldSamples = (int) (duration - ((C::STATIC_ATTACK_LENGTH + C::STATIC_DECAY_LENGTH) / C::ENV_TIME_RATIO) * (sampleRate) * 0.001);
    }

    shold->setValue(staticEnvHoldSamples);

    if (!basic)
    {
        attack->setValue(attackMs * C::ENV_TIME_RATIO);
        const auto holdLengthSamples = playableSampleLength - attackLengthSamples - decayLengthSamples;
        hold->setValue(decayMode == 1 ? 0 : holdLengthSamples);
        decay->setValue(decayMs * C::ENV_TIME_RATIO);
        inverseNyquist = VoiceUtil::getInverseNyquist(sampleRate);
    }
}

const std::vector<float>& Voice::getFrame()
{
    if (finished)
    {
        return C::EMPTY_FRAME;
    }

    if (frameOffset > 0)
    {
        frameOffset--;
        return C::EMPTY_FRAME;
    }

    envAmplitude = basic ? 1.0f : ampEnv->getEnvelope(false);
    staticEnvAmp = enableEnvs ? staticEnv->getEnvelope(staticDecay) : 1.0f;
    envAmplitude *= staticEnvAmp;

    float filterFreq = 0;

    if (!basic)
    {
        filterFreq = VoiceUtil::midiFreq(initialFilterValue * 1.44f) * inverseNyquist;
        const auto filterEnvFactor = (float) (filterEnv->getEnvelope(false) * (noteParameters->getFilterEnvelopeAmount() * 0.01));
        filterFreq += VoiceUtil::midiFreq(144) * inverseNyquist * filterEnvFactor;
    }

    readFrame();

    if (mpcSound->isMono())
    {
        frame[0] *= envAmplitude * amplitude;

        if (!basic)
        {
            frame[0] = svfLeft->filter(frame[0], filterFreq);
        }

        frame[1] = frame[0];
    }
    else
    {
        frame[0] *= envAmplitude * amplitude;
        frame[1] *= envAmplitude * amplitude;

        if (!basic)
        {
            frame[0] = svfLeft->filter(frame[0], filterFreq);
            frame[1] = svfRight->filter(frame[1], filterFreq);
        }
    }

    return frame;
}

void Voice::readFrame()
{
    if (mpcSound->isLoopEnabled() && position > end - 1)
    {
        position = mpcSound->getLoopTo();
    }

    if (((position >= (end - 1)) && !mpcSound->isLoopEnabled()) ||
        (staticEnv != nullptr && staticEnv->isComplete()) ||
        (ampEnv != nullptr && ampEnv->isComplete()))
    {
        frame = C::EMPTY_FRAME;
        finished = true;
        return;
    }

    const auto k = (int) (ceil(position));
    const auto j = k != 0 ? k - 1 : 0;
    const auto frac = position - (double) (j);
    const auto &sampleData = *mpcSound->getSampleData();

    if (mpcSound->isMono())
    {
        frame[0] = (sampleData[j] * (1.0f - frac)) + (sampleData[k] * frac);
    }
    else
    {
        frame[0] = (sampleData[j] * (1.0f - frac)) + (sampleData[k] * frac);
        const auto rOffset = sampleData.size() * 0.5;
        frame[1] = (sampleData[j + rOffset] * (1.0f - frac)) + (sampleData[k + rOffset] * frac);
    }

    position += increment;
}

int Voice::processAudio(AudioBuffer *buffer, int nFrames)
{
    if (finished)
    {
        buffer->makeSilence();
        return AUDIO_SILENCE;
    }

    if (buffer->getSampleRate() != sampleRate)
    {
        sampleRate = buffer->getSampleRate();
        initializeSamplerateDependents();
    }

    auto &left = buffer->getChannel(0);
    auto &right = buffer->getChannel(1);

    const auto masterLevelToUse = masterLevel.load();

    const auto masterLevelFactor = masterLevelToUse > -128 ? std::pow (10.f, static_cast<float>(masterLevelToUse) * 0.04f)
                                                     : 0;

    for (int i = 0; i < nFrames; i++)
    {
        auto &frameToUse = getFrame();

        left[i] = frameToUse[0] * masterLevelFactor;
        right[i] = frameToUse[1] * masterLevelFactor;

        if (decayCounter != 0)
        {
            if (decayCounter == 1)
            {
                startDecay();
            }

            decayCounter--;
        }
    }

    if (finished)
    {
        note = -1;
        mpcSound = {};
    }

    return AUDIO_OK;
}

void Voice::finish()
{
    finished = true;
}

void Voice::startDecay()
{
    staticDecay = true;
}

void Voice::startDecay(const int offset)
{
    if (offset > 0)
    {
        decayCounter = offset;
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
    return note;
}

bool Voice::isFinished() const
{
    return finished;
}

int Voice::getVoiceOverlap() const
{
    return voiceOverlapMode;
}

int Voice::getStripNumber() const
{
    return stripNumber;
}

int Voice::getStartTick() const
{
    return startTick;
}

bool Voice::isDecaying() const
{
    return staticDecay;
}

const MuteInfo& Voice::getMuteInfo() const
{
    return muteInfo;
}

const NoteParameters* Voice::getNoteParameters() const
{
    return noteParameters;
}

