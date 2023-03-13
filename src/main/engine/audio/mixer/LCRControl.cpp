#include "LCRControl.hpp"

#include <engine/audio/mixer/MixControlIds.hpp>

using namespace ctoot::audio::mixer;
using namespace std;

LCRControl::LCRControl(string name, shared_ptr<ctoot::control::ControlLaw> law, float precision, float initialValue)
	: LawControl(MixControlIds::LCR, name, law, precision, initialValue)
{
}
