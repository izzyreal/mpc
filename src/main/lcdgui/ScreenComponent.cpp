#include "ScreenComponent.hpp"

#include "Background.hpp"
#
#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui;

using namespace std;

ScreenComponent::ScreenComponent(const string& name, const int layer)
	: Component(name), layer(layer)
{
	auto background = dynamic_pointer_cast<Background>(addChild(make_shared<Background>()).lock());
	background->setName(name);
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
