#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Pad.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include <Mpc.hpp>

using namespace mpc::sampler;
using namespace mpc::engine;

Program::Program(mpc::Mpc& mpc, mpc::sampler::Sampler* samplerToUse)
{
	sampler = samplerToUse;
	
	for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
	{
		auto n = new NoteParameters(i);
		noteParameters.push_back(n);
	}
	
	for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
	{
		auto p = new Pad(mpc, i);
		pads.push_back(p);
	}
	
	slider = new PgmSlider();
}

void Program::init()
{
	midiProgramChange = 1;
}

int Program::getNumberOfSamples()
{
    auto counter = 0;

    for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
	{
        auto np = getNoteParameters(i + 35);
        
		if(np->getSoundIndex() != -1)
			counter++;
    }
    
	return counter;
}

void Program::setName(std::string s)
{
    name = s;
}

std::string Program::getName()
{
    return name;
}

NoteParameters* Program::getNoteParameters(int note)
{
	if (note < 35 || note > 98)
		return nullptr;

	return noteParameters[note - 35];
}

Pad* Program::getPad(int i)
{
	return pads[i];
}

std::shared_ptr<StereoMixer> Program::getStereoMixerChannel(int noteIndex)
{
	return std::dynamic_pointer_cast<StereoMixer>(noteParameters[noteIndex]->getStereoMixerChannel());
}

std::shared_ptr<IndivFxMixer> Program::getIndivFxMixerChannel(int noteIndex)
{
	return std::dynamic_pointer_cast<IndivFxMixer>(noteParameters[noteIndex]->getIndivFxMixerChannel());
}

int Program::getPadIndexFromNote(int note)
{
	if (note < 35 || note > 98)
		return -1;
	
	for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
	{
		if (pads[i]->getNote() == note)
			return i;
	}

	return -1;
}

std::vector<NoteParameters*> Program::getNotesParameters()
{
    return noteParameters;
}

mpc::sampler::PgmSlider* Program::getSlider()
{
    return slider;
}

void Program::setNoteParameters(int index, NoteParameters* nn)
{
	if (noteParameters[index] != nullptr)
		delete noteParameters[index];

	noteParameters[index] = nn;
}

int Program::getMidiProgramChange()
{
    return midiProgramChange;
}

void Program::setMidiProgramChange(int i)
{
	if (i < 1 || i > 128)
		return;

	midiProgramChange = i;
}

void Program::initPadAssign()
{
	for (int i = 0; i < Mpc2000XlSpecs::PROGRAM_PAD_COUNT; i++)
		pads[i]->setNote((*sampler->getInitMasterPadAssign())[i]);
}

int Program::getNoteFromPad(int i)
{
	return pads[i]->getNote();
}

std::vector<int> Program::getPadIndicesFromNote(int note)
{
    std::vector<int> result;
	
	for (int i = 0; i < pads.size(); i++)
	{
		if (pads[i]->getNote() == note)
			result.push_back(i);
	}

	return result;
}

Program::~Program()
{
	delete slider;
	for (auto& np : noteParameters) delete np;
	for (auto& p : pads) delete p;
}

bool Program::isPadPressedBySource(int padIndex, PadPressSource source)
{
    return pressedPadRegistry[padIndex].sourceCount[sourceIndex(source)] > 0;
}

void Program::registerPadPress(int padIndex, PadPressSource source)
{
    auto& pad = pressedPadRegistry[padIndex];
    pad.totalCount++;
    pad.sourceCount[sourceIndex(source)]++;
}

void Program::registerPadRelease(int padIndex, PadPressSource source)
{
    auto& pad = pressedPadRegistry[padIndex];
    if (pad.totalCount > 0) pad.totalCount--;
    auto& srcCount = pad.sourceCount[sourceIndex(source)];
    if (srcCount > 0) srcCount--;
}

bool Program::isPadRegisteredAsPressed(int padIndex) const
{
    return pressedPadRegistry[padIndex].totalCount > 0;
}

bool Program::isAnyPadRegisteredAsPressed() const
{
    for (const auto& pad : pressedPadRegistry)
        if (pad.totalCount > 0) return true;
    return false;
}

void Program::clearPressedPadRegistry()
{
    for (auto& pad : pressedPadRegistry)
    {
        pad.totalCount = 0;
        pad.sourceCount.fill(0);
    }
}

