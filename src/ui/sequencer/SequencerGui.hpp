#pragma once
#include <observer/Observable.hpp>

namespace mpc {
	namespace ui {
		namespace sequencer {

			class SequencerGui
				    : public moduru::observer::Observable
			{

			private:
				bool afterEnabled{};
				bool fullLevelEnabled{};
				bool sixteenLevelsEnabled{};
				int note{};
				int parameter{};
				int type{};
				int originalKeyPad{};

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
	}
}
