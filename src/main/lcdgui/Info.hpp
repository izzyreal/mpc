#pragma once
#include "Label.hpp"

#include "Component.hpp"

#include <memory>
#include <string>

namespace mpc::lcdgui {

	class Info
		: public Component
	{

	private:
		std::shared_ptr<Label> label{};

	public:
		std::weak_ptr<Label> getLabel();

	public:
		void Draw(std::vector<std::vector<bool>>* pixels) override;

	public:
		Info(const std::string& name, int x, int y, int size);
		~Info();

	};
}
