#pragma once
#include <observer/Observable.hpp>

namespace mpc::ui::sequencer {

	class SequencerGui
		: public moduru::observer::Observable
	{

	private:
		int note = 35;
		int parameter = 0;
		int type = 0;
		int originalKeyPad = 0;

	public:
		int getNote();
		void setNote(int i);
		int getParameter();
		void setParameter(int i);
		int getType();
		void setType(int i);
		int getOriginalKeyPad();
		void setOriginalKeyPad(int i);

		SequencerGui();

	};
}
