#pragma once

namespace mpc::nvram
{
	class NvRam
	{

	public:
		static void load();
		static void saveUserDefaults();
		static void saveKnobPositions();
		static int getMasterLevel();
		static int getRecordLevel();
		static int getSlider();

	};
}
