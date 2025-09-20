#include "Voice.hpp"

#include "sampler/NoteParameters.hpp"
#include <sampler/Sound.hpp>

#include "EnvelopeControls.hpp"
#include "EnvelopeGenerator.hpp"
#include "engine/audio/core/AudioBuffer.hpp"
#include "engine/control/LawControl.hpp"
#include "engine/filter/StateVariableFilter.hpp"
#include "engine/filter/StateVariableFilterControls.hpp"

#include <cmath>
#include <atomic>

#ifdef __linux__
#include <climits>
#endif

using namespace mpc::engine;

std::vector<float> Voice::EMPTY_FRAME = {0.f, 0.f};

Voice::Voice(int _stripNumber, bool _basic) {
    auto s = std::make_shared<VoiceState>();
    s->stripNumber = _stripNumber;
    s->basic = _basic;
    s->frame = EMPTY_FRAME;
    s->tempFrame = EMPTY_FRAME;
    s->staticEnvControls = new mpc::engine::EnvelopeControls(0, "StaticAmpEnv", AMPENV_OFFSET);
    s->staticEnv = new mpc::engine::EnvelopeGenerator(s->staticEnvControls);
    s->shold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            s->staticEnvControls->getControls()[HOLD_INDEX]).get();

    auto sattack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            s->staticEnvControls->getControls()[ATTACK_INDEX]).get();
    auto sdecay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
            s->staticEnvControls->getControls()[DECAY_INDEX]).get();

    sattack->setValue(STATIC_ATTACK_LENGTH);
    sdecay->setValue(STATIC_DECAY_LENGTH);

    if (!s->basic) {
        s->ampEnvControls = new mpc::engine::EnvelopeControls(0, "AmpEnv", AMPENV_OFFSET);
        s->filterEnvControls = new mpc::engine::EnvelopeControls(0, "StaticAmpEnv", AMPENV_OFFSET);
        s->ampEnv = new mpc::engine::EnvelopeGenerator(s->ampEnvControls);
        s->filterEnv = new mpc::engine::EnvelopeGenerator(s->filterEnvControls);
        s->svfControls = new mpc::engine::filter::StateVariableFilterControls("Filter", SVF_OFFSET);
        s->svfControls->createControls();
        s->svfLeft = new mpc::engine::filter::StateVariableFilter(s->svfControls);
        s->svfRight = new mpc::engine::filter::StateVariableFilter(s->svfControls);
        s->fattack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->filterEnvControls->getControls()[ATTACK_INDEX]).get();
        s->fhold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->filterEnvControls->getControls()[HOLD_INDEX]).get();
        s->fdecay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->filterEnvControls->getControls()[DECAY_INDEX]).get();
        s->attack = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->ampEnvControls->getControls()[ATTACK_INDEX]).get();
        s->hold = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->ampEnvControls->getControls()[HOLD_INDEX]).get();
        s->decay = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(
                s->ampEnvControls->getControls()[DECAY_INDEX]).get();
        s->reso = std::dynamic_pointer_cast<mpc::engine::control::LawControl>(s->svfControls->getControls()[RESO_INDEX]).get();
    }

    std::atomic_store_explicit(&currentState, s, std::memory_order_release);
}

