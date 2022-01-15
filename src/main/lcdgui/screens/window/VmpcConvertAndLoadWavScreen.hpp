#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::lcdgui::screens::window {

	class VmpcConvertAndLoadWavScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;

        VmpcConvertAndLoadWavScreen(mpc::Mpc& mpc, const int layerIndex);

        void setLoadRoutine(std::function<void()> loadRoutine);
        
    private:
        std::function<void()> loadRoutine;
        
	};
}
