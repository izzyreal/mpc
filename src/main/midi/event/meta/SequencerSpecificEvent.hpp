#pragma once

#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta {

	class SequencerSpecificEvent
		: public MetaEvent
	{
	private:
		std::vector<char> mData;

	public:
		virtual void setData(std::vector<char> data);
		virtual std::vector<char> getData();

	public:
		int getEventSize() override;
		void writeToOutputStream(std::ostream& out)  override;
		void writeToOutputStream(std::ostream& out, bool writeType) override;
		
	public:
		virtual int compareTo(mpc::midi::event::MidiEvent* other);

	public:
		SequencerSpecificEvent(int tick, int delta, std::vector<char> data);

	};
}
