#include "DrumNoteEventContextBuilder.hpp"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "engine/Drum.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::sampler;

DrumNoteOnContext DrumNoteEventContextBuilder::buildNoteOn(
    uint64_t noteEventId,
    Drum *drum,
    std::shared_ptr<Sampler> sampler,
    std::shared_ptr<AudioMixer> mixer,
    std::shared_ptr<MixerSetupScreen> mixerSetupScreen,
    std::vector<std::shared_ptr<Voice>> *voices,
    std::vector<MixerInterconnection*> &mixerConnections,
    int note,
    int velocity,
    int varType,
    int varValue,
    int frameOffset,
    bool firstGeneration,
    int startTick,
    int durationFrames
)
{
    DrumNoteOnContext ctx;
    ctx.noteEventId = noteEventId;
    ctx.drum = drum;
    ctx.sampler = sampler;
    ctx.mixer = mixer;
    ctx.mixerSetupScreen = mixerSetupScreen;
    ctx.voices = voices;
    ctx.mixerConnections = &mixerConnections,
    ctx.note = note;
    ctx.velocity = velocity;
    ctx.varType = varType;
    ctx.varValue = varValue;
    ctx.frameOffset = frameOffset;
    ctx.firstGeneration = firstGeneration;
    ctx.startTick = startTick;
    ctx.durationFrames = durationFrames;
    return ctx;
}

DrumNoteOffContext DrumNoteEventContextBuilder::buildNoteOff(
    uint64_t noteEventId,
    Drum *drum,
    std::vector<std::shared_ptr<Voice>> *voices,
    int note,
    int noteOnStartTick
)
{
    DrumNoteOffContext ctx;
    ctx.noteEventId = noteEventId;
    ctx.drumIndex = drum->getIndex();
    ctx.drumSimultA = &drum->getSimultA();
    ctx.drumSimultB = &drum->getSimultB();
    ctx.voices = voices;
    ctx.note = note;
    ctx.noteOnStartTick = noteOnStartTick;
    return ctx;
}

