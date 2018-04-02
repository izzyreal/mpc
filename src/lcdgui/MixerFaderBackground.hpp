#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {

		class MixerFaderBackground
			: public Component
		{

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			MixerFaderBackground(MRECT rect);
			~MixerFaderBackground();

		};

	}
}
