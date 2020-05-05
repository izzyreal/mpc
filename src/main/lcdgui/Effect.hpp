#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {

		class Effect
			: public Component
		{

		private:
			bool filled = false;

		public:
			void setFilled(bool b);

		public:
			void Draw(std::vector<std::vector<bool>>* pixels) override;

		public:
			Effect(MRECT rect);
			~Effect();

		};

	}
}
