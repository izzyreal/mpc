#include "CopyNoteParametersScreen.hpp"

#include <controls/BaseSamplerControls.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace moduru::lang;
using namespace std;

CopyNoteParametersScreen::CopyNoteParametersScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "copy-note-parameters", layerIndex)
{
	//baseControls = make_shared<BaseSamplerControls>(mpc);
}

void CopyNoteParametersScreen::open()
{
	init();

	auto note = sampler.lock()->getLastNp(program.lock().get())->getNumber();

	auto programIndex = mpcSoundPlayerChannel->getProgram();
	setProg0(programIndex);
	setNote0(note);
	setProg1(programIndex);
	setNote1(note - 35);
}

void CopyNoteParametersScreen::turnWheel(int i)
{
    init();
    
	if (param.compare("prog0") == 0)
	{
		setProg0(prog0 + i);
	}
	else if (param.compare("note0") == 0)
	{
		setNote0(mpc.getNote() + i);
	}
	else if (param.compare("prog1") == 0)
	{
		setProg1(prog1 + i);
	}
	else if (param.compare("note1") == 0)
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
		auto source = dynamic_cast<mpc::sampler::NoteParameters*>(sampler.lock()->getProgram(prog0).lock()->getNoteParameters(mpc.getNote()));
		auto dest = dynamic_cast<mpc::sampler::Program*>(sampler.lock()->getProgram(prog1).lock().get());
		auto clone = source->clone(note1);
		dest->setNoteParameters(note1, clone);
		openScreen("program-assign");
		break;
	}
	}
}

void CopyNoteParametersScreen::displayProg0()
{
	auto program = sampler.lock()->getProgram(prog0).lock();
	findField("prog0").lock()->setText(StrUtil::padLeft(to_string(prog0 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote0()
{
	auto noteParameters = sampler.lock()->getLastNp(program.lock().get());
	auto note0 = noteParameters->getNumber();
	auto program = sampler.lock()->getProgram(prog0).lock();
	auto padIndex = program->getPadIndexFromNote(note0);
	auto soundIndex = note0 != -1 ? noteParameters->getSoundIndex() : -1;
	auto noteText = note0 == -1 ? "--" : to_string(note0);
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? "-" + sampler.lock()->getSoundName(soundIndex) : "-OFF";
	
	if (note0 == -1)
		sampleName = "";
	
	findField("note0").lock()->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::displayProg1()
{
	auto program = sampler.lock()->getProgram(prog1).lock();
	findField("prog1").lock()->setText(StrUtil::padLeft(to_string(prog1 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote1()
{
	auto program = sampler.lock()->getProgram(prog1).lock();
	auto padIndex = program->getPadIndexFromNote(note1 + 35);
	auto soundIndex = note1 != -1 ? program->getNoteParameters(note1 + 35)->getSoundIndex() : -1;
	auto noteText = note1 == -1 ? "--" : to_string(note1 + 35);
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? "-" + sampler.lock()->getSoundName(soundIndex) : "-OFF";

	if (note1 == -1)
		sampleName = "";

	findField("note1").lock()->setText(noteText + "/" + padName + sampleName);
}


void CopyNoteParametersScreen::setProg0(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
		return;
	
	prog0 = i;
	displayProg0();
}

void CopyNoteParametersScreen::setProg1(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
		return;

	prog1 = i;
	displayProg1();
}

void CopyNoteParametersScreen::setNote0(int i)
{
	if (i < 35 || i > 98)
		return;

	mpc.setPadAndNote(mpc.getPad(), i);
	displayNote0();
}

void CopyNoteParametersScreen::setNote1(int i)
{
	if (i < 0 || i > 63)
		return;

	note1 = i;
	displayNote1();
}
