#pragma once
#include <observer/Observable.hpp>

#include <vector>

namespace mpc {
	namespace lcdgui {
		class MixerStrip;
	}

	namespace ui {
		namespace sampler {

			class MixerGui
				: public moduru::observer::Observable
			{

			private:
				int tab{ 0 };
				bool link{ false };
				std::vector<mpc::lcdgui::MixerStrip*> mixerStrips{};
				int xPos{ 0 };
				int yPos{ 0 };
	
			public:
				void setLink(bool b);
				bool getLink();
				void setTab(int i);
				int getTab();
				void setMixerStrips(std::vector<mpc::lcdgui::MixerStrip*> mixerStrips);
				std::vector<mpc::lcdgui::MixerStrip*> getMixerStrips();
				int getXPos();
				void setXPos(int i);
				int getYPos();
				void setYPos(int i);

			public:
				MixerGui();
				~MixerGui();

			};

		}
	}
}
