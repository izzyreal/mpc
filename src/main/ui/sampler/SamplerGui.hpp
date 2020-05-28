#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SamplerGui
		: public moduru::observer::Observable
	{
		std::string newName = "";

	public:

		void setNewName(std::string newName);
		std::string getNewName();

	};
}
