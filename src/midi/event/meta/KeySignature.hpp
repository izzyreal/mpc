#pragma once
#include <midi/event/meta/MetaEvent.hpp>

#include <io/OutputStream.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class MetaEventData;

				class KeySignature
					: public MetaEvent
				{

				public:
					static const int SCALE_MAJOR{ 0 };
					static const int SCALE_MINOR{ 1 };

				private:
					int mKey{};
					int mScale{};

				public:
					void setKey(int key);
					int getKey();
					void setScale(int scale);
					int getScale();

				public:
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out)  override;
					static std::shared_ptr<MetaEvent> parseKeySignature(int tick, int delta, MetaEventData* info);
					int compareTo(mpc::midi::event::MidiEvent* other);

					KeySignature(int tick, int delta, int key, int scale);

				public:
					void writeToFile(moduru::io::OutputStream* out, bool writeType);
				};

			}
		}
	}
}
