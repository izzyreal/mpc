#include "CopyNoteParametersScreen.hpp"

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

CopyNoteParametersScreen::CopyNoteParametersScreen(const int layerIndex)
	: ScreenComponent("copy-note-parameters", layerIndex)
{
}

void CopyNoteParametersScreen::open()
{
	init();

	auto programIndex = mpcSoundPlayerChannel->getProgram();
	auto note = mpc.getNote();

	setProg0(programIndex);
	setNote0(note);
	setProg1(programIndex);
	setNote1(note);
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
		setNote0(note0 + i);
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
	BaseControls::function(i);
	
	switch (i)
	{
	case 4:
	{
		auto source = dynamic_cast<mpc::sampler::NoteParameters*>(sampler.lock()->getProgram(prog0).lock()->getNoteParameters(note0));
		auto dest = dynamic_cast<mpc::sampler::Program*>(sampler.lock()->getProgram(prog1).lock().get());
		auto clone = source->clone();
		dest->setNoteParameters(note1, clone);
		ls.lock()->openScreen("program-assign");
		break;
	}
	}
}

void CopyNoteParametersScreen::displayProg0()
{
	auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(prog0).lock());
	findField("prog0").lock()->setText(StrUtil::padLeft(to_string(prog0 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote0()
{
	auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(prog0).lock());
	auto padIndex = program->getPadIndexFromNote(note0);
	auto soundIndex = note0 != 34 ? program->getNoteParameters(note0)->getSndNumber() : -1;
	auto noteText = note0 == 34 ? "--" : to_string(note0);
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? "-" + sampler.lock()->getSoundName(soundIndex) : "-OFF";
	
	if (note0 == -1)
	{
		sampleName = "";
	}
	
	findField("note0").lock()->setText(noteText + "/" + padName + sampleName);
}

void CopyNoteParametersScreen::displayProg1()
{
	auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(prog1).lock());
	findField("prog1").lock()->setText(StrUtil::padLeft(to_string(prog1 + 1), " ", 2) + "-" + program->getName());
}

void CopyNoteParametersScreen::displayNote1()
{
	auto program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(prog1).lock());
	auto padIndex = program->getPadIndexFromNote(note1);
	auto soundIndex = note1 != 34 ? program->getNoteParameters(note1)->getSndNumber() : -1;
	auto noteText = note1 == 34 ? "--" : to_string(note1);
	auto padName = padIndex != -1 ? sampler.lock()->getPadName(padIndex) : "OFF";
	auto sampleName = soundIndex != -1 ? "-" + sampler.lock()->getSoundName(soundIndex) : "-OFF";

	if (note1 == -1)
	{
		sampleName = "";
	}

	findField("note1").lock()->setText(noteText + "/" + padName + sampleName);
}


void CopyNoteParametersScreen::setProg0(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
	{
		return;
	}
	
	prog0 = i;
	displayProg0();
}

void CopyNoteParametersScreen::setProg1(int i)
{
	if (i < 0 || i >= sampler.lock()->getProgramCount())
	{
		return;
	}

	prog1 = i;
	displayProg1();
}

void CopyNoteParametersScreen::setNote0(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	note0 = i;
	displayNote0();
}

void CopyNoteParametersScreen::setNote1(int i)
{
	if (i < 0 || i > 63)
	{
		return;
	}
	note1 = i;
	displayNote1();
}
