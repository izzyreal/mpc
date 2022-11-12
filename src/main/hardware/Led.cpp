#include "Led.hpp"

using namespace mpc::hardware;

Led::Led(std::string label) : label(label)
{
}

std::string Led::getLabel()
{
	return label;
}

void Led::light(bool b)
{
	if (b)
		notifyObservers(std::string(label + "-on"));
	else
		notifyObservers(std::string(label + "-off"));
}
