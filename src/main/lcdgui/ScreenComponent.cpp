#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace std;

ScreenComponent::ScreenComponent(mpc::Mpc& mpc, const string& name, const int layer)
	: Component(name), layer(layer), mpc(mpc)
{
	baseControls = make_shared<BaseControls>(mpc);
	ls = mpc.getLayeredScreen();
	sampler = mpc.getSampler();
	sequencer = mpc.getSequencer();
	auto background = dynamic_pointer_cast<Background>(addChild(make_shared<Background>()).lock());
	background->setName(name);
}

weak_ptr<Component> ScreenComponent::addChild(shared_ptr<Component> child)
{
	auto background = findBackground().lock();
	
	if (background)
	{
		return background->addChild(child);
	}

	return Component::addChild(child);
}

void ScreenComponent::setTransferMap(const map<string, vector<string>>& transferMap)
{
	this->transferMap = transferMap;
}

const map<string, vector<string>>& ScreenComponent::getTransferMap()
{
	return transferMap;
}

void ScreenComponent::setFirstField(const std::string& firstField)
{
	this->firstField = firstField;
}

std::string ScreenComponent::getFirstField()
{
	return firstField;
}

weak_ptr<Wave> ScreenComponent::findWave()
{
	return dynamic_pointer_cast<Wave>(findChild("wave").lock());
}

weak_ptr<TwoDots> ScreenComponent::findTwoDots()
{
	return dynamic_pointer_cast<TwoDots>(findChild("two-dots").lock());
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
		{
			return field;
		}
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

weak_ptr<Background> ScreenComponent::findBackground()
{
	return findChild<Background>("");
}
