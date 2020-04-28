#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta {

	class MetaEventData;

	class GenericMetaEvent
		: public MetaEvent
	{

	private:
		std::vector<char> mData{};

	public:
		int getEventSize() override;
		void writeToOutputStream(ostream& out)  override;
		void writeToOutputStream(ostream& out, bool writeType)  override;


	public:
		virtual int compareTo(mpc::midi::event::MidiEvent* other);

	public:
		GenericMetaEvent(int tick, int delta, MetaEventData* info);

	};
}
