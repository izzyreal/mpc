#include "ScreenComponent.hpp"

#include "Background.hpp"

#include <Mpc.hpp>

using namespace mpc::lcdgui;
using namespace std;

ScreenComponent::ScreenComponent(const string& name, const int layer)
	: Component(name), layer(layer)
{
	auto background = dynamic_pointer_cast<Background>(addChild(make_shared<Background>()).lock());
	background->setName(name);
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
	mpc::Mpc::instance().getLayeredScreen().lock()->openScreen(screenName);
}

void ScreenComponent::setLastFocus(const string& screenName, const string& newLastFocus)
{
	mpc::Mpc::instance().getLayeredScreen().lock()->setLastFocus(screenName, newLastFocus);
}

const string ScreenComponent::getLastFocus(const string& screenName)
{
	return mpc::Mpc::instance().getLayeredScreen().lock()->getLastFocus(screenName);
}

weak_ptr<Background> ScreenComponent::findBackground()
{
	return findChild<Background>("");
}
