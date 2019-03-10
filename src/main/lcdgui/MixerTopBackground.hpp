#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {

		class MixerTopBackground
			: public Component
		{

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			MixerTopBackground(MRECT rect);
			~MixerTopBackground();

		};

	}
}
