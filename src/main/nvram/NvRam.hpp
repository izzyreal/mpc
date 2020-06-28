#pragma once

namespace mpc::nvram
{
	class NvRam
	{

	public:
		static void loadUserScreenValues();
		static void saveUserScreenValues();
		static void saveKnobPositions();
		static int getMasterLevel();
		static int getRecordLevel();
		static int getSlider();

	};
}
