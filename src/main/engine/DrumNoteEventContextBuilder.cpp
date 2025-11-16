#include "DrumNoteEventContextBuilder.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sequencer/Bus.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::sequencer;
using namespace mpc::engine::audio::mixer;
using namespace mpc::sampler;

DrumNoteOnContext DrumNoteEventContextBuilder::buildDrumNoteOnContext(
    uint64_t noteEventId, const std::shared_ptr<DrumBus> &drum,
    const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<AudioMixer> &mixer,
    const std::shared_ptr<MixerSetupScreen> &mixerSetupScreen,
    std::vector<std::shared_ptr<Voice>> *voices,
    std::vector<MixerInterconnection *> &mixerConnections, int note,
    int velocity, int varType, int varValue, int frameOffset,
    bool firstGeneration, int startTick, int durationFrames)
{
    DrumNoteOnContext ctx;
    ctx.noteEventId = noteEventId;
    ctx.drum = drum;
    ctx.sampler = sampler;
    ctx.mixer = mixer;
    ctx.mixerSetupScreen = mixerSetupScreen;
    ctx.voices = voices;
    ctx.mixerConnections = &mixerConnections;
    ctx.note = DrumNoteNumber(note);
    ctx.velocity = velocity;
    ctx.varType = varType;
    ctx.varValue = varValue;
    ctx.frameOffset = frameOffset;
    ctx.firstGeneration = firstGeneration;
    ctx.startTick = startTick;
    ctx.durationFrames = durationFrames;
    return ctx;
}

DrumNoteOffContext DrumNoteEventContextBuilder::buildDrumNoteOffContext(
    uint64_t noteEventId, const std::shared_ptr<DrumBus> &drum,
    std::vector<std::shared_ptr<Voice>> *voices, int note, int noteOnStartTick)
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
