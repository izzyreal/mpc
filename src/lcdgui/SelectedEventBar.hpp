#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {
		class SelectedEventBar
			: public Component
		{

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			SelectedEventBar(MRECT rect);
			~SelectedEventBar();

		};

	}
}
