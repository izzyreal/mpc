#pragma once

namespace mpc::sequencer {
	class Track;
	class Sequencer;
}

namespace mpc::command {

	class InsertEvent
	{

	private:
		mpc::sequencer::Track* track = nullptr;
		mpc::sequencer::Sequencer* sequencer = nullptr;
		int insertEventType = 0;

	public:
		virtual void execute();

		InsertEvent(int insertEventType, mpc::sequencer::Track* track, mpc::sequencer::Sequencer* sequencer);
	};
}
