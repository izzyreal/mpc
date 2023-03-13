#pragma once

namespace ctoot::mpc {

		class MpcStereoMixerChannel
		{

		private:
			int panning = 0;
			int level = 0;

		public:
			void setPanning(int i);
			int getPanning();
			void setLevel(int i);
			int getLevel();

		public:
			MpcStereoMixerChannel();
            virtual ~MpcStereoMixerChannel() {}
            
		};
}
