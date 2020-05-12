#pragma once
#include <observer/Observable.hpp>

namespace mpc::ui::sequencer {

	class SequencerGui
		: public moduru::observer::Observable
	{

	private:
		bool afterEnabled = false;
		bool fullLevelEnabled = false;
		bool sixteenLevelsEnabled = false;
		int note = 35;
		int parameter = 0;
		int type = 0;
		int originalKeyPad = 0;

	public:
		bool isAfterEnabled();
		void setAfterEnabled(bool b);
		bool isFullLevelEnabled();
		void setFullLevelEnabled(bool b);
		bool isSixteenLevelsEnabled();
		void setSixteenLevelsEnabled(bool b);
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
