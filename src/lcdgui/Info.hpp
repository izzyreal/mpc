#pragma once
#include "Label.hpp"

#include <memory>
#include <string>

namespace mpc {
	namespace lcdgui {

		class Info
		{

		private:
			std::weak_ptr<Label> label{};

		public:
			std::weak_ptr<Label> getLabel();

			Info(std::weak_ptr<Label> label, std::string name, int x, int y, int size);
			~Info();

		};

	}

}