void Voice::init(int velocity,
                 std::shared_ptr<mpc::sampler::Sound> mpcSound,
                 int note,
                 mpc::sampler::NoteParameters *np,
                 int varType,
                 int varValue,
                 int muteNote,
                 int muteDrum,
                 int frameOffset,
                 bool enableEnvs,
                 int startTick,
                 int newDuration) {
    auto s = std::make_shared<VoiceState>(*std::atomic_load_explicit(&currentState, std::memory_order_acquire));
    s->finished = false;
    s->duration = newDuration;
    s->noteParameters = np;
    s->startTick = startTick;
    s->enableEnvs = enableEnvs;
    s->frameOffset = frameOffset;
    s->note = note;
    s->velocity = velocity;
    s->mpcSound = mpcSound;
    s->varType = varType;
    s->varValue = varValue;
    s->staticDecay = false;
    s->muteInfo.setNote(muteNote);
    s->muteInfo.setDrum(muteDrum);
    s->veloToStart = 0;
    s->attackValue = 0;
    s->decayValue = 2;
    s->veloToAttack = 0;
    s->decayMode = 0;
    s->veloToLevel = 100;
    s->tune = mpcSound->getTune();
    if (np != nullptr) {
        s->tune += np->getTune();
        s->veloToStart = np->getVelocityToStart();
        s->attackValue = np->getAttack();
        s->decayValue = np->getDecay();
        s->veloToAttack = np->getVelocityToAttack();
        s->decayMode = np->getDecayMode();
        s->veloToLevel = np->getVeloToLevel();
        s->voiceOverlapMode = mpcSound->isLoopEnabled() ? 2 : np->getVoiceOverlap();
    }
    switch (varType) {
        case 0: s->tune += (varValue - 64) * 2; break;
        case 1: s->decayValue = varValue; s->decayMode = 1; break;
        case 2: s->attackValue = varValue; break;
    }
    const auto veloFactor = velocity / 127.f;
    const auto inverseVeloFactor = 1.f - veloFactor;
    s->end = mpcSound->getEnd();
    s->position = mpcSound->getStart() + (inverseVeloFactor * (s->veloToStart / 100.0) * mpcSound->getLastFrameIndex());
    s->sampleData = mpcSound->getSampleData();
    s->attackMs = (float)((s->attackValue / 100.0) * MAX_ATTACK_LENGTH_MS);
    s->attackMs += (float)((s->veloToAttack / 100.0) * MAX_ATTACK_LENGTH_MS * veloFactor);
    s->finalDecayValue = s->decayValue < 2 ? 2 : s->decayValue;
    s->decayMs = (float)((s->finalDecayValue / 100.0) * MAX_DECAY_LENGTH_MS);
    s->staticEnv->reset();
    s->veloToLevelFactor = (float)(s->veloToLevel * 0.01);
    s->amplitude = (float)((veloFactor * s->veloToLevelFactor) + 1.0f - s->veloToLevelFactor);
    s->amplitude *= (mpcSound->getSndLevel() * 0.01);
    if (!s->basic) {
        s->ampEnv->reset();
        s->filtParam = np->getFilterFrequency();
        if (varType == 3) {
            s->filtParam = varValue;
        }
        s->initialFilterValue = (float)(s->filtParam + (veloFactor * np->getVelocityToFilterFrequency()));
        s->initialFilterValue = (float)(17.0 + (s->initialFilterValue * 0.75));
        s->filterEnv->reset();
        s->fattack->setValue((float)(np->getFilterAttack() * 0.002) * MAX_ATTACK_LENGTH_SAMPLES);
        s->fhold->setValue(0);
        s->fdecay->setValue((float)(np->getFilterDecay() * 0.002) * MAX_DECAY_LENGTH_SAMPLES);
        s->reso->setValue((float)(0.0625 + (np->getFilterResonance() / 26.0)));
        s->svfLeft->update();
        s->svfRight->update();
    }
    s->decayCounter = 0;
    std::atomic_store_explicit(&currentState, s, std::memory_order_release);
}

void Voice::open() {}
void Voice::close() {}

int Voice::processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    if (s->finished) {
        buffer->makeSilence();
        return AUDIO_SILENCE;
    }
    if (buffer->getSampleRate() != sampleRate) {
        sampleRate = buffer->getSampleRate();
    }
    auto& left = buffer->getChannel(0);
    auto& right = buffer->getChannel(1);
    auto masterLevelToUse = masterLevel.load();
    auto masterLevelFactor = masterLevelToUse > -128 ? std::pow (10.f, static_cast<float>(masterLevelToUse) * 0.04f) : 0;
    for (int i = 0; i < nFrames; i++) {
        auto& frame = getFrame();
        left[i] = frame[0] * masterLevelFactor;
        right[i] = frame[1] * masterLevelFactor;
    }
    if (s->finished) {
        s->note = -1;
        s->mpcSound = {};
    }
    return AUDIO_OK;
}

bool Voice::isFinished() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->finished;
}

void Voice::finish() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    s->finished = true;
}

void Voice::setMasterLevel(int8_t masterLevelToUse) {
    masterLevel.store(masterLevelToUse);
}

std::vector<float>& Voice::getFrame() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->frame.empty() ? EMPTY_FRAME : s->frame;
}

int Voice::getNote() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->note;
}

mpc::sampler::NoteParameters *Voice::getNoteParameters() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->noteParameters;
}

int Voice::getVoiceOverlap() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->voiceOverlapMode;
}

int Voice::getStripNumber() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->stripNumber;
}

int Voice::getStartTick() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->startTick;
}

bool Voice::isDecaying() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->staticDecay;
}

MuteInfo& Voice::getMuteInfo() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    return s->muteInfo;
}

void Voice::startDecay() {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    s->staticDecay = true;
}

void Voice::startDecay(int offset) {
    auto s = std::atomic_load_explicit(&currentState, std::memory_order_acquire);
    if (offset > 0)
        s->decayCounter = offset;
    else
        s->staticDecay = true;
}

std::vector<float>& Voice::freqTable() {
    static std::vector<float> res;
    if (res.empty()) {
        for (auto i = 0; i < 140; i++) {
            res.push_back(midiFreqImpl(i));
        }
    }
    return res;
}

float Voice::midiFreq(float pitch) {
    if (pitch < 0) return freqTable()[0];
    if (pitch >= (int)(freqTable().size()) - 1)
        return freqTable()[(int)(freqTable().size()) - 2];
    auto idx = (int)(pitch);
    auto frac = pitch - idx;
    return freqTable()[idx] * (1 - frac) + freqTable()[idx + 1] * frac;
}

float Voice::midiFreqImpl(int pitch) {
    return (float)(440.0 * pow(2.0, ((double)(pitch) - 69.0) / 12.0));
}

