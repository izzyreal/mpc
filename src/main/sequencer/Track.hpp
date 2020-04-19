#pragma once

#include <Mpc.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/NoteEvent.hpp>
#include <observer/Observable.hpp>

#include <memory>

using namespace std;

namespace mpc::sequencer {

	class Track
		: public moduru::observer::Observable
	{

	private:
		static const int MAX_TICK{ 2147483647 };

		vector<shared_ptr<Event>> events{};

		vector<shared_ptr<NoteEvent>> queuedNoteOnEvents;
		vector<shared_ptr<NoteEvent>> noteOffs;

		mpc::sequencer::Sequence* parent{ nullptr };
		Mpc* mpc{ nullptr };
		weak_ptr<Sequencer> sequencer{};
		int busNumber{ 0 };
		string name{ "" };
		bool on{ false };
		int velocityRatio{ 0 };
		int programChange{ 0 };
		int device{ 0 };
		int trackIndex{ 0 };
		bool used{ false };
		int relativeTick{ 0 };
		int eventIndex{ 0 };
		weak_ptr<NoteEvent> lastAdded{};
		bool eventAvailable{ false };
		weak_ptr<Event> event{};
		bool multi{ false };
		bool delete_{ false };
		int tcValue{ 0 };
		vector<weak_ptr<Event>> tempEvents{};

	protected:
		static bool tickCmp(weak_ptr<Event> a, weak_ptr<Event> b);
		static bool noteCmp(weak_ptr<NoteEvent> a, weak_ptr<NoteEvent> b);

	public:
		void move(int tick, int oldTick);
		void setTrackIndex(int i);
		int getTrackIndex();
		weak_ptr<mpc::sequencer::NoteEvent> recordNoteOn();
		void flushNoteCache();
		void recordNoteOff(NoteEvent* n);
		void setUsed(bool b);
		void setOn(bool b);

	private:
		void addEventRealTime(shared_ptr<mpc::sequencer::Event> event);
		weak_ptr<NoteEvent> getNoteEvent(int tick, int note);

	public:
		weak_ptr<NoteEvent> addNoteEvent(int tick, int note);
		weak_ptr<Event> addEvent(int tick, string type);
		weak_ptr<Event> cloneEvent(weak_ptr<Event> src);
		bool adjustDurLastEvent(int newDur);
		void removeEvent(int i);
		void removeEvent(weak_ptr<Event> event);
		void removeEvents();
		void setVelocityRatio(int i);
		int getVelocityRatio();
		void setProgramChange(int i);
		int getProgramChange();
		void setBusNumber(int i);
		int getBusNumber();
		void setDeviceNumber(int i);
		int getDevice();
		weak_ptr<Event> getEvent(int i);
		void setName(string s);
		string getName();
		vector<weak_ptr<Event>> getEvents();

		int getNextTick();
		void playNext();
		void off(bool stop);
		bool isOn();
		bool isUsed();

	public:
		void setEventIndex(int i);

	public:
		vector<weak_ptr<Event>> getEventRange(int startTick, int endTick);
		void correctTimeRange(int startPos, int endPos, int stepLength);

	public:
		void removeDoubles();
		void sortEvents();
		vector<weak_ptr<NoteEvent>> getNoteEvents();

	private:
		vector<weak_ptr<NoteEvent>> getNoteEventsAtTick(int tick);

	public:
		void sortEventsByNotePerTick();

	private:
		void sortEventsOfTickByNote(vector<weak_ptr<NoteEvent>> noteEvents);

	public:
		void timingCorrect(int fromBar, int toBar, NoteEvent* noteEvent, int stepLength);
		int timingCorrectTick(int fromBar, int toBar, int tick, int stepLength);
		int swingTick(int tick, int noteValue, int percentage);
		void swing(int noteValue, int percentage, vector<int> noteRange);
		void swing(vector<weak_ptr<Event>> eventsToSwing, int noteValue, int percentage, vector<int> noteRange);
		void shiftTiming(bool later, int amount, int lastTick);
		void shiftTiming(vector<weak_ptr<Event>> eventsToShift, bool later, int amount, int lastTick);

	public:
		int getEventIndex();

	public:
		string getActualName();

	public:
		Track(mpc::sequencer::Sequence* parent, Mpc* mpc, int i);
		~Track();

	private:
		friend class Sequence;

	};
}
