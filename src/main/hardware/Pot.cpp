#include "Pot.hpp"

#include <Mpc.hpp>

#include <audiomidi/AudioMidiServices.hpp>

using namespace mpc::hardware;

Pot::Pot(mpc::Mpc& mpc, std::string label)
	: mpc(mpc)
{
	this->label = label;
	
}

void Pot::setValue(int i) {
	if (i < 0 || i > 100)
	{
		return;
	}

	if (label == "vol")
	{
		mpc.getAudioMidiServices().lock()->setMasterLevel(i);
	}
	else if (label == "rec")
	{
		mpc.getAudioMidiServices().lock()->setRecordLevel(i);
	}

    updateUi();
}

int Pot::getValue() {
	if (label == "vol")
	{
		return mpc.getAudioMidiServices().lock()->getMasterLevel();
	}
	else if (label == "rec")
	{
		return mpc.getAudioMidiServices().lock()->getRecordLevel();
	}
	return 0;
}

std::string Pot::getLabel()
{
	return label;
}
