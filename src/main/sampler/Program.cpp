#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

#include <Mpc.hpp>

using namespace mpc::sampler;
using namespace mpc::engine;

Program::Program(mpc::Mpc& mpc, mpc::sampler::Sampler* samplerToUse)
{
	sampler = samplerToUse;
	init();
	
	for (int i = 0; i < 64; i++)
	{
		auto n = new NoteParameters(i);
		noteParameters.push_back(n);
	}
	
	for (int i = 0; i < 64; i++)
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

    for (int i = 0; i < 64; i++)
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
	
	for (int i = 0; i < 64; i++)
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
	for (int i = 0; i < 64; i++)
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
