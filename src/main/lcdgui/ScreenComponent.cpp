#include "ScreenComponent.hpp"
#include "Background.hpp"

using namespace mpc::lcdgui;

using namespace std;

ScreenComponent::ScreenComponent(const string& name)
	: Component(name)
{
	auto background = dynamic_pointer_cast<Background>(addChild(make_shared<Background>()).lock());
	background->setName(name);
}
