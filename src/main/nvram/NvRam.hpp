#pragma once

namespace mpc { class Mpc; }

namespace mpc::nvram
{
	class NvRam
	{

	public:
		static void loadUserScreenValues(mpc::Mpc& mpc);
		static void saveUserScreenValues(mpc::Mpc& mpc);
		static void saveKnobPositions(mpc::Mpc& mpc);

		static void saveVmpcSettings(mpc::Mpc& mpc);
		static void loadVmpcSettings(mpc::Mpc& mpc);

		static int getMasterLevel();
		static int getRecordLevel();
		static int getSlider();

	};
}
