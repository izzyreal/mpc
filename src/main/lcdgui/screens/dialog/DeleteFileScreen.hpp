#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
	class DeleteFileScreen
		: public mpc::lcdgui::ScreenComponent
	{
	private:
		static void static_delete(void* args);

		std::thread deleteThread;
		void deleteFile();

	public:
		void function(int i) override;

		DeleteFileScreen(mpc::Mpc& mpc, const int layerIndex);
		~DeleteFileScreen();

	private:
		friend class DeleteFileThread;

	};
}
