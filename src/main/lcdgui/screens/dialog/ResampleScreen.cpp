#include "ResampleScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <thirdp/libsamplerate/samplerate.h>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ResampleScreen::ResampleScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "resample", layerIndex)
{
}

void ResampleScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName.compare("name") != 0 && sampler.lock()->getSound().lock())
	{
		newName = sampler.lock()->getSound().lock()->getName();
		newName = sampler.lock()->addOrIncreaseNumber(newName);
	}

	if (sampler.lock()->getSound().lock())
	{
		setNewFs(sampler.lock()->getSound().lock()->getSampleRate());
	}

	displayNewBit();
	displayNewFs();
	displayNewName();
	displayQuality();
}

void ResampleScreen::turnWheel(int i)
{
	init();

	if (param.compare("newfs") == 0)
	{
		setNewFs(newFs + i);
	}
	else if (param.compare("newbit") == 0)
	{
		setNewBit(newBit + i);
	}
	else if (param.compare("quality") == 0)
	{
		setQuality(quality + i);
	}
	else if (param.compare("newname") == 0)
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
        auto resampleScreen = this;
        auto _sampler = sampler.lock();
		nameScreen->setName(findField("newname").lock()->getText());

        auto renamer = [_sampler, resampleScreen](string& newName) {
            if (_sampler->isSoundNameOccupied(newName))
                return;

            resampleScreen->setNewName(newName);
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
		auto snd = sampler.lock()->getSound(sampler.lock()->getSoundIndex()).lock();
		auto destSnd = sampler.lock()->addSound().lock();
		destSnd->setName(newName);

		auto source = snd->getSampleData();

		if (newFs != snd->getSampleRate())
		{
			float* srcArray = &(*source)[0];

			SRC_DATA srcData;
			srcData.data_in = srcArray;
			srcData.input_frames = source->size();
			srcData.src_ratio = (double)(newFs) / (double)(snd->getSampleRate());
            
			srcData.output_frames = (floor)(source->size() * srcData.src_ratio);

			auto dest = destSnd->getSampleData();
			dest->resize(srcData.output_frames);

			float* destArray = &(*dest)[0];
			srcData.data_out = destArray;

			auto error = src_simple(&srcData, 0, 1);

			if (error != 0)
			{
				const char* errormsg = src_strerror(error);
				string errorStr(errormsg);
				MLOG("libsamplerate error: " + errorStr);
			}
		}
		else
		{
			*destSnd->getSampleData() = *source;
		}

		for (auto& f : *destSnd->getSampleData())
		{
			if (f > 1)
			{
				f = 1;
			}
			else if (f < -1)
			{
				f = -1;
			}
		}

		destSnd->setSampleRate(newFs);
		destSnd->setMono(snd->isMono());
		destSnd->setName(newName);

		if (newBit == 1)
		{
			sampler.lock()->process12Bit(destSnd->getSampleData());
		}
		else if (newBit == 2)
		{
			sampler.lock()->process8Bit(destSnd->getSampleData());
		}

		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		openScreen("sound");
		break;
	}
	}
}

void ResampleScreen::displayNewFs()
{
	findField("newfs").lock()->setText(to_string(newFs));
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

void ResampleScreen::setNewName(string s)
{
	newName = s;
	displayNewName();
}
