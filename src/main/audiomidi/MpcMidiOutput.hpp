#pragma once

#include <engine/midi/ShortMessage.hpp>
#include <vector>
#include <memory>

#include "thirdp/concurrentqueue.h"

namespace mpc::audiomidi {

	class MpcMidiOutput
	{

	private:
        moodycamel::ConcurrentQueue<std::shared_ptr<mpc::engine::midi::ShortMessage>> outputQueueA;
        moodycamel::ConcurrentQueue<std::shared_ptr<mpc::engine::midi::ShortMessage>> outputQueueB;

	public:
        void enqueueMessageOutputA(std::shared_ptr<mpc::engine::midi::ShortMessage>);
        void enqueueMessageOutputB(std::shared_ptr<mpc::engine::midi::ShortMessage>);
        unsigned char dequeueOutputA(std::vector<std::shared_ptr<mpc::engine::midi::ShortMessage>>& buf);
        unsigned char dequeueOutputB(std::vector<std::shared_ptr<mpc::engine::midi::ShortMessage>>& buf);
		void panic();

	};
}