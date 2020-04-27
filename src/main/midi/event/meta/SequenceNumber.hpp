#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta {

	class MetaEventData;

	class SequenceNumber
		: public MetaEvent
	{

	private:
		int mNumber{};

	public:
		int getMostSignificantBits();
		int getLeastSignificantBits();
		int getSequenceNumber();
		void writeToOutputStream(ostream& out)  override;
		static std::shared_ptr<MetaEvent> parseSequenceNumber(int tick, int delta, MetaEventData* info);

	public:
		int getEventSize() override;

	public:
		int compareTo(mpc::midi::event::MidiEvent* other);

		SequenceNumber(int tick, int delta, int number);

	};
}
