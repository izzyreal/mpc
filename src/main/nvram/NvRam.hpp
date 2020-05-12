#pragma once

#include <memory>

namespace mpc::ui {
	class UserDefaults;
}

namespace mpc::nvram {
	class KnobPositions;
}

namespace mpc::nvram {

	class NvRam
	{

	public:
		static mpc::ui::UserDefaults load();
		static void saveUserDefaults();
		static void saveKnobPositions();
		static int getMasterLevel();
		static int getRecordLevel();
		static int getSlider();

	public:
		NvRam();

	};
}
