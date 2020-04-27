#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <ostream>

namespace mpc::midi::event::meta {

	class EndOfTrack
		: public MetaEvent
	{

	public:
		int getEventSize() override;

	public:
		void writeToOutputStream(ostream& out) override;
		virtual int compareTo(mpc::midi::event::MidiEvent* other);

		EndOfTrack(int tick, int delta);

	};
}
