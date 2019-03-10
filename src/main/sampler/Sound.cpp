#include <sampler/Sound.hpp>

#include <Mpc.hpp>
#include <sampler/Sampler.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

using namespace mpc::sampler;
using namespace std;

Sound::Sound(int rate, int index) 
{
	this->memoryIndex = index;
	msoc = new ctoot::mpc::MpcSoundOscillatorControls(memoryIndex, 0);
}

Sound::Sound() 
{
	msoc = new ctoot::mpc::MpcSoundOscillatorControls(-1, 0);
	msoc->setName("click");
}

void Sound::setName(string s)
{
    name = s;
    setChanged();
    notifyObservers(string("samplename"));
}

int Sound::getMemoryIndex()
{
    return memoryIndex;
}

void Sound::setMemoryIndex(int i)
{
    memoryIndex = i;
}

void Sound::setNumberOfBeats(int i)
{
    if(i < 1 || i > 32)
        return;

    numberOfBeats = i;
    setChanged();
    notifyObservers(string("beat"));
}

int Sound::getBeatCount()
{
    return numberOfBeats;
}

string Sound::getName()
{
    return name;
}

vector<float>* Sound::getSampleData()
{
    return msoc->getSampleData();
}

int Sound::getSndLevel()
{
    return msoc->getSndLevel();
}

int Sound::getTune()
{
    return msoc->getTune();
}

int Sound::getStart()
{
    return msoc->getStart();
}

int Sound::getEnd()
{
    return msoc->getEnd();
}

bool Sound::isLoopEnabled()
{
    return msoc->isLoopEnabled();
}

int Sound::getLoopTo()
{
    return msoc->getLoopTo();
}

bool Sound::isMono()
{
    return msoc->isMono();
}

int Sound::getSampleRate()
{
    return msoc->getSampleRate();
}

void Sound::setSampleRate(int sr) {
	msoc->setSampleRate(sr);
}

int Sound::getLastFrameIndex()
{
    return msoc->getLastFrameIndex();
}

void Sound::setMono(bool mono)
{
    msoc->setMono(mono);
}

void Sound::setEnd(int end)
{
    msoc->setEnd(end);
}

void Sound::setLevel(int level)
{
    msoc->setSndLevel(level);
}

void Sound::setStart(int start)
{
    msoc->setStart(start);
}

void Sound::setLoopEnabled(bool loopEnabled)
{
    msoc->setLoopEnabled(loopEnabled);
}

void Sound::setLoopTo(int loopTo)
{
    msoc->setLoopTo(loopTo);
}

void Sound::setTune(int tune)
{
    msoc->setTune(tune);
}

ctoot::mpc::MpcSoundOscillatorControls* Sound::getMsoc()
{
    return msoc;
}

void Sound::insertFrame(vector<float> frame, unsigned int index) {
	msoc->insertFrame(frame, index);
}

Sound::~Sound() {
	delete msoc;
}
