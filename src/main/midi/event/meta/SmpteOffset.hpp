#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {
			namespace meta {

				class FrameRate;

				class MetaEventData;

				class SmpteOffset
					: public MetaEvent
				{

				public:
					static const int FRAME_RATE_24{ 0 };
					static const int FRAME_RATE_25{ 1 };
					static const int FRAME_RATE_30_DROP{ 2 };
					static const int FRAME_RATE_30{ 3 };

				private:
					FrameRate* mFrameRate{};
					int mHours{};
					int mMinutes{};
					int mSeconds{};
					int mFrames{};
					int mSubFrames{};

				public:
					virtual void setFrameRate(FrameRate* fps);
					virtual FrameRate* getFrameRate();
					virtual void setHours(int h);
					virtual int getHours();
					virtual void setMinutes(int m);
					virtual int getMinutes();
					virtual void setSeconds(int s);
					virtual int getSeconds();
					virtual void setFrames(int f);
					virtual int getFrames();
					virtual void setSubFrames(int s);
					virtual int getSubFrames();

				public:
					int getEventSize() override;

				public:
					void writeToFile(moduru::io::OutputStream* out) override;
					static std::shared_ptr<MetaEvent> parseSmpteOffset(int tick, int delta, MetaEventData* info);
					virtual int compareTo(mpc::midi::event::MidiEvent* other);

					SmpteOffset(int tick, int delta, FrameRate* fps, int hour, int min, int sec, int fr, int subfr);

				public:
					//virtual int compareTo(::java::lang::Object* o) override;
					void writeToFile(moduru::io::OutputStream* out, bool writeType);

				private:
					//friend class FrameRate;
				};

			}
		}
	}
}
