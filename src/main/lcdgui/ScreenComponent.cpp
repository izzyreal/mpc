#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>

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
