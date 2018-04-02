#pragma once

namespace mpc {
	namespace ctootextensions {

		class MuteInfo
		{

		private:
			int note{ 0 };
			int drum{ 0 };

		public:
			virtual void setNote(int note);
			virtual void setDrum(int drum);
			virtual int getNote();
			virtual int getDrum();
			virtual bool muteMe(int note, int drum);

			MuteInfo();

		};

	}
}
