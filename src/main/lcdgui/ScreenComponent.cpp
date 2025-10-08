#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>
#include "hardware/Hardware.hpp"
#include "hardware/TopPanel.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::controls;

ScreenComponent::ScreenComponent(mpc::Mpc& mpc, const std::string& name, const int layer)
	: Component(name), layer(layer), mpc(mpc), ls(mpc.getLayeredScreen()), sampler(mpc.getSampler()), sequencer(mpc.getSequencer())
{
	auto background = std::dynamic_pointer_cast<Background>(addChild(std::make_shared<Background>()));
	background->setName(name);
}

void ScreenComponent::setTransferMap(const std::map<std::string, std::vector<std::string>>& newTransferMap)
{
	transferMap = newTransferMap;
}

std::map<std::string, std::vector<std::string>>& ScreenComponent::getTransferMap()
{
	return transferMap;
}

void ScreenComponent::setFirstField(const std::string& newFirstField)
{
	firstField = newFirstField;
}

std::string ScreenComponent::getFirstField()
{
	return firstField;
}

std::shared_ptr<Wave> ScreenComponent::findWave()
{
	return findChild<Wave>("wave");
}

std::shared_ptr<EnvGraph> ScreenComponent::findEnvGraph()
{
	return findChild<EnvGraph>("env-graph");
}

const int& ScreenComponent::getLayerIndex()
{
	return layer;
}

std::shared_ptr<Field> ScreenComponent::findFocus()
{
	for (auto& field : findFields())
	{
		if (field->hasFocus())
			return field;
	}
	return {};
}

void ScreenComponent::openScreen(const std::string& screenName)
{
	mpc.getLayeredScreen()->openScreen(screenName);
}

void ScreenComponent::setLastFocus(const std::string& screenName, const std::string& newLastFocus)
{
	mpc.getLayeredScreen()->setLastFocus(screenName, newLastFocus);
}

const std::string ScreenComponent::getLastFocus(const std::string& screenName)
{
	return mpc.getLayeredScreen()->getLastFocus(screenName);
}

void ScreenComponent::openWindow()
{
    auto layeredScreen = mpc.getLayeredScreen();

    if (layeredScreen->getScreenToReturnToWhenPressingOpenWindow().empty() ||
        name == "midi-output-monitor")
    {
        return;
    }

    layeredScreen->openScreen(layeredScreen->getScreenToReturnToWhenPressingOpenWindow());
    layeredScreen->clearScreenToReturnToWhenPressingOpenWindow();
}

void ScreenComponent::pad(int padIndexWithBank, int velo)
{
    const auto currentScreenName = name;
    const bool isSamplerScreen = std::find(BaseControls::samplerScreens.begin(),
                                           BaseControls::samplerScreens.end(),
                                           currentScreenName) != BaseControls::samplerScreens.end();

    const bool isSoundScreen = std::find(BaseControls::soundScreens.begin(),
                                         BaseControls::soundScreens.end(),
                                         currentScreenName) != BaseControls::soundScreens.end();

    const bool allowCentralNoteAndPadUpdate = std::find(BaseControls::allowCentralNoteAndPadUpdateScreens.begin(),
                                                        BaseControls::allowCentralNoteAndPadUpdateScreens.end(),
                                                        currentScreenName) != BaseControls::allowCentralNoteAndPadUpdateScreens.end();
    
    const bool isFullLevelEnabled = mpc.getHardware()->getTopPanel()->isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = mpc.getHardware()->getTopPanel()->isSixteenLevelsEnabled();
    const bool isTapPressed = mpc.getControls()->isTapPressed();
    const bool isNoteRepeatLocked = mpc.getControls()->isNoteRepeatLocked();
    const bool isErasePressed = mpc.getControls()->isErasePressed();
    const bool isStepRecording = mpc.getControls()->isStepRecording();
    const bool isRecMainWithoutPlaying = mpc.getControls()->isRecMainWithoutPlaying();
    auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct");
    auto assign16LevelsScreen = mpc.screens->get<mpc::lcdgui::screens::window::Assign16LevelsScreen>("assign-16-levels");

    auto activeTrack = mpc.getSequencer()->getActiveTrack();

    std::shared_ptr<mpc::sampler::Program> program;
    mpc::engine::Drum *activeDrum = nullptr;

    const auto drumScreen = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum");
    
    const auto drumIndex = isSamplerScreen ? drumScreen->getDrum() : track->getBus() - 1;

    int programNote = -1;
    
    if (drumIndex != -1)
    {
        activeDrum = &mpc.getDrum(drumIndex);
        program = sampler->getProgram(activeDrum->getProgram());
        programNote = program->getPad(padIndexWithBank)->getNote();
    }

    std::function<void(int)> setMpcNote = [mpc = &mpc] (int n) { mpc->setNote(n); };
    std::function<void(int)> setMpcPad = [mpc = &mpc] (int p) { mpc->setPad(p); };

    PadPushContext ctx {
        currentScreenName,
            isSoundScreen,
            isFullLevelEnabled,
            isSixteenLevelsEnabled,
            isTapPressed,
            isNoteRepeatLocked,
            isErasePressed,
            isStepRecording,
            isRecMainWithoutPlaying,
            mpc.getBank(),
            mpc.getSequencer()->isPlaying(),
            mpc.getSequencer()->isRecordingOrOverdubbing(),
            mpc.getSequencer()->getCurrentBarIndex(),
            mpc.getSequencer()->getTickPosition(),
            currentScreenName == "song",
            mpc.getAudioMidiServices()->getFrameSequencer()->getMetronomeOnlyTickPosition(),
            timingCorrectScreen->getNoteValueLengthInTicks(),
            timingCorrectScreen->getSwing(),
            activeTrack->getBus(),
            program,
            programNote,
            sampler->getSoundIndex(),
            drumScreen->getDrum(),
            isSamplerScreen,
            activeTrack,
            sampler,
            mpc.getSequencer(),
            mpc.getHardware(),
            mpc.getAudioMidiServices(),
            mpc.getControls(),
            mpc.getLayeredScreen(),
            timingCorrectScreen,
            assign16LevelsScreen,
            mpc.getEventHandler(),
            mpc.getAudioMidiServices()->getFrameSequencer(),
            mpc.getBasicPlayer(),
            allowCentralNoteAndPadUpdate,
            mpc.getActiveControls(),
            setMpcNote,
            setMpcPad,
            mpc.getLayeredScreen()->getFocus()}; 

    mpc::controls::BaseControls::pad(ctx, padIndexWithBank, velo);
}

mpc::engine::Drum& ScreenComponent::activeDrum()
{
    const auto drumScreen = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum");

    const bool isSamplerScreen = std::find(BaseControls::samplerScreens.begin(),
                                           BaseControls::samplerScreens.end(),
                                           name) != BaseControls::samplerScreens.end();
    
    auto drumIndex = isSamplerScreen ? (int) (drumScreen->getDrum()) : mpc.getSequencer()->getActiveTrack()->getBus() - 1;

    // Should not happen (TM)
    // In all seriousness though, we should return a shared_ptr or optional here, and consumers should check the result.
    // For now we always return a valid drum
    if (drumIndex < 0) drumIndex = 0;
    
    return mpc.getDrum(drumIndex);
}

