#include <sampler/Sound.hpp>

#include <Mpc.hpp>
#include <sampler/Sampler.hpp>
#include <mpc/MpcSoundOscillatorControls.hpp>

using namespace mpc::sampler;
using namespace std;

Sound::Sound(int rate, int index) 
{
	this->memoryIndex = index;
	oscillatorControls = new ctoot::mpc::MpcSoundOscillatorControls(memoryIndex, 0);
}

Sound::Sound() 
{
	oscillatorControls = new ctoot::mpc::MpcSoundOscillatorControls(-1, 0);
	oscillatorControls->setName("click");
}

void Sound::setName(string s)
{
    name = s;
    
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

void Sound::setBeatCount(int i)
{
    if(i < 1 || i > 32)
        return;

    numberOfBeats = i;
    
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
    return oscillatorControls->getSampleData();
}

int Sound::getSndLevel()
{
    return oscillatorControls->getSndLevel();
}

int Sound::getTune()
{
    return oscillatorControls->getTune();
}

int Sound::getStart()
{
    return oscillatorControls->getStart();
}

int Sound::getEnd()
{
    return oscillatorControls->getEnd();
}

bool Sound::isLoopEnabled()
{
    return oscillatorControls->isLoopEnabled();
}

int Sound::getLoopTo()
{
    return oscillatorControls->getLoopTo();
}

bool Sound::isMono()
{
    return oscillatorControls->isMono();
}

int Sound::getSampleRate()
{
    return oscillatorControls->getSampleRate();
}

void Sound::setSampleRate(int sr) {
	oscillatorControls->setSampleRate(sr);
}

int Sound::getLastFrameIndex()
{
    return oscillatorControls->getLastFrameIndex();
}

int Sound::getFrameCount()
{
    return oscillatorControls->getFrameCount();
}

void Sound::setMono(bool mono)
{
    oscillatorControls->setMono(mono);
}

void Sound::setEnd(int end)
{
    oscillatorControls->setEnd(end);
}

void Sound::setLevel(int level)
{
    oscillatorControls->setSndLevel(level);
}

void Sound::setStart(int start)
{
    oscillatorControls->setStart(start);
}

void Sound::setLoopEnabled(bool loopEnabled)
{
    oscillatorControls->setLoopEnabled(loopEnabled);
}

void Sound::setLoopTo(int loopTo)
{
    oscillatorControls->setLoopTo(loopTo);
}

void Sound::setTune(int tune)
{
    oscillatorControls->setTune(tune);
}

ctoot::mpc::MpcSoundOscillatorControls* Sound::getOscillatorControls()
{
    return oscillatorControls;
}

void Sound::insertFrame(vector<float> frame, unsigned int index) {
	oscillatorControls->insertFrame(frame, index);
}

Sound::~Sound() {
	delete oscillatorControls;
}
