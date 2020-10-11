#pragma once

#include <midi/core/ShortMessage.hpp>
#include <vector>

namespace mpc::audiomidi {

	class MpcMidiPorts
	{

	private:
		std::vector<std::vector<ctoot::midi::core::ShortMessage>> receivers =
			std::vector<std::vector<ctoot::midi::core::ShortMessage>>(2);

	public:
		std::vector<std::vector<ctoot::midi::core::ShortMessage>>& getReceivers();
		void panic();

	};
}