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
	displayNewBit();
	displayNewFs();
	displayNewName();
	displayQuality();
}

void ResampleScreen::turnWheel(int i)
{
	init();

	auto soundGui = mpc.getUis().lock()->getSoundGui();

	if (param.compare("newfs") == 0)
	{
		soundGui->setNewFs(soundGui->getNewFs() + i);
		displayNewFs();
	}
	else if (param.compare("newbit") == 0)
	{
		soundGui->setNewBit(soundGui->getNewBit() + i);
		displayNewBit();
	}
	else if (param.compare("quality") == 0)
	{
		soundGui->setQuality(soundGui->getQuality() + i);
		displayQuality();
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
		destSnd->setName(soundGui->getNewName());

		auto source = snd->getSampleData();

		if (soundGui->getNewFs() != dynamic_pointer_cast<mpc::sampler::Sound>(snd)->getSampleRate())
		{
			float* srcArray = &(*source)[0];

			SRC_DATA srcData;
			srcData.data_in = srcArray;
			srcData.input_frames = source->size();
			srcData.src_ratio = (double)(soundGui->getNewFs()) / (double)(dynamic_pointer_cast<mpc::sampler::Sound>(snd)->getSampleRate());
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

		destSnd->setSampleRate(soundGui->getNewFs());
		destSnd->setMono(snd->isMono());
		destSnd->setName(soundGui->getNewName());

		if (soundGui->getNewBit() == 1)
		{
			sampler.lock()->process12Bit(destSnd->getSampleData());
		}
		else if (soundGui->getNewBit() == 2)
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
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newfs").lock()->setText(to_string(soundGui->getNewFs()));
}

void ResampleScreen::displayQuality()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("quality").lock()->setText(qualityNames[soundGui->getQuality()]);
}

void ResampleScreen::displayNewBit()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newbit").lock()->setText(bitNames[soundGui->getNewBit()]);
}

void ResampleScreen::displayNewName()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newname").lock()->setText(soundGui->getNewName());
}
