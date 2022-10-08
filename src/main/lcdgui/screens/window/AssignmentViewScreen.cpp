#include "AssignmentViewScreen.hpp"

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

AssignmentViewScreen::AssignmentViewScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "assignment-view", layerIndex)
{
}

void AssignmentViewScreen::open()
{
	findField("note").lock()->setFocusable(false);
	findField("note").lock()->setInverted(true);

	ls.lock()->setFocus(getFocusFromPadIndex());

	init();
	displayAssignmentView();
	mpc.addObserver(this); // Subscribe to "pad" and "bank" messages
}

void AssignmentViewScreen::close()
{
	mpc.deleteObserver(this);
}

void AssignmentViewScreen::up()
{
	init();

	if (param.find("0") != string::npos)
	{
		return;
	}

	auto padIndex = mpc.getPad() + 4;
	ls.lock()->setFocus(padFocusNames[padIndex % 16]);
	mpc.setPad(padIndex);
}

void AssignmentViewScreen::down()
{
	init();
	
	if (param.find("3") != string::npos)
	{
		return;
	}

	auto padIndex = mpc.getPad() - 4;
	ls.lock()->setFocus(padFocusNames[padIndex % 16]);
	mpc.setPad(padIndex);
}

void AssignmentViewScreen::left()
{
    init();
	
	if (param.find("a") != string::npos)
	{
		return;
	}

    ScreenComponent::left();
    
	auto padIndex = mpc.getPad() - 1;
    ls.lock()->setFocus(padFocusNames[padIndex % 16]);
    mpc.setPad(padIndex);
}

void AssignmentViewScreen::right()
{
	init();

	if (param.find("d") != string::npos)
	{
		return;
	}

	ScreenComponent::right();
	auto padIndex = mpc.getPad() + 1;
    ls.lock()->setFocus(padFocusNames[padIndex % 16]);
    mpc.setPad(padIndex);
}

void AssignmentViewScreen::turnWheel(int i)
{
	init();
    auto lastPad = sampler.lock()->getLastPad(program.lock().get());
	lastPad->setNote(lastPad->getNote() + i);
    displayNote();
    displaySoundName();
    displayPad(lastPad->getIndex() % 16);
}

void AssignmentViewScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s == "bank")
	{
		displayAssignmentView();
	}
	else if (s == "pad")
	{
		mpc.getLayeredScreen().lock()->setFocus(getFocusFromPadIndex());
		displayAssignmentView();
	}
	else if (s == "note")
	{
		displayNote();
		displaySoundName();
		displayPad(getPadIndexFromFocus());
	}
}

void AssignmentViewScreen::displayAssignmentView()
{
	for (int i = 0; i < 16; i++)
		displayPad(i);

    displayBankInfoAndNoteLabel();
	displayNote();
	displaySoundName();
}

void AssignmentViewScreen::displayPad(int i)
{
	auto note = program.lock()->getPad(i + (16 * mpc.getBank()))->getNote();

	string sampleName = "";

	if (note != 34)
	{
		auto sampleNumber = program.lock()->getNoteParameters(note)->getSoundIndex();
		sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "--";
	
		if (sampleName.length() > 8)
			sampleName = StrUtil::trim(sampleName.substr(0, 8));
	}

	findField(padFocusNames[i]).lock()->setText(sampleName);
}

void AssignmentViewScreen::displayBankInfoAndNoteLabel()
{
	findLabel("info0").lock()->setText("Bank:" + letters[mpc.getBank()] + " Note:");
}

void AssignmentViewScreen::displayNote()
{
	auto note = program.lock()->getPad(getPadIndexFromFocus())->getNote();
	auto text = note == 34 ? "--" : to_string(note);
	findField("note").lock()->setText(text);
}

void AssignmentViewScreen::displaySoundName()
{
	auto padIndex = getPadIndexFromFocus();
	int note = program.lock()->getPad(padIndex)->getNote();

	if (note == 34)
	{
		findLabel("info2").lock()->setText("=");
		return;
	}

	int soundIndex = program.lock()->getNoteParameters(note)->getSoundIndex();

	string soundName = soundIndex == -1 ? "OFF" : sampler.lock()->getSoundName(soundIndex);

	init();

	auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
	
	string stereo = "";

	if (soundIndex != -1 && noteParameters->getStereoMixerChannel().lock()->isStereo())
		stereo = "(ST)";

	findLabel("info2").lock()->setText("=" + StrUtil::padRight(soundName, " ", 16) + stereo);
}

int AssignmentViewScreen::getPadIndexFromFocus()
{
	auto padIndex = -1;

	init();

	for (int i = 0; i < padFocusNames.size(); i++)
	{
		if (padFocusNames[i] == param)
		{
			padIndex = i;
			break;
		}
	}

	return padIndex;
}


string AssignmentViewScreen::getFocusFromPadIndex()
{
	auto padIndex = mpc.getPad();
	
	while (padIndex > 15)
		padIndex -= 16;

	return padFocusNames[padIndex];
}
