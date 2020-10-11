#pragma once

#include <observer/Observable.hpp>

#include <string>

namespace mpc::hardware
{
	class Led
		: public moduru::observer::Observable
	{

	private:
		std::string label = "";

	public:
		std::string getLabel();

	public:
		void light(bool b);

	public:
		Led(std::string label);
	};
}
