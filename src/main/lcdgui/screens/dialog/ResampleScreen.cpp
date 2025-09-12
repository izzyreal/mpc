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
	const auto previousScreenName = ls->getPreviousScreenName();

	if (previousScreenName != "name" && sampler->getSound())
	{
		newName = sampler->getSound()->getName();
		newName = sampler->addOrIncreaseNumber(newName);
		setNewFs(sampler->getSound()->getSampleRate());
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
}

void ResampleScreen::openNameScreen()
{
    init();

    if (param == "newname")
    {
        const auto enterAction = [this](std::string& nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            setNewName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->initialize(findField("newname")->getText(), 16, enterAction, name);
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
		const auto snd = sampler->getSound(sampler->getSoundIndex());
		auto destSnd = sampler->addSound("sound");

        if (destSnd == nullptr)
        {
            return;
        }

		destSnd->setName(newName);
        destSnd->setSampleRate(newFs);
        destSnd->setMono(snd->isMono());

		const auto source = snd->getSampleData();

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

        const auto ratio = newFs / (float) snd->getSampleRate();

        destSnd->setStart(snd->getStart() * ratio);
        destSnd->setEnd(snd->getEnd() * ratio);
        destSnd->setLoopTo(snd->getLoopTo() * ratio);

		sampler->setSoundIndex(sampler->getSoundCount() - 1);
		openScreen("sound");
		break;
	}
	}
}

void ResampleScreen::displayNewFs()
{
	findField("newfs")->setText(std::to_string(newFs));
}

void ResampleScreen::displayQuality()
{
	findField("quality")->setText(qualityNames[quality]);
}

void ResampleScreen::displayNewBit()
{
	findField("newbit")->setText(bitNames[newBit]);
}

void ResampleScreen::displayNewName()
{
	findField("newname")->setText(newName);
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
