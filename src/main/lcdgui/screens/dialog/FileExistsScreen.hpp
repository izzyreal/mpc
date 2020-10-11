#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <disk/ApsSaver.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class FileExistsScreen
		: public mpc::lcdgui::ScreenComponent
	{
	private:
		std::unique_ptr<mpc::disk::ApsSaver> apsSaver;

	public:
		void function(int i) override;

		FileExistsScreen(mpc::Mpc& mpc, const int layerIndex);

	};
}
