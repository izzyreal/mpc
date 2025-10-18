#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>

#include "command/context/PushPadScreenUpdateContext.h"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Track.hpp"

#include "lcdgui/ScreenGroups.h"
#include "lcdgui/screens/DrumScreen.hpp"

#include "command/context/TriggerDrumContextFactory.h"

using namespace mpc::lcdgui;
using namespace mpc::command;
using namespace mpc::command::context;

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

void ScreenComponent::pad(int programPadIndex, int velo)
{
    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(mpc, programPadIndex, velo, name);
    TriggerDrumNoteOnCommand(ctx, programPadIndex, velo).execute();

    PushPadScreenUpdateContext padPushScreenUpdateCtx {
        ctx.currentScreenName,
        ctx.isSixteenLevelsEnabled,
        mpc::sequencer::isDrumNote(ctx.note),
        ctx.allowCentralNoteAndPadUpdate,
        ctx.screenComponent, ctx.setMpcNote,
        ctx.setMpcPad,
        ctx.currentFieldName,
        ctx.bank
    };

    PushPadScreenUpdateCommand(padPushScreenUpdateCtx, ctx.note, programPadIndex).execute();

}

mpc::engine::Drum& ScreenComponent::activeDrum()
{
    const auto drumScreen = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum");

    const bool isSamplerScreen = screengroups::isSamplerScreen(name);
    auto drumIndex = isSamplerScreen ? (int) (drumScreen->getDrum()) : mpc.getSequencer()->getActiveTrack()->getBus() - 1;

    // Should not happen (TM)
    // In all seriousness though, we should return a shared_ptr or optional here, and consumers should check the result.
    // For now we always return a valid drum
    if (drumIndex < 0) drumIndex = 0;
    
    return mpc.getDrum(drumIndex);
}

