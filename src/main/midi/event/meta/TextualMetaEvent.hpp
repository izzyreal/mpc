#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <string>

namespace mpc::midi::event::meta {

	class TextualMetaEvent
		: public MetaEvent
	{

	public:
		std::string mText{ "" };

	public:
		virtual void setText(std::string t);
		virtual std::string getText();
		int getEventSize() override;

	public:
		void writeToOutputStream(ostream& out) override;
		int compareTo(mpc::midi::event::MidiEvent* other);
		std::string toString();

	public:
		TextualMetaEvent(int tick, int delta, int type, std::string text);

	};
}

