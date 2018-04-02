#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData;

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

				public:
					void writeToFile(moduru::io::OutputStream* out)  override;
					static std::shared_ptr<MetaEvent> parseTempo(int tick, int delta, MetaEventData* info);
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

					Tempo();
					Tempo(int tick, int delta, int mpqn);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);

				};

			}
		}
	}
}
