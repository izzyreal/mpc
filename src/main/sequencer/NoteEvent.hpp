#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
	class NoteEvent
		: public Event
	{

	private:
		int number{ 60 };
		int duration{ -1 };
		int variationTypeNumber{ 0 };
		int variationValue{ 64 };
		int velocity{ 0 };

	protected:
		std::shared_ptr<NoteEvent> noteOff{};

	public:
		std::weak_ptr<NoteEvent> getNoteOff();
		void setNote(int i);
		int getNote();
		void setDuration(int i);
		int getDuration();
		int getVariationTypeNumber();
		void setVariationTypeNumber(int i);
		void setVariationValue(int i);
		int getVariationValue();
		void setVelocity(int i);
		void setVelocityZero();
		int getVelocity();

		void CopyValuesTo(std::weak_ptr<Event> dest) override;

		NoteEvent();
		NoteEvent(int i);

		NoteEvent(bool noteOffTrue); // ctor used for noteOffs
		std::string getTypeName() override { return "note"; }

		~NoteEvent();

	};
}
