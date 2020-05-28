#pragma once
#include <observer/Observable.hpp>

#include <memory>

namespace mpc::ui::sampler
{
	class SamplerGui
		: public moduru::observer::Observable
	{

		std::string prevScreenName = "";
		std::string newName = "";

	public:
		std::string getPrevScreenName();
		void setPrevScreenName(std::string s);
		void setNewName(std::string newName);
		std::string getNewName();

	};
}
