#pragma once

#include <observer/Observable.hpp>

namespace mpc::ui::sequencer::window {

	class Assign16LevelsGui
		: public moduru::observer::Observable
	{

	private:
		int note = 35;
		int parameter = 0;
		int type = 0;
		int originalKeyPad = 0;

	public:
		void setNote(int newNote);
		int getNote();
		int getParameter();
		void setParameter(int i);
		int getType();
		void setType(int i);
		int getOriginalKeyPad();
		void setOriginalKeyPad(int i);

	};
}
