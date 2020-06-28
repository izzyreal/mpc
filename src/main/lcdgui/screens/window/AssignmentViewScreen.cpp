#include "AssignmentViewScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

AssignmentViewScreen::AssignmentViewScreen(const int layerIndex) 
	: ScreenComponent("assignment-view", layerIndex)
{
}

void AssignmentViewScreen::open()
{
	findLabel("info1").lock()->setInverted(true);
	findLabel("info1").lock()->setSize(13, 9);
	
	ls.lock()->setFocus(getFocusFromPadIndex());

	init();
	displayAssignmentView();
	mpc.addObserver(this); // Subscribe to "padandnote" and "bank" messages
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

	auto note = program.lock()->getPad(mpc.getPad() + 4)->getNote();
	auto focusPadIndex = mpc.getPad() + 4;
	
	while (focusPadIndex > 15)
	{
		focusPadIndex -= 16;
	}

	ls.lock()->setFocus(padFocusNames[focusPadIndex]);
	mpc.setPadAndNote(mpc.getPad() + 4, note);
}

void AssignmentViewScreen::down()
{
	init();
	
	if (param.find("3") != string::npos)
	{
		return;
	}

	auto note = program.lock()->getPad(mpc.getPad() - 4)->getNote();
	auto focusPadIndex = mpc.getPad() - 4;

	while (focusPadIndex > 15)
	{
		focusPadIndex -= 16;
	}

	ls.lock()->setFocus(padFocusNames[focusPadIndex]);
	mpc.setPadAndNote(mpc.getPad() - 4, note);
}

void AssignmentViewScreen::left()
{
    init();
	
	if (param.find("a") != string::npos)
	{
		return;
	}

    BaseControls::left();
    
	auto padIndex = mpc.getPad() - 1;
    auto note = program.lock()->getPad(padIndex)->getNote();
    mpc.setPadAndNote(padIndex, note);
}

void AssignmentViewScreen::right()
{
	init();

	if (param.find("d") != string::npos)
	{
		return;
	}

	BaseControls::right();
	auto padIndex = mpc.getPad() + 1;
	auto note = program.lock()->getPad(padIndex)->getNote();
	mpc.setPadAndNote(padIndex, note);
}

void AssignmentViewScreen::turnWheel(int i)
{
	init();	
	sampler.lock()->getLastPad(program.lock().get())->setNote(sampler.lock()->getLastPad(program.lock().get())->getNote() + i);
}

void AssignmentViewScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("bank") == 0)
	{
		displayAssignmentView();
	}
	else if (s.compare("padandnote") == 0)
	{
		mpc.getLayeredScreen().lock()->setFocus(getFocusFromPadIndex());
		displayAssignmentView();
	}
	else if (s.compare("note") == 0)
	{
		displayInfo1();
		displayInfo2();
		displayPad(getPadIndexFromFocus());
	}
}

void AssignmentViewScreen::displayAssignmentView()
{
	for (int i = 0; i < 16; i++)
		displayPad(i);

	displayInfo0();
	displayInfo1();
	displayInfo2();
}

void AssignmentViewScreen::displayPad(int i)
{
	auto note = program.lock()->getPad(i + (16 * mpc.getBank()))->getNote();

	string sampleName = "";

	if (note != 34)
	{
		auto sampleNumber = program.lock()->getNoteParameters(note)->getSndNumber();
		sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "--";
	
		if (sampleName.length() > 8)
		{
			sampleName = sampleName.substr(0, 8);
		}
		
		while (!sampleName.empty() && isspace(sampleName.back()))
		{
			sampleName.pop_back();
		}
	}

	findField(padFocusNames[i]).lock()->setText(sampleName);
}

void AssignmentViewScreen::displayInfo0()
{
	findLabel("info0").lock()->setText("Bank:" + letters[mpc.getBank()] + " Note:");
}

void AssignmentViewScreen::displayInfo1()
{
	auto focus = ls.lock()->getFocus();
	auto note = program.lock()->getPad(getPadIndexFromFocus())->getNote();
	auto text = note != 34 ? to_string(note) : "--";
	
	findLabel("info1").lock()->setText(text);
}

void AssignmentViewScreen::displayInfo2()
{
	auto focus = mpc.getLayeredScreen().lock()->getFocus();
	auto padIndex = getPadIndexFromFocus();
	int note = program.lock()->getPad(padIndex)->getNote();

	if (note == 34)
	{
		findLabel("info2").lock()->setText("=");
		return;
	}

	int sampleNumber = program.lock()->getNoteParameters(note)->getSndNumber();

	string sampleName = sampleNumber != -1 ? sampler.lock()->getSoundName(sampleNumber) : "";

	string stereo = "";
	auto lastPad = sampler.lock()->getLastPad(program.lock().get());
	
	if (sampleNumber != -1 && lastPad->getStereoMixerChannel().lock()->isStereo())
	{
		stereo = "(ST)";
	}

	findLabel("info2").lock()->setText("=" + sampleName + stereo);
}

int AssignmentViewScreen::getPadIndexFromFocus()
{
	auto focus = ls.lock()->getFocus();
	auto padIndex = -1;

	for (int i = 0; i < padFocusNames.size(); i++)
	{
		if (padFocusNames[i].compare(focus) == 0) {
			padIndex = i;
			break;
		}
	}

	return padIndex + mpc.getBank() * 16;
}


string AssignmentViewScreen::getFocusFromPadIndex()
{
	auto padIndex = mpc.getPad();
	
	while (padIndex > 15)
	{
		padIndex -= 16;
	}

	return padFocusNames[padIndex];
}
