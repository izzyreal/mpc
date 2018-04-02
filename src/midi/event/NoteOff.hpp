#pragma once
#include <midi/event/ChannelEvent.hpp>

namespace mpc {
	namespace midi {
		namespace event {

			class NoteOff
				: public ChannelEvent
			{

			public:
				virtual int getNoteValue();
				virtual int getVelocity();
				virtual void setNoteValue(int p);
				virtual void setVelocity(int v);

				NoteOff(int tick, int channel, int note, int velocity);
				NoteOff(int tick, int delta, int channel, int note, int velocity);
			};

		}
	}
}
