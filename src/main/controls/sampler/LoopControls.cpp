#include <controls/sampler/LoopControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::controls::sampler;
using namespace std;

void LoopControls::init()
{
	AbstractSamplerControls::init();
	typableParams = vector<string>{	"to", "endlengthvalue" };
}

void LoopControls::openWindow()
{
	init();
	auto lLs = ls.lock();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
		soundGui->setPreviousScreenName("loop");
		lLs->openScreen("sound");
	}
	else if (param.compare("to") == 0) {
		lLs->openScreen("looptofine");
	}
	else if (param.compare("endlengthvalue") == 0) {
		lLs->openScreen("loopendfine");
	}
}

void LoopControls::function(int f)
{
	init();
	string newSampleName;
	vector<int> zone;
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (f) {
	case 0:
		lLs->openScreen("trim");
		break;
	case 1:
		lSampler->sort();
		break;
	case 2:
		lLs->openScreen("zone");
		break;
	case 3:
		lLs->openScreen("params");
		break;
	case 4:
		if (lSampler->getSoundCount() == 0) {
			return;
		}
		newSampleName = lSampler->getSoundName(soundGui->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = lSampler->addOrIncreaseNumber(newSampleName);
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("loop");
		lLs->openScreen("editsound");
		break;
	case 5:
		if (Mpc::instance().getControls().lock()->isF6Pressed()) {
			return;
		}
		Mpc::instance().getControls().lock()->setF6Pressed(true);
		zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()), soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		lSampler->playX(soundGui->getPlayX(), &zone);
		break;
	}
}

void LoopControls::turnWheel(int i)
{
    init();
    if(param == "") return;
	auto lSampler = sampler.lock();
    auto soundInc = getSoundIncrement(i);
	auto lSound = sound.lock();
    auto const oldLoopLength = lSound->getEnd() - lSound->getLoopTo();
    auto const loopFix = zoomGui->isLoopLngthFix();
	auto mtf = ls.lock()->lookupField(param).lock();
	if (mtf->isSplit()) {
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}

    if (param.compare("to") == 0) {
		if (loopFix && lSound->getLoopTo() + soundInc + oldLoopLength > lSound->getLastFrameIndex()) {
			return;
		}

        lSound->setLoopTo(lSound->getLoopTo() + soundInc);
		if (loopFix) {
			lSound->setEnd(lSound->getLoopTo() + oldLoopLength);
		}
    }
    else if (param.compare("endlengthvalue") == 0) {
		if (loopFix && lSound->getEnd() + soundInc - oldLoopLength < 0) {
			return;
		}
        lSound->setEnd(lSound->getEnd() + soundInc);
		if (loopFix) {
			lSound->setLoopTo(lSound->getEnd() - oldLoopLength);
		}
    }
	else if (param.compare("playx") == 0) {
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("loop") == 0) {
		lSampler->setLoopEnabled(soundGui->getSoundIndex(), i > 0);
	}
	else if (param.compare("endlength") == 0) {
		soundGui->setEndSelected(i > 0);
	}
	else if (param.compare("snd") == 0 && i > 0) {
		lSampler->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		lSampler->setSoundGuiPrevSound();
	}
}

void LoopControls::setSlider(int i)
{
	if (!Mpc::instance().getControls().lock()->isShiftPressed())
	{
		return;
	}

	init();
	auto lSound = sound.lock();
	auto const oldLength = lSound->getEnd() - lSound->getLoopTo();
	auto const lengthFix = zoomGui->isSmplLngthFix();
	auto candidatePos = (int)((i / 124.0) * lSound->getLastFrameIndex());
	auto maxPos = 0;

	if (param.compare("to") == 0) {
		maxPos = lengthFix ? lSound->getLastFrameIndex() - oldLength : lSound->getLastFrameIndex();
		if (candidatePos > maxPos) {
			candidatePos = maxPos;
		}
		lSound->setLoopTo(candidatePos);
		if (lengthFix) {
			lSound->setEnd(lSound->getLoopTo() + oldLength);
		}
	}
	else if (param.compare("endlengthvalue") == 0) {
		maxPos = lengthFix ? oldLength : 0;
		if (candidatePos < maxPos) {
			candidatePos = maxPos;
		}

		lSound->setEnd(candidatePos);
		if (lengthFix) {
			lSound->setLoopTo(lSound->getEnd() - oldLength);
		}
	}
}

void LoopControls::left()
{
    AbstractSamplerControls::splitLeft();
}

void LoopControls::right()
{
	AbstractSamplerControls::splitRight();
}

void LoopControls::pressEnter()
{
	init();
	
	if (!isTypable())
	{
		return;
	}
	
	auto lLs = ls.lock();
	auto mtf = lLs->lookupField(param).lock();
	if (!mtf->isTypeModeEnabled())
	{
		return;
	}

	auto candidate = mtf->enter();
	auto lSound = sound.lock();
	auto const oldLength = lSound->getEnd() - lSound->getLoopTo();
	auto const lengthFix = zoomGui->isLoopLngthFix();

	if (candidate != INT_MAX) {
		if (param.compare("to") == 0) {
			if (lengthFix && candidate + oldLength > lSound->getLastFrameIndex())
			{
				return;
			}
	
			lSound->setLoopTo(candidate);
			
			if (lengthFix)
			{
				lSound->setEnd(lSound->getLoopTo() + oldLength);
			}
		}
		else if (param.compare("endlengthvalue") == 0)
		{
			if (lengthFix && candidate - oldLength < 0)
			{
				return;
			}

			lSound->setEnd(candidate);
			if (lengthFix)
			{
				lSound->setLoopTo(lSound->getEnd() - oldLength);
			}
		}
	}
}
