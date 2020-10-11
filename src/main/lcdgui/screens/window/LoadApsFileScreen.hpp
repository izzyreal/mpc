#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <disk/ApsLoader.hpp>

namespace mpc::lcdgui::screens::window
{
	class LoadApsFileScreen
		: public mpc::lcdgui::ScreenComponent
	{

	private:
		std::unique_ptr<mpc::disk::ApsLoader> apsLoader;

	public:
		void function(int i) override;

		LoadApsFileScreen(mpc::Mpc& mpc, const int layerIndex);

	};
}
