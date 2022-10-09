#include "Pot.hpp"

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>

using namespace mpc::hardware;
using namespace std;

Pot::Pot(mpc::Mpc& mpc, string label)
	: mpc(mpc)
{
	this->label = label;
	
}

void Pot::setValue(int i) {
	if (i < 0 || i > 100)
	{
		return;
	}

	if (label.compare("vol") == 0)
	{
		mpc.getAudioMidiServices().lock()->setMasterLevel(i);
	}
	else if (label.compare("rec") == 0)
	{
		mpc.getAudioMidiServices().lock()->setRecordLevel(i);
	}

    updateUi();
}

int Pot::getValue() {
	if (label.compare("vol") == 0)
	{
		return mpc.getAudioMidiServices().lock()->getMasterLevel();
	}
	else if (label.compare("rec") == 0)
	{
		return mpc.getAudioMidiServices().lock()->getRecordLevel();
	}
	return 0;
}

string Pot::getLabel()
{
	return label;
}
