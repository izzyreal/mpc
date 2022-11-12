#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class Sequence;
}

namespace mpc::sequencer
{
	class TempoChangeEvent
		: public Event
	{

	private:
		int ratio = 1000;
		int stepNumber = 0;
		Sequence* parent = nullptr;

	public:
		void plusOneBar(TempoChangeEvent *next);
		void minusOneBar(TempoChangeEvent *previous);
		void plusOneBeat(TempoChangeEvent *next);
		void minusOneBeat(TempoChangeEvent *previous);
		void plusOneClock(TempoChangeEvent* next);
		void minusOneClock(TempoChangeEvent* previous);
		void setRatio(int i);
		int getRatio();
		void setStepNumber(int i);
		int getStepNumber();
		int getBar(int n, int d);
		int getBeat(int n, int d);
		int getClock(int denominator);
		double getTempo();

	public:
		void CopyValuesTo(std::weak_ptr<Event> dest) override;

	public:
		// Smelly smelly smelly
		void setParent(Sequence* newParent);

	public:
		TempoChangeEvent(Sequence* parent);
		TempoChangeEvent(Sequence* parent, int ratio);
		TempoChangeEvent(Sequence* parent, int ratio, int step);
		std::string getTypeName() override { return "tempo-change"; }

	};
}
