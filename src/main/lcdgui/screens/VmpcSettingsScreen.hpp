#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::sampler { class Pad; }

namespace mpc::sequencer { class Track; }

namespace mpc::nvram { class NvRam; }

namespace mpc::lcdgui::screens
{
	class VmpcSettingsScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;

		VmpcSettingsScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;
        void function(int) override;
        
	private:
		const std::vector<std::string> initialPadMappingNames{ "VMPC2000XL", "ORIGINAL" };
        const std::vector<std::string> _16LevelsEraseModeNames{ "All levels", "Only pressed level" };
        
		int initialPadMapping = 0;
        int _16LevelsEraseMode = 0;
        int autoConvertWavs = 0;
		
        void setInitialPadMapping(int i);
        void set16LevelsEraseMode(int i);
        void setAutoConvertWavs(int i);
		
        void displayInitialPadMapping();
        void display16LevelsEraseMode();
        void displayAutoConvertWavs();
		
		friend class mpc::sampler::Pad;
		friend class mpc::nvram::NvRam;
        friend class mpc::sequencer::Track;

	};
}
