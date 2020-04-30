#include <controls/sampler/TrimControls.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>

#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sampler/window/ZoomGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

#include <lang/StrUtil.hpp>

#include <stdint.h>
#include <limits.h>

using namespace mpc::controls::sampler;
using namespace std;

TrimControls::TrimControls() : AbstractSamplerControls()
{
	typableParams = vector<string>{ "st", "end" };
}

void TrimControls::openWindow()
{
	init();
	auto lLs = ls.lock();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(soundGui->getSoundIndex(), sampler.lock()->getSoundCount());
		soundGui->setPreviousScreenName("trim");
		lLs->openScreen("sound");
	}
	else if (param.compare("st") == 0) {
		lLs->openScreen("startfine");
	}
	else if (param.compare("end") == 0) {
		lLs->openScreen("endfine");
	}
}

void TrimControls::function(int f)
{
	init();
	string newSampleName;
	vector<int> zone;
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (f) {
	case 0:
		lSampler->sort();
		break;
	case 1:
		lLs->openScreen("loop");
		break;
	case 2:
		lLs->openScreen("zone");
		break;
	case 3:
		lLs->openScreen("params");
		break;
	case 4:
		if (lSampler->getSoundCount() == 0)
			return;

		newSampleName = lSampler->getSoundName(soundGui->getSoundIndex());
		newSampleName = lSampler->addOrIncreaseNumber(moduru::lang::StrUtil::trim(newSampleName));
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("trim");
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

void TrimControls::turnWheel(int i)
{
	init();
	if (param == "") return;
	auto lSound = sound.lock();
	auto const oldLength = lSound->getEnd() - lSound->getStart();
	auto const lengthFix = zoomGui->isSmplLngthFix();
	auto lSampler = sampler.lock();
	//auto notch = getNotch(increment);
	auto soundInc = getSoundIncrement(i);
	auto mtf = ls.lock()->lookupField(param).lock();
	if (mtf->isSplit()) {
		soundInc = i >= 0 ? splitInc[mtf->getActiveSplit() - 1] : -splitInc[mtf->getActiveSplit() - 1];
	}
	if (param.compare("st") == 0) {
		if (lengthFix && lSound->getStart() + soundInc + oldLength > lSound->getLastFrameIndex()) {
			return;
		}
		lSound->setStart(lSound->getStart() + soundInc);
		if (lengthFix) {
			lSound->setEnd(lSound->getStart() + oldLength);
		}
	}
	else if (param.compare("end") == 0) {
		if (lengthFix && lSound->getEnd() + soundInc - oldLength < 0) {
			return;
		}
		lSound->setEnd(lSound->getEnd() + soundInc);
		if (lengthFix) {
			lSound->setStart(lSound->getEnd() - oldLength);
		}
	}
	else if (param.compare("view") == 0) {
		soundGui->setView(soundGui->getView() + i);
	}
	else if (param.compare("playx") == 0) {
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("snd") == 0 && i > 0) {
		lSampler->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0) {
		lSampler->setSoundGuiPrevSound();
	}
}

void TrimControls::setSlider(int i)
{
	if (!Mpc::instance().getControls().lock()->isShiftPressed()) return;
    init();
	auto lSound = sound.lock();
	auto const oldLength = lSound->getEnd() - lSound->getStart();
    auto const lengthFix = zoomGui->isSmplLngthFix();
    auto candidatePos = (int) ((i / 124.0) * lSound->getLastFrameIndex());
    auto maxPos = int (0);
	if (param.compare("st") == 0) {
		maxPos = lengthFix ? lSound->getLastFrameIndex() - oldLength : lSound->getLastFrameIndex();
		if (candidatePos > maxPos) {
			candidatePos = maxPos;
		}
		lSound->setStart(candidatePos);
		if (lengthFix) {
			lSound->setEnd(lSound->getStart() + oldLength);
		}
	}
	else if (param.compare("end") == 0) {
		maxPos = lengthFix ? oldLength : int(0);
		if (candidatePos < maxPos) {
			candidatePos = maxPos;
		}
		lSound->setEnd(candidatePos);
		if (lengthFix) {
			lSound->setStart(lSound->getEnd() - oldLength);
		}
	}
}

void TrimControls::left()
{
    super::splitLeft();
}

void TrimControls::right()
{
    super::splitRight();
}

void TrimControls::pressEnter()
{
	init();
	if (!isTypable()) return;
	auto lLs = ls.lock();
	auto mtf = lLs->lookupField(param).lock();
	if (!mtf->isTypeModeEnabled())
		return;

	auto candidate = mtf->enter();
	auto lSound = sound.lock();
	auto const oldLength = lSound->getEnd() - lSound->getStart();
	auto const lengthFix = zoomGui->isSmplLngthFix();
	if (candidate != INT_MAX) {
		if (param.compare("st") == 0) {
			if (lengthFix && candidate + oldLength > lSound->getLastFrameIndex())
				return;

			lSound->setStart(candidate);
			if (lengthFix)
				lSound->setEnd(lSound->getStart() + oldLength);

		}
		if (param.compare("end") == 0) {
			if (lengthFix && candidate - oldLength < 0)
				return;

			lSound->setEnd(candidate);
			if (lengthFix)
				lSound->setStart(lSound->getEnd() - oldLength);

		}
	}
}
