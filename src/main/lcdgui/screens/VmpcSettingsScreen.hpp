#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::sampler
{
	class Pad;
}

namespace mpc::lcdgui::screens
{
	class VmpcSettingsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		VmpcSettingsScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		const std::vector<std::string> initialPadMappingNames{ "VMPC", "ORIGINAL" };
		int initialPadMapping = 0;
		void setInitialPadMapping(int i);
		void displayInitialPadMapping();
		
		friend class mpc::sampler::Pad;

	};
}
