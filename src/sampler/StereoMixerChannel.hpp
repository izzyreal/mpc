#pragma once

#include <observer/Observable.hpp>

namespace mpc {
	namespace sampler {

		class StereoMixerChannel
			: public moduru::observer::Observable
		{

		private:
			int panning{ 0 };
			int level{ 0 };
			bool stereo{ false };

		public:
			void setStereo(bool b);
			bool isStereo();
			void setPanning(int i);
			int getPanning();
			void setLevel(int i);
			int getLevel();

		public:
			StereoMixerChannel();

		};
	}
}
