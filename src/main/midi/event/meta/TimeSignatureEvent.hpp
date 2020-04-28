#pragma once
#include <midi/event/meta/MetaEvent.hpp>

namespace mpc::midi::event::meta {

	class MetaEventData;

	class TimeSignature
		: public MetaEvent
	{

	public:
		static const int METER_EIGHTH{ 12 };
		static const int METER_QUARTER{ 24 };
		static const int METER_HALF{ 48 };
		static const int METER_WHOLE{ 96 };
		static const int DEFAULT_METER{ 24 };
		static const int DEFAULT_DIVISION{ 8 };

	private:
		int mNumerator{};
		int mDenominator{};
		int mMeter{};
		int mDivision{};

	public:
		virtual void setTimeSignature(int num, int den, int meter, int div);
		virtual int getNumerator();
		virtual int getDenominatorValue();
		virtual int getRealDenominator();
		virtual int getMeter();
		virtual int getDivision();

	public:
		int getEventSize() override;
		void writeToOutputStream(ostream& out)  override;
		void writeToOutputStream(ostream& out, bool writeType) override;

	public:
		static std::shared_ptr<MetaEvent> parseTimeSignature(int tick, int delta, MetaEventData* info);

	private:
		int log2(int den);

	public:
		std::string toString();
		virtual int compareTo(mpc::midi::event::MidiEvent* other);

	public:
		TimeSignature();
		TimeSignature(int tick, int delta, int num, int den, int meter, int div);


	};
}
