#include <engine/control/AbstractLaw.hpp>

using namespace ctoot::control;
using namespace std;

AbstractLaw::AbstractLaw(float min, float max, string units) 
{
	this->min = min;
	this->max = max;
	this->units = units;
}

const int AbstractLaw::resolution;

string AbstractLaw::getUnits()
{
    return units;
}
