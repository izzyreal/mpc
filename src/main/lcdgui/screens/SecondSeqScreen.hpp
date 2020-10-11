#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::disk {
	class AllLoader;
}

namespace mpc::file::all {
	class Sequencer;
}

namespace mpc::sequencer {
	class Sequencer;
}

namespace mpc::lcdgui::screens
{
	class SecondSeqScreen
		: public mpc::lcdgui::ScreenComponent
	{

	public:
		void function(int i) override;
		void turnWheel(int i) override;

	public:
		SecondSeqScreen(mpc::Mpc& mpc, const int layerIndex);

		void open() override;

	private:
		int sq = 0;
		
		void setSq(int i);
		void displaySq();
		void displayFunctionKeys();

		friend class mpc::sequencer::Sequencer;
		friend class mpc::disk::AllLoader;
		friend class mpc::file::all::Sequencer;
	};
}
