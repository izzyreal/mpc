#include "MpcMidiPorts.hpp"

using namespace mpc::audiomidi;
using namespace ctoot::midi::core;

std::vector<std::vector<ShortMessage>>& MpcMidiPorts::getReceivers() {
	return receivers;
}

void MpcMidiPorts::panic()
{	
	ShortMessage tootMsg;
	for (auto& r : receivers)
	{
		for (int ch = 0; ch < 16; ch++)
		{
			for (int note = 0; note < 128; note++)
			{
				tootMsg.setMessage(ShortMessage::NOTE_OFF, ch, note, 0);
				r.push_back(tootMsg);
			}
		}
	}
}
