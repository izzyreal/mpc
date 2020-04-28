#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta {
	class MetaEventData;
}

namespace mpc::midi::event::meta {

	class Tempo
		: public MetaEvent
	{

	public:
		static float DEFAULT_BPM;
		static const int DEFAULT_MPQN{ 500000 };

	private:
		int mMPQN{};
		float mBPM{};

	public:
		virtual int getMpqn();
		virtual float getBpm();
		virtual void setMpqn(int m);
		virtual void setBpm(float b);

	public:
		int getEventSize() override;
		void writeToOutputStream(ostream& out) override;
		void writeToOutputStream(ostream& out, bool writeType) override;

	public:
		static std::shared_ptr<MetaEvent> parseTempo(int tick, int delta, MetaEventData* info);

	public:
		virtual int compareTo(mpc::midi::event::MidiEvent* other);

	public:
		Tempo();
		Tempo(int tick, int delta, int mpqn);

	};
}
