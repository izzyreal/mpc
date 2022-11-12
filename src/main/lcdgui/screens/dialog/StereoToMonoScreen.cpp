#include "StereoToMonoScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

StereoToMonoScreen::StereoToMonoScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "stereo-to-mono", layerIndex)
{
}

void StereoToMonoScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName != "name" && previousScreenName != "popup")
	{
		updateNewNames();
		ls.lock()->setFocus("stereosource");
	}
	
	displayNewLName();
	displayNewRName();
	displayStereoSource();
}

void StereoToMonoScreen::turnWheel(int i)
{
	init();

	auto nameScreen = mpc.screens->get<NameScreen>("name");

	if (param == "stereosource")
	{
		sampler->setSoundIndex(sampler->getNextSoundIndex(sampler->getSoundIndex(), i > 0));
		displayStereoSource();
	}
	else if (param == "newlname" || param == "newrname")
	{
        const auto stereoToMonoScreen = this;
        const auto isL = param == "newlname";
        
        nameScreen->setName(isL ? newLName : newRName);
        auto renamer = [isL, stereoToMonoScreen](std::string& newName) {
            if (isL) stereoToMonoScreen->setNewLName(newName);
            else stereoToMonoScreen->setNewRName(newName);
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "stereo-to-mono");
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
		auto sound = sampler->getSound().lock();

		if (sound->isMono())
			return;

		for (auto& s : sampler->getSounds())
		{
			if (s.lock()->getName() == newLName || s.lock()->getName() == newRName)
			{
				auto popupScreen = mpc.screens->get<PopupScreen>("popup");
				popupScreen->setText("Name already used");
				popupScreen->returnToScreenAfterInteraction(name);
				openScreen("popup");
				return;
			}
		}

		auto left = sampler->addSound(sound->getSampleRate()).lock();
		auto right = sampler->addSound(sound->getSampleRate()).lock();

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
	if (! sampler->getSound().lock() || sampler->getSound().lock()->isMono())
		return;

	auto name = sampler->getSound().lock()->getName();
	name = StrUtil::trim(name);
	name = StrUtil::padRight(name, "_", 16);
	name = name.substr(0, 14);

	setNewLName(name + "-L");
	setNewRName(name + "-R");
}

void StereoToMonoScreen::displayStereoSource()
{
	auto sound = sampler->getSound().lock();

	if (!sound)
	{
		return;
	}

	findField("stereosource")->setText(sound->getName());

	if (sound->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(1);
		findBackground()->repaintUnobtrusive(findChild<FunctionKey>("fk4")->getRect());
	}
	else
	{
		ls.lock()->setFunctionKeysArrangement(0);
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
