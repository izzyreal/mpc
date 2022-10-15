#include "MonoToStereoScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

MonoToStereoScreen::MonoToStereoScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "mono-to-stereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
	auto prevScreen = ls.lock()->getPreviousScreenName();

	if (sampler->getSound().lock() && prevScreen != "name" && prevScreen != "popup")
	{
		string name = sampler->getSound().lock()->getName();
		name = StrUtil::trim(name);
		name = StrUtil::padRight(name, "_", 16);
		name = name.substr(0, 14);
		newStName = name + "-S";
	}

	if (prevScreen != "name" && prevScreen != "popup")
		ls.lock()->setFocus("lsource");

	setRSource(sampler->getSoundIndex());
	displayLSource();
	displayNewStName();
}

void MonoToStereoScreen::turnWheel(int i)
{
	init();
	
	if (param == "lsource" && i < 0)
	{
		sampler->selectPreviousSound();
		displayLSource();

	}
	else if (param == "lsource" && i > 0)
	{
		sampler->selectNextSound();
		displayLSource();
	}
	else if (param == "rsource")
	{
		setRSource(sampler->getNextSoundIndex(rSource, i > 0));
		displayRSource();
	}
	else if (param == "newstname")
	{
		const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto monoToStereoScreen = this;
		nameScreen->setName(newStName);
        
        auto renamer = [monoToStereoScreen](string& newName) {
            monoToStereoScreen->newStName = newName;
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "mono-to-stereo");
		openScreen("name");
	}
}

void MonoToStereoScreen::function(int j)
{
	init();
	
	switch (j)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
	{
		if (!sampler->getSound().lock()->isMono() || !sampler->getSound(rSource).lock()->isMono())
			return;

		for (auto& s : sampler->getSounds())
		{
			if (s.lock()->getName() == newStName)
			{
				auto popupScreen = mpc.screens->get<PopupScreen>("popup");
				popupScreen->setText("Name already used");
				popupScreen->returnToScreenAfterInteraction(name);
				openScreen("popup");
				return;
			}
		}

		auto left = sampler->getSound().lock();
		auto right = sampler->getSound(rSource).lock();

		vector<float> newSampleDataRight;

		if (right->getSampleRate() > left->getSampleRate())
		{
			newSampleDataRight = vector<float>(left->getSampleData()->size());

			for (int i = 0; i < newSampleDataRight.size(); i++)
				newSampleDataRight[i] = (*right->getSampleData())[i];
		}
		else
		{
			newSampleDataRight = *right->getSampleData();
		}

		auto newSound = sampler->addSound(left->getSampleRate()).lock();
		newSound->setName(newStName);
		sampler->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
        newSound->setMono(false);
		openScreen("sound");
	}
	}
}

void MonoToStereoScreen::displayLSource()
{
	if (!sampler->getSound().lock())
		return;

	findField("lsource").lock()->setText(sampler->getSound().lock()->getName());

	if (sampler->getSound().lock()->isMono() && sampler->getSound().lock()->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(0);
	}
	else
	{
		ls.lock()->setFunctionKeysArrangement(1);
		findChild<Background>("").lock()->repaintUnobtrusive(findChild<FunctionKey>("fk4").lock()->getRect());
	}
}

void MonoToStereoScreen::displayRSource()
{
	if (!sampler->getSound(rSource).lock())
		return;

	findField("rsource").lock()->setText(sampler->getSoundName(rSource));

	if (sampler->getSound().lock()->isMono() && sampler->getSound(rSource).lock()->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(0);
	}
	else
	{
		ls.lock()->setFunctionKeysArrangement(1);
		findChild<Background>("").lock()->repaintUnobtrusive(findChild<FunctionKey>("fk4").lock()->getRect());
	}
}

void MonoToStereoScreen::displayNewStName()
{
	findField("newstname").lock()->setText(newStName);
}

void MonoToStereoScreen::setRSource(int i)
{
	if (i < 0 || i >= sampler->getSoundCount())
		return;

	rSource = i;
	displayRSource();
}
