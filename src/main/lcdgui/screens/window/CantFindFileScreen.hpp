#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::disk
{
	class ProgramLoader;
}

namespace mpc::lcdgui::screens::window
{
	class CantFindFileScreen
		: public mpc::lcdgui::ScreenComponent
	{
	public:
		void function(int i) override;
		void open() override;

		CantFindFileScreen(const int layerIndex);

	private:
		std::string fileName = "";
		bool skipAll = false;
		bool waitingForUser = false;

		friend class mpc::disk::ProgramLoader;

	};
}
