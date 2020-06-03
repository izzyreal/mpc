#include "AutoChromaticAssignmentScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

AutoChromaticAssignmentScreen::AutoChromaticAssignmentScreen(const int layerIndex) 
	: ScreenComponent("auto-chromatic-assignment", layerIndex)
{
}

void AutoChromaticAssignmentScreen::open()
{
	if (ls.lock()->getPreviousScreenName().compare("name") != 0)
	{
		auto letterNumber = sampler.lock()->getProgramCount() + 21;
		newName = "NewPgm-" + mpc::Mpc::akaiAscii[letterNumber];
	}

	init();

	setSourceSoundIndex(sampler.lock()->getLastNp(program.lock().get())->getSndNumber());
	displayOriginalKey();
	displayTune();
	displayProgramName();
}

void AutoChromaticAssignmentScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("source") == 0)
	{
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
		setSourceSoundIndex(sampler.lock()->getLastNp(program.lock().get())->getSndNumber());
	}
	else if (param.compare("program-name") == 0)
	{
		auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));
		nameScreen->setName(newName);
		nameScreen->parameterName = "autochrom";
		ls.lock()->openScreen("name");
	}
	else if (param.compare("snd") == 0)
	{
		setSourceSoundIndex(sourceSoundIndex + i);
	}
	else if (param.compare("original-key") == 0)
	{
		setOriginalKey(originalKey + i);
	}
	else if (param.compare("tune") == 0)
	{
		setTune(tune + i);
	}
}

void AutoChromaticAssignmentScreen::setSourceSoundIndex(int i)
{
	if (i < -1 || i >= sampler.lock()->getSoundCount())
	{
		return;
	}

	sourceSoundIndex = i;
	displaySource();
}

void AutoChromaticAssignmentScreen::setOriginalKey(int i)
{
	if (i < 35 || i > 98)
	{
		return;
	}
	originalKey = i;
	displayOriginalKey();
}

void AutoChromaticAssignmentScreen::setTune(int i)
{
	if (i < -240 || i > 240)
	{
		return;
	}
	tune = i;
	displayTune();
}

void AutoChromaticAssignmentScreen::displaySource()
{
	auto note = sampler.lock()->getLastNp(program.lock().get())->getNumber();
	auto padIndex = program.lock()->getPadIndexFromNote(note);
	auto padName = padIndex == -1 ? "OFF" : sampler.lock()->getPadName(padIndex);
	findField("source").lock()->setText(to_string(note) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displayTune()
{
	string prefix = tune < 0 ? "-" : " ";
	findField("tune").lock()->setText(prefix + StrUtil::padLeft(to_string(abs(tune)), " ", 3));
}

void AutoChromaticAssignmentScreen::displayOriginalKey()
{
	auto padIndex = program.lock()->getPadIndexFromNote(originalKey);
	auto padName = sampler.lock()->getPadName(padIndex);
	findField("original-key").lock()->setText(to_string(originalKey) + "/" + padName);
}

void AutoChromaticAssignmentScreen::displaySnd()
{
	auto sampleName = sourceSoundIndex == -1 ? "OFF" : sampler.lock()->getSoundName(sourceSoundIndex);
	string stereo = sourceSoundIndex == -1 ? "" : sampler.lock()->getSound(sourceSoundIndex).lock()->isMono() ? "" : "(ST)";
	findField("snd").lock()->setText(StrUtil::padRight(sampleName, " ", 16) + stereo);
}
void AutoChromaticAssignmentScreen::displayProgramName()
{
	findField("program-name").lock()->setText(newName);
}
