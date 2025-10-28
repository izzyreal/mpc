#include "sequencer/LegacyClock.hpp"

#include <cmath>

using namespace mpc::sequencer;

const double LegacyClock::ppqn_f = 96.0;
const double LegacyClock::rec60 = (1.0 / 60.0);
const double LegacyClock::rec03 = (10.0 / 3.0);

LegacyClock::LegacyClock()
{
    init(44100);
}

void LegacyClock::init(double Fs)
{
    Fs_rec = 1.0 / Fs;
    set_bpm(120.0);
    zero();
    reset();
}

void LegacyClock::set_bpm(const double &BPM)
{
    bpm = fabs(BPM);
    omega = ((bpm)*rec60 * Fs_rec);
}

bool LegacyClock::proc()
{
    bool tick = false;
    double tmp = pd;
    pd -= floor(pd);
    if (tmp > pd)
    {
        tickN = -1;
    }
    vpd = pd; // get the value before the increment
              // if (ilpd > ipd) { }
    pd += omega;
    vpd = vpd * ppqn_f;
    dinphase = vpd; // store this as a phase that cycles from 0 to 47.999
    vpd -= floor(vpd);
    if (lvpd > vpd)
    {
        tick = true;
        ++tickN; // starts at 0..
    }
    lvpd = vpd;
    return tick;
}

void LegacyClock::reset()
{
    // more than 1.0
    lvpd = rec03;
    pd = 0.0;
    vpd = 0.0;
    dinphase = 0.0;
    tickN = -1;
}

void LegacyClock::zero()
{
    pd = dinphase / (ppqn_f);
    dinphase = dinphase - vpd;
    pd -= omega;
    lvpd = -0.3;
}

double LegacyClock::getBpm() const
{
    return bpm;
}
