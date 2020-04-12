#pragma once

#include <vector>

namespace ctoot::midi::core {
	class ShortMessage;
}

namespace mpc::audiomidi {

	class MpcMidiPorts
	{

	private:
		std::vector<std::vector<ctoot::midi::core::ShortMessage>> receivers;

	public:
		std::vector<std::vector<ctoot::midi::core::ShortMessage>>& getReceivers();

	public:
		MpcMidiPorts();
		~MpcMidiPorts();
	};
}