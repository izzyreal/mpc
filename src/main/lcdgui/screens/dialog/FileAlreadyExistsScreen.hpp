#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <disk/ApsSaver.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class FileAlreadyExistsScreen
		: public mpc::lcdgui::ScreenComponent
	{
	private:
		std::unique_ptr<mpc::disk::ApsSaver> apsSaver;

	public:
		void function(int i) override;
		
		FileAlreadyExistsScreen(const int layerIndex);

	};
}
