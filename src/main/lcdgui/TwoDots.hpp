#pragma once
#include "Component.hpp"

#include <vector>

namespace mpc {
	namespace lcdgui {

		class TwoDots
			: public Component
		{

		private:
			std::vector<bool> selected{};
			bool color{ false };
			std::vector<bool> visible{};
			bool inverted{ false };

		public:
			void setInverted(bool b);
			void setSelected(int i, bool b);
			void setVisible(int i, bool b);

		public:
			void Draw(std::vector<std::vector<bool>>* pixels) override;

		public:
			TwoDots();
			~TwoDots();
		};

	}
}
