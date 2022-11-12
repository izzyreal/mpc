#include "ResampleScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

ResampleScreen::ResampleScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "resample", layerIndex)
{
}

void ResampleScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName != "name" && sampler->getSound().lock())
	{
		newName = sampler->getSound().lock()->getName();
		newName = sampler->addOrIncreaseNumber(newName);
	}

	if (sampler->getSound().lock())
	{
		setNewFs(sampler->getSound().lock()->getSampleRate());
	}

	displayNewBit();
	displayNewFs();
	displayNewName();
	displayQuality();
}

void ResampleScreen::turnWheel(int i)
{
	init();

	if (param == "newfs")
	{
		setNewFs(newFs + i);
	}
	else if (param == "newbit")
	{
		setNewBit(newBit + i);
	}
	else if (param == "quality")
	{
		setQuality(quality + i);
	}
	else if (param == "newname")
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
        auto resampleScreen = this;
        auto _sampler = sampler;
		nameScreen->setName(findField("newname").lock()->getText());

        auto renamer = [_sampler, resampleScreen](std::string& newName2) {
            if (_sampler->isSoundNameOccupied(newName2))
                return;

            resampleScreen->setNewName(newName2);
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "resample");
		openScreen("name");
	}
}

void ResampleScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
	{
		auto snd = sampler->getSound(sampler->getSoundIndex()).lock();
		auto destSnd = sampler->addSound().lock();
		destSnd->setName(newName);
        destSnd->setSampleRate(newFs);

		auto source = snd->getSampleData();

		if (newFs != snd->getSampleRate())
		{
            sampler::Sampler::resample(*source, snd->getSampleRate(), destSnd);
        }
		else
		{
			*destSnd->getSampleData() = *source;
		}

		for (auto& f : *destSnd->getSampleData())
		{
			if (f > 1) f = 1;
			else if (f < -1) f = -1;
		}

		destSnd->setMono(snd->isMono());
		destSnd->setName(newName);
        const int diff = newFs - snd->getSampleRate();
        int newTuning = static_cast<int>(diff * (120.f / newFs));
        if (newTuning < -120) newTuning = -120;
        else if (newTuning > 120) newTuning = 120;
        destSnd->setTune(newTuning);

		if (newBit == 1)
		{
			sampler::Sampler::process12Bit(destSnd->getSampleData());
		}
		else if (newBit == 2)
		{
            sampler::Sampler::process8Bit(destSnd->getSampleData());
		}

		sampler->setSoundIndex(sampler->getSoundCount() - 1);
		openScreen("sound");
		break;
	}
	}
}

void ResampleScreen::displayNewFs()
{
	findField("newfs").lock()->setText(std::to_string(newFs));
}

void ResampleScreen::displayQuality()
{
	findField("quality").lock()->setText(qualityNames[quality]);
}

void ResampleScreen::displayNewBit()
{
	findField("newbit").lock()->setText(bitNames[newBit]);
}

void ResampleScreen::displayNewName()
{
	findField("newname").lock()->setText(newName);
}

void ResampleScreen::setNewFs(int i)
{
	if (i < 4000 || i > 65000)
	{
		return;
	}

	newFs = i;
	displayNewFs();
}

void ResampleScreen::setQuality(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	quality = i;
	displayQuality();
}

void ResampleScreen::setNewBit(int i)
{
	if (i < 0 || i > 2)
	{
		return;
	}

	newBit = i;
	displayNewBit();
}

void ResampleScreen::setNewName(std::string s)
{
	newName = s;
	displayNewName();
}
