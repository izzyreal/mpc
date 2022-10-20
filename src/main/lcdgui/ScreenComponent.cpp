#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace std;

ScreenComponent::ScreenComponent(mpc::Mpc& mpc, const string& name, const int layer)
	: Component(name), layer(layer), mpc(mpc)
{
	ls = mpc.getLayeredScreen();
	sampler = mpc.getSampler().lock();
	sequencer = mpc.getSequencer().lock();
	auto background = dynamic_pointer_cast<Background>(addChild(make_shared<Background>()).lock());
	background->setName(name);
}

void ScreenComponent::setTransferMap(const map<string, vector<string>>& newTransferMap)
{
	transferMap = newTransferMap;
}

map<string, vector<string>>& ScreenComponent::getTransferMap()
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

weak_ptr<Wave> ScreenComponent::findWave()
{
	return dynamic_pointer_cast<Wave>(findChild("wave").lock());
}

weak_ptr<EnvGraph> ScreenComponent::findEnvGraph()
{
	return dynamic_pointer_cast<EnvGraph>(findChild("env-graph").lock());
}

const int& ScreenComponent::getLayerIndex()
{
	return layer;
}

weak_ptr<Field> ScreenComponent::findFocus()
{
	for (auto& field : findFields())
	{
		if (field.lock()->hasFocus())
			return field;
	}
	return {};
}

void ScreenComponent::openScreen(const string& screenName)
{
	mpc.getLayeredScreen().lock()->openScreen(screenName);
}

void ScreenComponent::setLastFocus(const string& screenName, const string& newLastFocus)
{
	mpc.getLayeredScreen().lock()->setLastFocus(screenName, newLastFocus);
}

const string ScreenComponent::getLastFocus(const string& screenName)
{
	return mpc.getLayeredScreen().lock()->getLastFocus(screenName);
}
