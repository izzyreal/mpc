#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {

		class HorizontalBar
			: public Component
		{

		private:
			int value;

		public:
			void setValue(int value);

		public:
			void Draw(std::vector<std::vector<bool>>* pixels) override;

		public:
			HorizontalBar(MRECT rect, int value);
			~HorizontalBar();

		};

	}
}
