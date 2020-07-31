#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
	class LoadScreen;
}

namespace mpc::lcdgui::screens::window
{
	class LoadASequenceFromAllSCreen;
}

namespace mpc::lcdgui::screens::window
{
	class LoadASequenceScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void turnWheel(int i) override;
		void function(int i) override;

		LoadASequenceScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		int loadInto = 0;
		void setLoadInto(int i);

		void displayLoadInto();
		void displayFile();

		friend class LoadASequenceFromAllScreen;
		friend class mpc::lcdgui::screens::LoadScreen;

	};
}
