#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <string>

namespace mpc::midi::event::meta
{
	class TextualMetaEvent
		: public MetaEvent
	{

	public:
		std::string mText;

	public:
		void setText(std::string t);
		std::string getText();

	public:
		void writeToOutputStream(std::ostream& out) override;
		void writeToOutputStream(std::ostream& out, bool writeType) override;
		std::string toString() override;
		int getEventSize() override;

	public:
		int compareTo(mpc::midi::event::MidiEvent* other);

	public:
		TextualMetaEvent(int tick, int delta, int type, std::string text);

	};
}
