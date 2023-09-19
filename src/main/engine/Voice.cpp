#include "Voice.hpp"

#include "sampler/NoteParameters.hpp"
#include <sampler/Sound.hpp>

#include "EnvelopeControls.hpp"
#include "EnvelopeGenerator.hpp"\

#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/filter/StateVariableFilter.hpp"
#include "engine/filter/StateVariableFilterControls.hpp"

#include <cmath>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::engine;

std::vector<float> Voice::EMPTY_FRAME = {0.f, 0.f};

Voice::Voice(int _stripNumber, bool _basic)
        : stripNumber(_stripNumber), basic(_basic), frame(EMPTY_FRAME) {
    tempFrame = EMPTY_FRAME;
    staticEnvControls = new mpc::engine::EnvelopeControls(0, "StaticAmpEnv", AMPENV_OFFSET);
    staticEnv = new mpc::engine::EnvelopeGenerator(staticEnvControls);
    shold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            staticEnvControls->getControls()[HOLD_INDEX]).get();

    auto sattack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            staticEnvControls->getControls()[ATTACK_INDEX]).get();


    auto sdecay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            staticEnvControls->getControls()[DECAY_INDEX]).get();

    sattack->setValue(STATIC_ATTACK_LENGTH);
    sdecay->setValue(STATIC_DECAY_LENGTH);

    if (!basic) {
        ampEnvControls = new mpc::engine::EnvelopeControls(0, "AmpEnv", AMPENV_OFFSET);
        filterEnvControls = new mpc::engine::EnvelopeControls(0, "StaticAmpEnv", AMPENV_OFFSET);
        ampEnv = new mpc::engine::EnvelopeGenerator(ampEnvControls);
        filterEnv = new mpc::engine::EnvelopeGenerator(filterEnvControls);
        svfControls = new mpc::engine::filter::StateVariableFilterControls("Filter", SVF_OFFSET);
        svfControls->createControls();
        svfLeft = new mpc::engine::filter::StateVariableFilter(svfControls);
        svfRight = new mpc::engine::filter::StateVariableFilter(svfControls);
        fattack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                filterEnvControls->getControls()[ATTACK_INDEX]).get();
        fhold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                filterEnvControls->getControls()[HOLD_INDEX]).get();
        fdecay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                filterEnvControls->getControls()[DECAY_INDEX]).get();
        attack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                ampEnvControls->getControls()[ATTACK_INDEX]).get();
        hold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                ampEnvControls->getControls()[HOLD_INDEX]).get();
        decay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                ampEnvControls->getControls()[DECAY_INDEX]).get();
        reso = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(svfControls->getControls()[RESO_INDEX]).get();
    }
}

