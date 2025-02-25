#include "StereoToMonoScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;

StereoToMonoScreen::StereoToMonoScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "stereo-to-mono", layerIndex)
{
}

void StereoToMonoScreen::open()
{
	auto previousScreenName = ls->getPreviousScreenName();

	if (previousScreenName != "name" && previousScreenName != "popup")
	{
		updateNewNames();
		ls->setFocus("stereosource");
	}
	
	displayNewLName();
	displayNewRName();
	displayStereoSource();
}

void StereoToMonoScreen::turnWheel(int i)
{
	init();

	if (param == "stereosource")
	{
		sampler->nudgeSoundIndex(i > 0);
		displayStereoSource();
	}
}

void StereoToMonoScreen::openNameScreen()
{
    if (param == "newlname" || param == "newrname")
    {
        const auto isL = param == "newlname";

        const auto enterAction = [this, isL](std::string& nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            if (isL) setNewLName(nameScreenName);
            else setNewRName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto newName = isL ? newLName : newRName;
        nameScreen->initialize(newName, 16, enterAction, name);
        openScreen("name");
    }
}

void StereoToMonoScreen::function(int i)
{
	init();
		
	switch (i)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
	{
		auto sound = sampler->getSound();

		if (sound->isMono())
			return;

		for (auto& s : sampler->getSounds())
		{
			if (s->getName() == newLName || s->getName() == newRName)
			{
				auto popupScreen = mpc.screens->get<PopupScreen>("popup");
				popupScreen->setText("Name already used");
				popupScreen->returnToScreenAfterInteraction(name);
				openScreen("popup");
				return;
			}
		}

		auto left = sampler->addSound(sound->getSampleRate(), "stereo-to-mono");

        if (left == nullptr)
        {
            return;
        }

		auto right = sampler->addSound(sound->getSampleRate(), "stereo-to-mono");

        if (right == nullptr)
        {
            sampler->deleteSound(left);
            return;
        }

		left->setName(newLName);
		right->setName(newRName);

		left->setMono(true);
		right->setMono(true);
		
		auto leftData = left->getSampleData();
		auto rightData = right->getSampleData();

		for (int frameIndex = 0; frameIndex < sound->getFrameCount(); frameIndex++)
		{
			leftData->push_back((*sound->getSampleData())[frameIndex]);
			rightData->push_back((*sound->getSampleData())[frameIndex + sound->getFrameCount()]);
		}
		
		left->setEnd(left->getSampleData()->size());
		right->setEnd(right->getSampleData()->size());
		openScreen("sound");
		
		break;
	}
	}
}

void StereoToMonoScreen::updateNewNames()
{
	if (! sampler->getSound() || sampler->getSound()->isMono())
		return;

	auto name = sampler->getSound()->getName();
	name = StrUtil::trim(name);
	name = StrUtil::padRight(name, "_", 16);
	name = name.substr(0, 14);

	setNewLName(name + "-L");
	setNewRName(name + "-R");
}

void StereoToMonoScreen::displayStereoSource()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		return;
	}

	findField("stereosource")->setText(sound->getName());

	if (sound->isMono())
	{
		ls->setFunctionKeysArrangement(1);
		findBackground()->repaintUnobtrusive(findChild<FunctionKey>("fk4")->getRect());
	}
	else
	{
		ls->setFunctionKeysArrangement(0);
	}
}

void StereoToMonoScreen::displayNewLName()
{
	findField("newlname")->setText(newLName);
}

void StereoToMonoScreen::displayNewRName()
{
	findField("newrname")->setText(newRName);
}

void StereoToMonoScreen::setNewLName(std::string s)
{
	newLName = s;
	displayNewLName();
}

void StereoToMonoScreen::setNewRName(std::string s)
{
	newRName = s;
	displayNewRName();
}
