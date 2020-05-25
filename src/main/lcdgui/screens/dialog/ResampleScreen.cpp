#include "ResampleScreen.hpp"

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <thirdp/libsamplerate/samplerate.h>

#include <cmath>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

ResampleScreen::ResampleScreen(const int layerIndex)
	: ScreenComponent("resample", layerIndex)
{
}

void ResampleScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName.compare("name") != 0)
	{
		newName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newName = sampler.lock()->addOrIncreaseNumber(newName);
	}

	setNewFs(sampler.lock()->getSound().lock()->getSampleRate());
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
		nameGui->setName(ls.lock()->lookupField("newname").lock()->getText());
		nameGui->setParameterName("newname");
		ls.lock()->openScreen("name");
	}
}

void ResampleScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		auto snd = sampler.lock()->getSound(sampler.lock()->getSoundIndex()).lock();
		auto destSnd = sampler.lock()->addSound().lock();
		destSnd->setName(newName);

		auto source = snd->getSampleData();

		if (newFs != dynamic_pointer_cast<mpc::sampler::Sound>(snd)->getSampleRate())
		{
			float* srcArray = &(*source)[0];

			SRC_DATA srcData;
			srcData.data_in = srcArray;
			srcData.input_frames = source->size();
			srcData.src_ratio = (double)(newFs) / (double)(dynamic_pointer_cast<mpc::sampler::Sound>(snd)->getSampleRate());
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
		ls.lock()->openScreen("sound");
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
	auto soundGui = mpc.getUis().lock()->getSoundGui();
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