void Voice::init(
        int newVelocity,
        std::shared_ptr<mpc::sampler::Sound> newMpcSound,
        int newNote,
        mpc::sampler::NoteParameters *np,
        int newVarType,
        int newVarValue,
        int muteNote,
        int muteDrum,
        int newFrameOffset,
        bool newEnableEnvs,
        int newStartTick,
        int newDuration
) {
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

    if (np != nullptr) {
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
    sampleData = mpcSound->getSampleData();
    attackMs = (float) ((attackValue / 100.0) * MAX_ATTACK_LENGTH_MS);
    attackMs += (float) ((veloToAttack / 100.0) * MAX_ATTACK_LENGTH_MS * veloFactor);
    finalDecayValue = decayValue < 2 ? 2 : decayValue;
    decayMs = (float) ((finalDecayValue / 100.0) * MAX_DECAY_LENGTH_MS);
    staticEnv->reset();
    veloToLevelFactor = (float) (veloToLevel * 0.01);
    amplitude = (float) ((veloFactor * veloToLevelFactor) + 1.0f - veloToLevelFactor);
    amplitude *= (mpcSound->getSndLevel() * 0.01);

    if (!basic) {
        ampEnv->reset();
        filtParam = np->getFilterFrequency();

        if (varType == 3) {
            filtParam = varValue;
        }

        initialFilterValue = (float) (filtParam + (veloFactor * np->getVelocityToFilterFrequency()));
        initialFilterValue = (float) (17.0 + (initialFilterValue * 0.75));
        filterEnv->reset();
        fattack->setValue((float) (np->getFilterAttack() * 0.002) * MAX_ATTACK_LENGTH_SAMPLES);
        fhold->setValue(0);
        fdecay->setValue((float) (np->getFilterDecay() * 0.002) * MAX_DECAY_LENGTH_SAMPLES);
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

    if (!basic) {
        ampEnvControls->setSampleRate(sampleRate);
        filterEnvControls->setSampleRate(sampleRate);
    }

    increment = pow(2.0, ((double) (tune) / 120.0)) * (44100.0 / sampleRate);

    const auto veloFactor = 1.f - (velocity / 127.f);
    auto start = mpcSound->getStart() + (veloFactor * (veloToStart / 100.0) * mpcSound->getLastFrameIndex());

    auto playableSampleLength = mpcSound->isLoopEnabled() ? INT_MAX : (int) ((end - start) / increment);

    auto attackLengthSamples = (int) (attackMs * sampleRate * 0.001);
    auto decayLengthSamples = (int) (decayMs * sampleRate * 0.001);

    if (attackLengthSamples > MAX_ATTACK_LENGTH_SAMPLES) {
        attackLengthSamples = (int) (MAX_ATTACK_LENGTH_SAMPLES);
    }

    if (decayLengthSamples > MAX_DECAY_LENGTH_SAMPLES) {
        decayLengthSamples = MAX_DECAY_LENGTH_SAMPLES;
    }

    auto timeRatio = 5.46f;
    auto staticEnvHoldSamples = (int) (playableSampleLength - ((STATIC_ATTACK_LENGTH + STATIC_DECAY_LENGTH) / timeRatio) * (sampleRate) * 0.001);

    if (duration != -1 && duration < playableSampleLength)
    {
        staticEnvHoldSamples = (int) (duration - ((STATIC_ATTACK_LENGTH + STATIC_DECAY_LENGTH) / timeRatio) * (sampleRate) * 0.001);
    }

    shold->setValue(staticEnvHoldSamples);

    if (!basic) {
        auto holdLengthSamples = playableSampleLength - attackLengthSamples - decayLengthSamples;
        attack->setValue(decayMode == 1 ? (float) (0) : attackMs * timeRatio);
        hold->setValue(decayMode == 1 ? 0 : holdLengthSamples);
        decay->setValue(decayMs * timeRatio);
    }
}

std::vector<float>& Voice::getFrame() {
    if (finished)
        return EMPTY_FRAME;

    if (frameOffset > 0) {
        frameOffset--;
        return EMPTY_FRAME;
    }

    envAmplitude = basic ? 1.0f : ampEnv->getEnvelope(false);
    staticEnvAmp = enableEnvs ? staticEnv->getEnvelope(staticDecay) : 1.0f;
    envAmplitude *= staticEnvAmp;

    float filterFreq = 0;

    if (!basic) {
        auto invNyquist = getInverseNyquist(sampleRate);
        filterFreq = midiFreq(initialFilterValue * 1.44f) * invNyquist;
        auto filterEnvFactor = (float) (filterEnv->getEnvelope(false) * (noteParameters->getFilterEnvelopeAmount() * 0.01));
        filterFreq += midiFreq(144) * invNyquist * filterEnvFactor;
    }

    readFrame();

    if (mpcSound->isMono()) {
        tempFrame[0] *= envAmplitude * amplitude;

        if (!basic)
            tempFrame[0] = svfLeft->filter(tempFrame[0], filterFreq);

        tempFrame[1] = tempFrame[0];
    } else {
        tempFrame[0] *= envAmplitude * amplitude;
        tempFrame[1] *= envAmplitude * amplitude;

        if (!basic) {
            tempFrame[0] = svfLeft->filter(tempFrame[0], filterFreq);
            tempFrame[1] = svfRight->filter(tempFrame[1], filterFreq);
        }
    }

    return tempFrame;
}

void Voice::readFrame() {
    if (mpcSound->isLoopEnabled() && position > end - 1)
        position = mpcSound->getLoopTo();

    if ( ( (position >= (end - 1)) && !mpcSound->isLoopEnabled() ) || (staticEnv != nullptr && staticEnv->isComplete()) ||
         (ampEnv != nullptr && ampEnv->isComplete())) {
        tempFrame = EMPTY_FRAME;
        finished = true;
        return;
    }

    k = (int) (ceil(position));
    j = k - 1;

    if (j == -1)
        j = 0;

    frac = position - (double) (j);

    if (mpcSound->isMono()) {
        tempFrame[0] = ((*sampleData)[j] * (1.0f - frac)) + ((*sampleData)[k] * frac);
    } else {
        auto rOffset = sampleData->size() * 0.5;
        tempFrame[0] = ((*sampleData)[j] * (1.0f - frac)) + ((*sampleData)[k] * frac);
        tempFrame[1] = ((*sampleData)[j + rOffset] * (1.0f - frac)) + ((*sampleData)[k + rOffset] * frac);
    }

    position += increment;
}

void Voice::open() {
}

int Voice::processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) {
    if (finished) {
        buffer->makeSilence();
        return AUDIO_SILENCE;
    }

    if (buffer->getSampleRate() != sampleRate) {
        sampleRate = buffer->getSampleRate();
        initializeSamplerateDependents();
    }

    auto& left = buffer->getChannel(0);
    auto& right = buffer->getChannel(1);

    auto masterLevelToUse = masterLevel.load();

    auto masterLevelFactor = masterLevelToUse > -128 ? std::pow (10.f, static_cast<float>(masterLevelToUse) * 0.04f)
                                                     : 0;

    for (int i = 0; i < nFrames; i++) {
        frame = getFrame();

        left[i] = frame[0] * masterLevelFactor;
        right[i] = frame[1] * masterLevelFactor;

        if (decayCounter != 0) {
            if (decayCounter == 1)
            {
                startDecay();
            }

            decayCounter--;
        }
    }

    if (finished) {
        note = -1;
        mpcSound = {};
    }

    return AUDIO_OK;
}

bool Voice::isFinished() {
    return finished;
}

void Voice::close() {
}

void Voice::finish() {
    finished = true;
}

void Voice::startDecay() {
    staticDecay = true;
}

int Voice::getVoiceOverlap() {
    return voiceOverlapMode;
}

int Voice::getStripNumber() {
    return stripNumber;
}

int Voice::getStartTick()
{
    return startTick;
}

bool Voice::isDecaying() {
    return staticDecay;
}

MuteInfo& Voice::getMuteInfo() {
    return muteInfo;
}

void Voice::startDecay(int offset) {
    if (offset > 0)
        decayCounter = offset;
    else
        startDecay();
}

int Voice::getNote() {
    return note;
}

mpc::sampler::NoteParameters *Voice::getNoteParameters() {
    return noteParameters;
}

void Voice::setMasterLevel(int8_t masterLevelToUse)
{
    masterLevel.store(masterLevelToUse);
}

std::vector<float>& Voice::freqTable()
{
    static std::vector<float> res;
    if (res.empty()) {
        for (auto i = 0; i < 140; i++) {
            res.push_back(midiFreqImpl(i));
        }
    }
    return res;
}

float Voice::midiFreq(float pitch)
{
    if (pitch < 0)
        return freqTable()[0];

    if (pitch >= (int)(freqTable().size()) - 1)
        return freqTable()[(int)(freqTable().size()) - 2];

    auto idx = (int)(pitch);
    auto frac = pitch - idx;
    return freqTable()[idx] * (1 - frac) + freqTable()[idx + 1] * frac;
}

float Voice::midiFreqImpl(int pitch)
{
    return (float)(440.0 * pow(2.0, ((double)(pitch) - 69.0) / 12.0));
}

Voice::~Voice() {
    delete staticEnvControls;
    delete staticEnv;

    if (!basic) {
        delete ampEnvControls;
        delete filterEnvControls;
        delete ampEnv;
        delete filterEnv;
        delete svfControls;
        delete svfLeft;
        delete svfRight;
    }
}
