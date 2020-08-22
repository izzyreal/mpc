#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Pad.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <Mpc.hpp>

using namespace mpc::sampler;
using namespace ctoot::mpc;
using namespace std;

Program::Program(mpc::Mpc& mpc, mpc::sampler::Sampler* sampler)
{
	this->sampler = sampler;
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
        
		if(np->getSndNumber() != -1)
			counter++;
    }
    
	return counter;
}

void Program::setName(string s)
{
    name = s;
}

string Program::getName()
{
    return name;
}

MpcNoteParameters* Program::getNoteParameters(int note)
{
	if (note < 35 || note > 98)
		return nullptr;

	return noteParameters[note - 35];
}

Pad* Program::getPad(int i)
{
	return pads[i];
}

weak_ptr<MpcStereoMixerChannel> Program::getStereoMixerChannel(int noteIndex)
{
	return dynamic_pointer_cast<MpcStereoMixerChannel>(noteParameters[noteIndex]->getStereoMixerChannel().lock());
}

weak_ptr<MpcIndivFxMixerChannel> Program::getIndivFxMixerChannel(int noteIndex)
{
	return dynamic_pointer_cast<MpcIndivFxMixerChannel>(noteParameters[noteIndex]->getIndivFxMixerChannel().lock());
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

vector<NoteParameters*> Program::getNotesParameters()
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
	
	notifyObservers(string("midiprogramchange"));
}

void Program::initPadAssign()
{
	sampler->getLastNp(this)->deleteObservers();
	sampler->getLastPad(this)->deleteObservers();

	for (int i = 0; i < 64; i++)
		pads[i]->setNote((*sampler->getInitMasterPadAssign())[i]);
}

int Program::getNoteFromPad(int i)
{
	return pads[i]->getNote();
}

vector<int> Program::getPadIndicesFromNote(int note)
{
	vector<int> result;
	
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
