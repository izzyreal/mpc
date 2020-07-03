#include "Popup.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

using namespace mpc::lcdgui;
using namespace std;

Popup::Popup()
	: Component("popup")
{
	addChild(make_unique<Background>());
	findChild<Background>("background").lock()->setName("popup");

	addChild(make_shared<Label>("popup", "", 43, 23, 0));
	findChild<Label>("popup").lock()->setInverted(true);

	setSize(184, 17);
	setLocation(34, 18);
}

void Popup::setText(string text)
{
	findChild<Label>("popup").lock()->setText(text);
}

void Popup::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}

	Component::Draw(pixels);
}
