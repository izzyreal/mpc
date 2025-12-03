#include "DrumNoteEventContextBuilder.hpp"
#include "Mpc.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sequencer/Bus.hpp"
#include "engine/audio/mixer/AudioMixer.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::sequencer;
using namespace mpc::engine::audio::mixer;
using namespace mpc::sampler;

DrumNoteOnContext DrumNoteEventContextBuilder::buildDrumNoteOnContext(
    const uint64_t noteEventId, const performance::Drum &drum,
    const std::shared_ptr<DrumBus> &drumBus,
    const std::shared_ptr<Sampler> &sampler,
    const std::shared_ptr<AudioMixer> &mixer,
    const std::shared_ptr<MixerSetupScreen> &mixerSetupScreen,
    std::vector<std::shared_ptr<Voice>> *voices,
    std::vector<MixerInterconnection *> &mixerConnections, const int note,
    const int velocity, const int varType, const int varValue,
    const int frameOffset, const bool firstGeneration, const int startTick,
    const int durationFrames)
{
    DrumNoteOnContext ctx;
    ctx.noteEventId = noteEventId;
    ctx.drumBus = drumBus;
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
    ctx.setOptA = [drumBus](const int n1, const int n2)
    {
        drumBus->getSimultA()[n1] = n2;
    };
    ctx.setOptB = [drumBus](const int n1, const int n2)
    {
        drumBus->getSimultB()[n1] = n2;
    };
    return ctx;
}

DrumNoteOffContext DrumNoteEventContextBuilder::buildDrumNoteOffContext(
    const uint64_t noteEventId, const std::shared_ptr<DrumBus> &drum,
    std::vector<std::shared_ptr<Voice>> *voices, const int note,
    const int noteOnStartTick)
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
