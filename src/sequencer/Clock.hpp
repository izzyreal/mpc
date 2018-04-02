#pragma once

#include <cmath>

namespace mpc {
	namespace sequencer {

		class Clock {

		private:
			static const double ppqn_f;
			static const double rec60;
			static const double rec03;
			double bpm, omega, pd, lpd;
			double vpd, lvpd, dinphase;
			int tickN; // the tick number
			double Fs_rec;
			int cycleCount;
			int tickOffset{ 0 };

		public:
			void init(double Fs);
			void set_bpm(const double &BPM);
			bool proc();
			void reset();
			void zero();
			
			int getTickN();
			void setTick(int i);
			int getTickPosition();
			double getBpm();
		};

	}
}
