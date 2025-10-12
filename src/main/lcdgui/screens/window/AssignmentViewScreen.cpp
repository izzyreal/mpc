#include "AssignmentViewScreen.hpp"
#include "sampler/Pad.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;

AssignmentViewScreen::AssignmentViewScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "assignment-view", layerIndex)
{
}

void AssignmentViewScreen::open()
{
	findField("note")->setFocusable(false);
	findField("note")->setInverted(true);

	ls->setFocus(getFocusFromPadIndex());

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

	if (param.find("0") != std::string::npos)
	{
		return;
	}

	auto padIndex = mpc.getPad() + 4;
	ls->setFocus(padFocusNames[padIndex % 16]);
	mpc.setPad(padIndex);
}

void AssignmentViewScreen::down()
{
	init();
	
	if (param.find("3") != std::string::npos)
	{
		return;
	}

	auto padIndex = mpc.getPad() - 4;
	ls->setFocus(padFocusNames[padIndex % 16]);
	mpc.setPad(padIndex);
}

void AssignmentViewScreen::left()
{
    init();
	
	if (param.find("a") != std::string::npos)
	{
		return;
	}

    ScreenComponent::left();
    
	auto padIndex = mpc.getPad() - 1;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.setPad(padIndex);
}

void AssignmentViewScreen::right()
{
	init();

	if (param.find("d") != std::string::npos)
	{
		return;
	}

	ScreenComponent::right();
	auto padIndex = mpc.getPad() + 1;
    ls->setFocus(padFocusNames[padIndex % 16]);
    mpc.setPad(padIndex);
}

void AssignmentViewScreen::turnWheel(int i)
{
	init();
    auto lastPad = sampler->getLastPad(program.get());
	lastPad->setNote(lastPad->getNote() + i);
    displayNote();
    displaySoundName();
    displayPad(lastPad->getIndex() % 16);
}

void AssignmentViewScreen::update(Observable* o, Message message)
{
    const auto msg = std::get<std::string>(message);

	if (msg == "bank")
	{
		displayAssignmentView();
	}
	else if (msg == "pad")
	{
		ls->setFocus(getFocusFromPadIndex());
		displayAssignmentView();
	}
	else if (msg == "note")
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
	auto note = program->getPad(i + (16 * mpc.getBank()))->getNote();

	std::string sampleName;

	if (note != 34)
	{
		auto sampleNumber = program->getNoteParameters(note)->getSoundIndex();
		sampleName = sampleNumber != -1 ? sampler->getSoundName(sampleNumber) : "--";
	
		if (sampleName.length() > 8)
			sampleName = StrUtil::trim(sampleName.substr(0, 8));
	}

	findField(padFocusNames[i])->setText(sampleName);
}

void AssignmentViewScreen::displayBankInfoAndNoteLabel()
{
	findLabel("info0")->setText("Bank:" + letters[mpc.getBank()] + " Note:");
}

void AssignmentViewScreen::displayNote()
{
	auto note = program->getPad(getPadIndexFromFocus())->getNote();
	auto text = note == 34 ? "--" : std::to_string(note);
	findField("note")->setText(text);
}

void AssignmentViewScreen::displaySoundName()
{
    init();

    auto padIndex = getPadIndexFromFocus();
	int note = program->getPad(padIndex)->getNote();

	if (note == 34)
	{
		findLabel("info2")->setText("=");
		return;
	}

	int soundIndex = program->getNoteParameters(note)->getSoundIndex();

    std::string soundName = soundIndex == -1 ? "OFF" : sampler->getSoundName(soundIndex);

    std::string stereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono() ? "(ST)" : "";

	findLabel("info2")->setText("=" + StrUtil::padRight(soundName, " ", 16) + stereo);
}

int AssignmentViewScreen::getPadIndexFromFocus()
{
    init();

    int padIndex = -1;

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

std::string AssignmentViewScreen::getFocusFromPadIndex()
{
	auto padIndex = mpc.getPad();
	
	while (padIndex > 15)
		padIndex -= 16;

	return padFocusNames[padIndex];
}
