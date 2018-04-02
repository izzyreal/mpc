#pragma once

namespace mpc {
	namespace nvram {

		class KnobPositions
		{

		public:
			int recordLevel{ 0 };
			int masterLevel{ 65 };
			int slider{ 64 };

			KnobPositions();

		private:
			//friend class NvRam;
		};

	}
}
