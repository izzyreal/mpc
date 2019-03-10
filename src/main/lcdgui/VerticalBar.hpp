#pragma once
#include "Component.hpp"

namespace mpc {
	namespace lcdgui {

		class VerticalBar
			: public Component
		{

		private:
			int value{ 100 };
			bool color = true;

		public:
			virtual void setValue(int value);
			virtual void setColor(bool on);

		public:
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			VerticalBar(MRECT rect);
			~VerticalBar();

		};

	}
}
