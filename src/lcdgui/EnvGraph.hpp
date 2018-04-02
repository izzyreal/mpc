#pragma once
#include "Component.hpp"

#include <memory>
#include <vector>

namespace mpc {
	class Mpc;
	namespace lcdgui {
		class EnvGraph
			: public Component
		{

		private:
			mpc::Mpc* mpc;
			std::vector<std::vector<int>> coordinates{};

		public:
			virtual void setCoordinates(std::vector<std::vector<int>> ia);
			void Draw(std::vector<std::vector<bool> >* pixels) override;

		public:
			EnvGraph(mpc::Mpc* mpc);
			~EnvGraph();

		};

	}
}
