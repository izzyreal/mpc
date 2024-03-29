#include <engine/audio/mixer/MixerControls.hpp>

#include <engine/audio/mixer/BusControls.hpp>

#include <engine/audio/mixer/MixerControlsFactory.hpp>
#include <engine/audio/mixer/MixerControlsIds.hpp>
#include <engine/control/CompoundControl.hpp>
#include <engine/control/Control.hpp>

#include <algorithm>

using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::core;
using namespace mpc::engine;
using namespace std;

MixerControls::MixerControls(string name, string mainBusName)
	: CompoundControl(1, name)
{
	mainBusControls = make_shared<BusControls>(MixerControlsIds::MAIN_BUS, mainBusName);
}

float MixerControls::getSmoothingFactor()
{
    return smoothingFactor;
}

void MixerControls::createAuxBusControls(string name)
{
	if (!canAddBusses) {
		return;
	}
	auto busControls = make_shared<BusControls>(MixerControlsIds::AUX_BUS, name);
	auxBusControls.push_back(std::move(busControls));
}

shared_ptr<BusControls> MixerControls::getBusControls(string name)
{
	auto mbc = mainBusControls;
	
	auto mbcName = mbc->getName();

	if (std::mismatch(mbcName.begin(), mbcName.end(), name.begin()).first == mbcName.end()) {
		return mainBusControls;
	}

	for (auto& busControls : auxBusControls) {
		if (busControls->getName() == name) {
			return busControls;
		}
	}
	return {};
}

shared_ptr<BusControls> MixerControls::getMainBusControls()
{
	return mainBusControls;
}

vector<shared_ptr<BusControls>> MixerControls::getAuxBusControls()
{
	return auxBusControls;
}

shared_ptr<AudioControlsChain> MixerControls::createStripControls(int id, string name)
{
    return createStripControls(id, name, true);
}

shared_ptr<AudioControlsChain> MixerControls::createStripControls(int id, string name, bool hasMixControls)
{
	if (getStripControls(name)) {
		return {};
	}
	auto chain = std::make_shared<AudioControlsChain>(id, name);
	MixerControlsFactory::addMixControls(this, chain, hasMixControls);
	addStripControls(chain);
	return chain;
}

void MixerControls::addStripControls(shared_ptr<CompoundControl> cc)
{
	canAddBusses = false;
	add(cc);
}

shared_ptr<AudioControlsChain> MixerControls::getStripControls(string name)
{
	for (auto& c : getControls()) {
		if (c->getName() == name) {
			try {
				return dynamic_pointer_cast<AudioControlsChain>(c);
			}
			catch (const bad_cast&) {
			}
		}
	}
	return {};
}

FaderControl* MixerControls::createFaderControl()
{
    return new FaderControl();
}
