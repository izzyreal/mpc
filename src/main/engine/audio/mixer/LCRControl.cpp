#include "LCRControl.hpp"

#include "engine/audio/mixer/MixControlIds.hpp"

using namespace mpc::engine::audio::mixer;
using namespace std;

LCRControl::LCRControl(const string &name,
                       const shared_ptr<mpc::engine::control::ControlLaw> &law,
                       float initialValue)
    : LawControl(MixControlIds::LCR, name, law, initialValue)
{
}
