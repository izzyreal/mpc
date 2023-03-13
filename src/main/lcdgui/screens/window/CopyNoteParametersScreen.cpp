#include "CopyNoteParametersScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;

CopyNoteParametersScreen::CopyNoteParametersScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "copy-note-parameters", layerIndex)
{
}

void CopyNoteParametersScreen::open()
{
	init();

	auto note = sampler->getLastNp(program.get())->getNumber();

	auto programIndex = mpcSoundPlayerChannel().getProgram();
	setProg0(programIndex);
	setNote0(note);
	setProg1(programIndex);
	setNote1(note - 35);
}

void CopyNoteParametersScreen::turnWheel(int i)
{
    init();
    
	if (param == "prog0")
	{
		setProg0(prog0 + i);
	}
	else if (param == "note0")
	{
		setNote0(mpc.getNote() + i);
	}
	else if (param == "prog1")
	{
		setProg1(prog1 + i);
	}
	else if (param == "note1")
	{
		setNote1(note1 + i);
	}
}

void CopyNoteParametersScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 4:
	{
		auto source = dynamic_cast<mpc::sampler::NoteParameters*>(sampler->getProgram(prog0)->getNoteParameters(mpc.getNote()));
		auto dest = dynamic_cast<mpc::sampler::Program*>(sampler->getProgram(prog1).get());
		auto clone = source->clone(note1);
		dest->setNoteParameters(note1, clone);
		openScreen("program-assign");
		break;
	}
	}
}

void CopyNoteParametersScreen::displayProg0()
{
	auto program = sampler->getProgram(prog0);
	findField("prog0")->setText(StrUtil::padLeft(std::to_string(prog0 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote0()
{
	auto noteParameters = sampler->getLastNp(program.get());
	auto note0 = noteParameters->getNumber();
	auto program = sampler->getProgram(prog0);
	auto padIndex = program->getPadIndexFromNote(note0);
	auto soundIndex = note0 != -1 ? noteParameters->getSoundIndex() : -1;
	auto noteText = note0 == -1 ? "--" : std::to_string(note0);
	auto padName = sampler->getPadName(padIndex);
	auto sampleName = soundIndex != -1 ? "-" + sampler->getSoundName(soundIndex) : "-OFF";
	
	if (note0 == -1)
		sampleName = "";
	
	findField("note0")->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::displayProg1()
{
	auto program = sampler->getProgram(prog1);
	findField("prog1")->setText(StrUtil::padLeft(std::to_string(prog1 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote1()
{
	auto program = sampler->getProgram(prog1);
	auto padIndex = program->getPadIndexFromNote(note1 + 35);
	auto soundIndex = note1 != -1 ? program->getNoteParameters(note1 + 35)->getSoundIndex() : -1;
	auto noteText = note1 == -1 ? "--" : std::to_string(note1 + 35);
	auto padName = sampler->getPadName(padIndex);
	auto sampleName = soundIndex != -1 ? "-" + sampler->getSoundName(soundIndex) : "-OFF";

	if (note1 == -1)
		sampleName = "";

	findField("note1")->setText(noteText + "/" + padName + sampleName);
}


void CopyNoteParametersScreen::setProg0(int i)
{
	if (i < 0 || i >= sampler->getProgramCount())
		return;
	
	prog0 = i;
	displayProg0();
}

void CopyNoteParametersScreen::setProg1(int i)
{
	if (i < 0 || i >= sampler->getProgramCount())
		return;

	prog1 = i;
	displayProg1();
}

void CopyNoteParametersScreen::setNote0(int i)
{
	mpc.setNote(i);
	displayNote0();
}

void CopyNoteParametersScreen::setNote1(int i)
{
	if (i < 0 || i > 63)
		return;

	note1 = i;
	displayNote1();
}
