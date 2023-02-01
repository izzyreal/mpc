#pragma once
#include <observer/Observable.hpp>

namespace mpc::sequencer
{
	class Event
		: public moduru::observer::Observable
	{
	public:
        bool dontDelete = false;
        int wasMoved = 0;

		virtual void setTick(int relativeTick);
		int getTick();
		int getTrack();
		void setTrack(int i);

		virtual void CopyValuesTo(std::weak_ptr<Event> dest);
        virtual std::string getTypeName() = 0;

    protected:
        int tick = 0;

    private:
        int track = 0;

    };
}
