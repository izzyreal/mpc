#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class DeleteFolderScreen
		: public mpc::lcdgui::ScreenComponent
	{
	private:
		std::thread deleteFolderThread{};
		static void static_deleteFolder(void* this_p);
		void deleteFolder();

	public:
		void function(int i) override;

	public:
		DeleteFolderScreen(mpc::Mpc& mpc, const int layerIndex);
		~DeleteFolderScreen();

	};
}
