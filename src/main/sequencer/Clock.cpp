#include <sequencer/Clock.hpp>

using namespace mpc::sequencer;

const double Clock::ppqn_f = 96.0;
const double Clock::rec60 = (1.0 / 60.0);
const double Clock::rec03 = (10.0 / 3.0);

void Clock::init(double Fs)
{
	Fs_rec = 1.0 / Fs;
	set_bpm(120.0);
	zero();
	reset();
}

void Clock::set_bpm(const double &BPM)
{
	bpm = fabs(BPM);
	omega = ((bpm)* rec60 * Fs_rec);
}

bool Clock::proc()
{
	bool tick = false;
	double tmp = pd;
	pd -= floor(pd);
	if (tmp > pd) { 
		tickN = -1;
		cycleCount++;
	}
	vpd = pd; // get the value before the increment
			  // if (ilpd > ipd) { }
	pd += omega; lpd = pd;
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

void Clock::reset()
{
	lpd = rec03; // more than 1.0
	lvpd = rec03;
	pd = 0.0;
	vpd = 0.0;
	dinphase = 0.0;
	cycleCount = 0;
	tickN = -1;
	tickOffset = 0;
}

void Clock::zero()
{
	pd = dinphase / (ppqn_f);
	dinphase = dinphase - vpd;
	pd -= omega;
	lvpd = lpd = -0.3;
}

int Clock::getTickN() {
	return tickN;
}

int Clock::getTickPosition() {
	auto candidate = tickN + (cycleCount * 96) + tickOffset + 1;
	return candidate < 0 ? 0 : candidate;
}

void Clock::setTick(int n) {
	int pos = getTickPosition();
	int diff = (n - pos) - 1;
	tickOffset += diff;
}

double Clock::getBpm() {
	return bpm;
}
