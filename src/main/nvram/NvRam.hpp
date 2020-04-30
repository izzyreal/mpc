#pragma once

#include <memory>

namespace mpc {
	
}

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
		static std::shared_ptr<mpc::ui::UserDefaults> load();
		static void saveUserDefaults();
		static void saveKnobPositions();
		static int getMasterLevel();
		static int getRecordLevel();
		static int getSlider();

	public:
		NvRam();

	};
}
