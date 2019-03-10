#pragma once

namespace mpc {
	class Mpc;

	namespace ui {
		class UserDefaults;
	}

	namespace nvram {

		class KnobPositions;

		class NvRam
		{

		public:
			static mpc::ui::UserDefaults* load();
			static void saveUserDefaults();
			static void saveKnobPositions(mpc::Mpc* mpc);
			static int getMasterLevel();
			static int getRecordLevel();
			static int getSlider();

		private:
			static KnobPositions* getKnobPositions();

		public:
			NvRam();

		private:
			//friend class KnobPositions;
		};

	}
}
